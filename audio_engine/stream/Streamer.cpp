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
  for (auto &fade : eventTimeline.fadeTimeline) {
    if (!fade.fadeStatus)
      continue;

    auto *transport = fade.track->getTransport();

    float currentGain = transport->getGain();
    float fadeChange = fade.fadeRate * bufferToFill.numSamples;
    float editedGain = juce::jlimit(0.0f, 1.0f, currentGain + fadeChange);

    transport->setGain(editedGain);

    fade.fadeSamplesRemaining -= bufferToFill.numSamples;

    if (fade.fadeSamplesRemaining <= 0) {
      transport->setGain(fade.fadeType == FadeState::FADE_IN ? 1.0f : 0.0f);
      fade.fadeStatus = false;
      fade.eventTriggered = true;
    }
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

        mixer.addInputSource(t, false);
        t->start();
        break;

      case TimelineEvent::FADE_IN:
        std::cout << "FADE_IN\n";
        eventTimeline.startFade(event);
        break;

      case TimelineEvent::FADE_OUT:
        std::cout << "FADE_OUT\n";
        eventTimeline.startFade(event);
        break;

      case TimelineEvent::STOP:
        std::cout << "STOP\n";
        // defer stop to avoid audio glitches
        event->eventTriggered = true;
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
}

/*

*/

// --------------------
// Timer (scheduler)
// --------------------
void Streamer::timerCallback() {
  auto& fades = eventTimeline.fadeTimeline;

  fades.erase(
      std::remove_if(fades.begin(), fades.end(),
          [](const FadeState& fade) {
              return fade.eventTriggered;
          }),
      fades.end());
}


// --------------------
// Playback control
// --------------------
bool Streamer::addNext(std::unique_ptr<AudioTrack> track) {
  AudioTrack *raw = track.get(); // stable pointer
  tracks.push_back(std::move(track));
  return eventTimeline.addTrack(*raw);
}

void Streamer::start() {

  startTimerHz(10); // scheduler tick (not timing-critical)

  
}

void Streamer::stop() {
  stopTimer();

  mixer.removeAllInputs();
}

} // namespace mixer
