#pragma once
#include "AudioTrack.h"
#include "TimelineEvent.h"
#include <cstddef>
#include <cstdint>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>

class EventTimeline {
public:
  EventTimeline(double sampleRate);
  ~EventTimeline();

  bool addTrack(AudioTrack &track);

  TimelineEvent* getEvent( int64_t endBlock);

  void startFade(TimelineEvent *event);
  std::vector<FadeState> fadeTimeline;
  

private:
  double currentSampleRate;

  int64_t sumTrackSamples = -1;
  std::vector<TimelineEvent> timeline;
  size_t eventIndex = 0;
  size_t fadeIndex = 0;

  
};