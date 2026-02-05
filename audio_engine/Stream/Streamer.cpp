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

  // Hande fades here
  /*struct FadeState {
  bool fadeStatus;
  enum FadeType { FADE_IN, FADE_OUT } fadeType;
  AudioTrack *track;
  int64_t fadeSamplesRemaining = 0;
  float fadeRate;
  };
  */

  // First, process active fades
  for (int i = 0; i < eventTimeline.fadeTimeline.size(); ++i) {
    AudioTrack *track = eventTimeline.fadeTimeline[i].track;

    float currentGain = track->getGain();
    float editedGain =
        currentGain - eventTimeline.fadeTimeline[i].track->getFadeOutGainRate();
    editedGain = juce::jlimit(0.0f, 1.0f, editedGain);
    track->setGain(editedGain);

    // --- Debug print ---
    std::cout << "[FADE] Track: " << track->getSource() << " | Fade type: "
              << (eventTimeline.fadeTimeline[i].fadeType == FadeState::FADE_IN
                      ? "IN"
                      : "OUT")
              << " | Gain: " << editedGain << " | Samples left: "
              << eventTimeline.fadeTimeline[i].fadeSamplesRemaining << "\n";
  }

  // Now handle timeline events
  TimelineEvent *event = nullptr;
  do {
    event = eventTimeline.getEvent(blockEnd);

    if (event) {
      auto *t = event->track->getTransport();
      AudioTrack *track = event->track;

      std::cout << "[EVENT] Track: " << track->getSource() << " | Event type: ";

      switch (event->type) {
      case TimelineEvent::START:
        std::cout << "START\n";
        t->setPosition(0.0);
        mixer.addInputSource(t, false);
        t->start();
        break;

      case TimelineEvent::FADE_IN:
        std::cout << "FADE_IN\n";
        eventTimeline.startFade(event);
        break;

      case TimelineEvent::FADE_OUT:
        std::cout << "FADE_OUT\n";
        // optionally arm fade here if not already active
        break;

      case TimelineEvent::STOP:
        std::cout << "STOP\n";
        // defer stop to avoid audio glitches
        break;

      case TimelineEvent::REMOVE_FROM_MIXER:
        std::cout << "REMOVE_FROM_MIXER\n";
        mixer.removeInputSource(t);
        break;

      case TimelineEvent::END_TRACKLIST:
        std::cout << "END_TRACKLIST\n";
        break;

      default:
        std::cout << "UNKNOWN\n";
        break;
      }

      std::cout << "Event sample: " << event->eventSample
                << " | Block end: " << blockEnd << "\n";
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
