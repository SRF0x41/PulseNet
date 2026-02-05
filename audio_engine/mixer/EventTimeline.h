#pragma once
#include "AudioTrack.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>

class EventTimeline {
public:
  EventTimeline(double sampleRate);
  ~EventTimeline();

  bool addTrack(AudioTrack &track);

  juce::AudioTransportSource* getEvent();


private:
  double currentSampleRate;
  struct TimelineEvent {
    int64_t startSample; // When this event triggers
    AudioTrack *track;   // Which track
    // enum EventType { START, FADE_IN, FADE_OUT } type;
    double durationSamples; // Optional: for fades
  };
  int64_t sumTrackSamples = -1;
  std::vector<TimelineEvent> timelineEvents;

};