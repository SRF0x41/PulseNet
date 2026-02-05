#include "Streamer.h"
#include "AudioTrack.h"
#include "TimelineEvent.h"
#include <cstdint>

namespace mixer {

// --------------------
// Construction
// --------------------
Streamer::Streamer() : eventTimeline(currentSampleRate) {
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

void Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  bufferToFill.clearActiveBufferRegion();

  int64_t blockStart = globalSamplePosition;
  int64_t blockEnd = globalSamplePosition + bufferToFill.numSamples;

  TimelineEvent *event = nullptr;

  // Hande fades here

  do {

    event = eventTimeline.getEvent(blockEnd);

    if (event) {

      auto *t = event->track->getTransport();
      AudioTrack *track = event->track;
      switch (event->type) {

      case TimelineEvent::START: {
        t->setPosition(0.0);
        mixer.addInputSource(t, false);
        t->start();
        break;
      }

      case TimelineEvent::FADE_IN: {

        break;
      }

      case TimelineEvent::FADE_OUT: {

        break;
      }

      case TimelineEvent::STOP: {
        // defer actual stop to avoid glitches
        break;
      }

      case TimelineEvent::REMOVE_FROM_MIXER: {
        mixer.removeInputSource(t);
        break;
      }

      case TimelineEvent::END_TRACKLIST: {
        break;
      }

      default:
        break;
      }
    }
  } while (event);
  mixer.getNextAudioBlock(bufferToFill);

  globalSamplePosition += bufferToFill.numSamples;

  //   int64_t blockStart = globalSamplePosition;
  //   int64_t blockEnd = globalSamplePosition + bufferToFill.numSamples;

  //   while (currentEventIndex < globalTimelineEvents.size()) {
  //     auto &ev = globalTimelineEvents[currentEventIndex];

  //     if (ev.startSample >= blockEnd)
  //       break;

  //     if (ev.startSample >= blockStart) {
  //       std::cout << "Starting " << ev.track->getSource() << '\n';
  //       auto *t = ev.track->getTransport();
  //       t->setPosition(0.0);
  //       mixer.addInputSource(t, false);
  //       t->start();
  //     }

  //     ++currentEventIndex;
  //   }

  //   mixer.getNextAudioBlock(bufferToFill);
  //   globalSamplePosition += bufferToFill.numSamples;
}

/*

*/

// --------------------
// Timer (scheduler)
// --------------------
void Streamer::timerCallback() {}

// --------------------
// Playback control
// --------------------
bool Streamer::addNext(AudioTrack &track) {
  return eventTimeline.addTrack(track);
}

void Streamer::start() {

  startTimerHz(10); // scheduler tick (not timing-critical)
}

void Streamer::stop() {
  stopTimer();

  mixer.removeAllInputs();
}

} // namespace mixer
