#include "Streamer.h"
#include <cstdint>

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
    currentSampleRate = sampleRate;
    mixer.prepareToPlay(samplesPerBlock, sampleRate);
}

void Streamer::releaseResources()
{
    mixer.releaseResources();
}

void Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    const int numSamples = bufferToFill.numSamples;

    // === Sample-accurate scheduler ===
    if (nextScheduled && samplesUntilNextStart >= 0)
    {
        if (samplesUntilNextStart < numSamples)
        {
            if (currentIndex + 1 < (int)trackList.size())
            {
                AudioTrack* nextTrack =
                    trackList[currentIndex + 1];

                if (auto* nextTransport = nextTrack->getTransport())
                {
                    nextTransport->setPosition(0.0);
                    mixer.addInputSource(nextTransport, false);
                    nextTransport->start();

                    std::cout << "Sample-accurate start: "
                              << nextTrack->getSource() << '\n';
                }
            }

            samplesUntilNextStart = -1;
        }
        else
        {
            samplesUntilNextStart -= numSamples;
        }
    }

    mixer.getNextAudioBlock(bufferToFill);
}

// --------------------
// Timer (scheduler)
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

    // Schedule NEXT track based on NEXT track's rule
    if (!nextScheduled && currentIndex + 1 < (int)trackList.size())
    {
        AudioTrack* nextTrack = trackList[currentIndex + 1];
        const double overlapSeconds = nextTrack->getStartOverlapSeconds();
        const double secondsUntilNextStart = currentTransport->getLengthInSeconds() - overlapSeconds;
        samplesUntilNextStart = static_cast<int64_t>(secondsUntilNextStart*currentSampleRate);
        nextScheduled = true;

        std::cout << "Scheduled next track in "
                      << secondsUntilNextStart
                      << " seconds\n";
        
    }

    // Advance playlist when current ends
    if (!currentTransport->isPlaying())
    {
        mixer.removeInputSource(currentTransport);

        currentIndex++;
        nextScheduled = false;
        samplesUntilNextStart = -1;

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
    if (!track.getTransport())
        return false;

    trackList.push_back(&track);

    std::cout << "Queued: "
              << track.getSource() << '\n';
    return true;
}

void Streamer::start()
{
    if (trackList.empty())
        return;

    currentIndex = 0;
    nextScheduled = false;
    samplesUntilNextStart = -1;

    auto* transport = trackList[0]->getTransport();
    if (!transport)
        return;

    transport->setPosition(0.0);
    mixer.addInputSource(transport, false);
    transport->start();

    startTimerHz(10); // scheduler tick (not timing-critical)
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
