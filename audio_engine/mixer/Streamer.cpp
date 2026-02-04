#include "Streamer.h"
#include <cstdint>

namespace mixer {

// --------------------
// Construction
// --------------------
Streamer::Streamer() {
  deviceManager.initialise(0, 2, nullptr, true);

  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);
}

Streamer::~Streamer() {
  stop();

  audioSourcePlayer.setSource(nullptr);
  deviceManager.removeAudioCallback(&audioSourcePlayer);

  mixer.releaseResources();
}

// --------------------
// AudioSource
// --------------------
void Streamer::prepareToPlay(int samplesPerBlock, double sampleRate) {
  currentSampleRate = sampleRate;
  mixer.prepareToPlay(samplesPerBlock, sampleRate);
}

void Streamer::releaseResources() { mixer.releaseResources(); }

double Streamer::getGlobalPositionSeconds() {
  if (currentSampleRate <= 0.0)
    return 0.0;

  return static_cast<double>(globalSamplePosition) / currentSampleRate;
}

double Streamer::getSumSamplesSeconds() {
  return static_cast<double>(sumTrackSamples) / currentSampleRate;
}

void Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  bufferToFill.clearActiveBufferRegion();

  int64_t blockStart = globalSamplePosition;
  int64_t blockEnd = globalSamplePosition + bufferToFill.numSamples;

  while (currentEventIndex < globalTimelineEvents.size()) {
    auto &ev = globalTimelineEvents[currentEventIndex];

    if (ev.startSample >= blockEnd)
      break;

    if (ev.startSample >= blockStart) {
      std::cout << "Starting " << ev.track->getSource() << '\n';
      auto *t = ev.track->getTransport();
      t->setPosition(0.0);
      mixer.addInputSource(t, false);
      t->start();
    }

    ++currentEventIndex;
  }

  mixer.getNextAudioBlock(bufferToFill);
  globalSamplePosition += bufferToFill.numSamples;
}

/*
void Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo&
bufferToFill)
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
void Streamer::timerCallback() {}

// --------------------
// Playback control
// --------------------
bool Streamer::addNext(AudioTrack &track) {
  if (!track.getTransport())
    return false;

  TimelineEvent new_track;
  new_track.track = &track;
  new_track.durationSamples = track.getTotalSamples(currentSampleRate);
  if (sumTrackSamples <= 0) {
    new_track.startSample = 0;
  } else {
    new_track.startSample =
        sumTrackSamples - track.getOverlapSamples(currentSampleRate);
  }

  globalTimelineEvents.push_back(new_track);

  sumTrackSamples += track.getTotalSamples(currentSampleRate) -
                     track.getOverlapSamples(currentSampleRate);

  std::sort(globalTimelineEvents.begin(), globalTimelineEvents.end(),
            [](auto &a, auto &b) { return a.startSample < b.startSample; });

  std::cout << "Queued: " << track.getSource() << " at "
            << new_track.startSample << '\n';

  return true;
}

void Streamer::start() {
  // if (trackList.empty())
  //     return;

  // auto* transport = globalTimelineEvents
  // if (!transport)
  //     return;

  // transport->setPosition(0.0);
  // mixer.addInputSource(transport, false);
  // transport->start();

  startTimerHz(10); // scheduler tick (not timing-critical)
}

void Streamer::stop() {
  stopTimer();

  // for (auto* track : trackList)
  // {
  //     if (auto* t = track->getTransport())
  //         t->stop();
  // }

  // mixer.removeAllInputs();
  // trackList.clear();
}

} // namespace mixer
