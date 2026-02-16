#pragma once

#include <cstdint>
#include <iostream>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <vector>

#include "AudioTrack.h"
#include "EventTimeline.h"

namespace mixer {

class Streamer : public juce::AudioSource, public juce::Timer {
public:
  Streamer();
  ~Streamer() override;

  // AudioSource
  void prepareToPlay(int samplesPerBlock, double sampleRate) override;
  void releaseResources() override;
  void
  getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

  // Playback control
  bool addNext(std::unique_ptr<AudioTrack> track);
  void start();
  void stop();

  // Timer
  void timerCallback() override;

  // Global timeline
  double getGlobalPositionSeconds();

  void pause();
  void resume();

  void skipNext();

private:
  juce::AudioDeviceManager deviceManager;
  juce::AudioSourcePlayer audioSourcePlayer;
  juce::MixerAudioSource mixer;

  // std::vector<AudioTrack*> trackList;

  // sample-accurate scheduling
  double currentSampleRate = 44100.0;

  // Global sample timeline scheduling
  int64_t globalSamplePosition = 0;

  EventTimeline eventTimeline;

  std::vector<std::unique_ptr<AudioTrack>> tracks;

  bool pauseStatus = false;
  bool requestSkip = false;
};

} // namespace mixer
