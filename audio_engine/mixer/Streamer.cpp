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

   

    mixer.getNextAudioBlock(bufferToFill);
}

// --------------------
// Timer (scheduler)
// --------------------
void Streamer::timerCallback()
{
    

    
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
