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
    // crossfade / scheduling logic goes here later
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
    startTimerHz(10);

    for (auto* track : trackList)
    {
        if (auto* t = track->getTransport())
            t->start();
    }
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
