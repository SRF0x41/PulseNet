#pragma once
#include "AudioTrack.h"
#include <cstdint>
struct TimelineEvent {

  int64_t eventSample; // When this event triggers
  AudioTrack *track;   // Which track
  enum EventType {
    START,
    STOP,
    FADE_IN,
    FADE_OUT,
    REMOVE_FROM_MIXER,
    END_TRACKLIST
  } type;
};

struct FadeState {
  bool fadeStatus;
  enum FadeType { FADE_IN, FADE_OUT } fadeType;
  AudioTrack *track;
  int64_t fadeSamplesRemaining = 0;
  float fadeRate;
  bool removeFadeState = false;
};
