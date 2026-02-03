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


double Streamer::getGlobalPositionSeconds()
{
    if (currentSampleRate <= 0.0)
        return 0.0;

    return static_cast<double>(globalSamplePosition) / currentSampleRate;
}


void Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    
    bufferToFill.clearActiveBufferRegion();

   
    globalSamplePosition += bufferToFill.numSamples;
    mixer.getNextAudioBlock(bufferToFill);
}

/*
void Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    const int numSamples = bufferToFill.numSamples;
    const int64_t bufferStart = globalSamplePosition;
    const int64_t bufferEnd   = bufferStart + numSamples;

    bufferToFill.clearActiveBufferRegion();

    // Check each track
    for (auto* track : trackList)
    {
        if (!track->isPlaying())
        {
            int64_t startSample = track->getScheduledStartSample();
            if (startSample >= bufferStart && startSample < bufferEnd)
            {
                mixer.addInputSource(track->getTransport(), false);
                track->startTransport();
            }
        }
    }

    globalSamplePosition += numSamples;
    mixer.getNextAudioBlock(bufferToFill);
}

or 

struct TimelineEvent {
    int64_t startSample;        // When this event triggers
    AudioTrack* track;          // Which track
    enum EventType { START, FADE_IN, FADE_OUT } type;
    double durationSamples;     // Optional: for fades
};

std::vector<TimelineEvent> timeline;
size_t currentEventIndex = 0;
int64_t globalSamplePosition = 0;


void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    int64_t blockStart = globalSamplePosition;
    int64_t blockEnd = globalSamplePosition + bufferToFill.numSamples;

    // Check all events that occur in this block
    while (currentEventIndex < timeline.size() &&
           timeline[currentEventIndex].startSample < blockEnd) {

        auto& ev = timeline[currentEventIndex];

        int64_t offsetInBlock = ev.startSample - blockStart;

        if (offsetInBlock >= 0 && offsetInBlock < bufferToFill.numSamples) {
            switch(ev.type) {
                case TimelineEvent::START:
                    ev.track->getTransport()->setPosition(0.0);
                    mixer.addInputSource(ev.track->getTransport(), false);
                    ev.track->startTransport();
                    break;

                case TimelineEvent::FADE_IN:
                    ev.track->startFadeIn(ev.durationSamples); 
                    break;

                case TimelineEvent::FADE_OUT:
                    ev.track->startFadeOut(ev.durationSamples); 
                    break;
            }
        }

        currentEventIndex++;
    }

    mixer.getNextAudioBlock(bufferToFill);
    globalSamplePosition += bufferToFill.numSamples;
}


*/

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
