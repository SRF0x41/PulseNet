#include "Streamer.h"

namespace mixer {

// --------------------
// Construction
// --------------------
Streamer::Streamer()
{
    deviceManager.initialise(0, 2, nullptr, true);

    audioSourcePlayer.setSource(this);
    deviceManager.addAudioCallback(&audioSourcePlayer);
}

Streamer::~Streamer()
{
    stop();

    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);

    mixer.releaseResources();
}

// --------------------
// AudioSource
// --------------------
void Streamer::prepareToPlay(int samplesPerBlock,
                             double sampleRate)
{
    mixer.prepareToPlay(samplesPerBlock, sampleRate);
}

void Streamer::releaseResources()
{
    mixer.releaseResources();
}

void Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    mixer.getNextAudioBlock(bufferToFill);
}

// --------------------
// Timer
// --------------------
void Streamer::timerCallback()
{
    if (currentIndex < 0 ||
        currentIndex >= (int)trackList.size())
        return;

    AudioTrack* currentTrack = trackList[currentIndex];
    auto* currentTransport = currentTrack->getTransport();

    if (!currentTransport)
        return;

    const double remaining =
        currentTransport->getLengthInSeconds()
        - currentTransport->getCurrentPosition();

    // Check if there's a NEXT track
    if (!nextStarted &&
        currentIndex + 1 < (int)trackList.size())
    {
        AudioTrack* nextTrack = trackList[currentIndex + 1];
        const double overlap =
            nextTrack->getStartOverlapSeconds();

        // NEXT track decides when it enters
        if (remaining <= overlap)
        {
            auto* nextTransport = nextTrack->getTransport();
            if (nextTransport)
            {
                nextTransport->setPosition(0.0);
                mixer.addInputSource(nextTransport, false);
                nextTransport->start();

                nextStarted = true;

                std::cout << "Next track entered ("
                          << overlap << "s rule): "
                          << nextTrack->getSource() << '\n';
            }
        }
    }

    // Remove finished track
    if (!currentTransport->isPlaying())
    {
        mixer.removeInputSource(currentTransport);

        currentIndex++;
        nextStarted = false;

        if (currentIndex >= (int)trackList.size())
        {
            stopTimer();
            return;
        }

        std::cout << "Now primary: "
                  << trackList[currentIndex]->getSource() << '\n';
    }
}





// --------------------
// Playback control
// --------------------
bool Streamer::addNext(AudioTrack& track)
{
    auto* transport = track.getTransport();
    if (!transport)
        return false;

    transport->setPosition(0.0);
    transport->setGain(1.0f);

    mixer.addInputSource(transport, false);
    trackList.push_back(&track);

    std::cout << "Queued: " << track.getSource() << '\n';
    return true;
}

void Streamer::start()
{
    if (trackList.empty())
        return;

    currentIndex = 0;

    auto* transport = trackList[currentIndex]->getTransport();
    if (!transport)
        return;

    transport->setPosition(0.0);
    transport->start();

    startTimerHz(10); // scheduler tick
}


void Streamer::stop()
{
    stopTimer();

    for (auto* track : trackList)
    {
        if (auto* t = track->getTransport())
            t->stop();
    }

    mixer.removeAllInputs();
    trackList.clear();
}

} // namespace mixer
