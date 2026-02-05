#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <vector>
#include <iostream>

#include "AudioTrack.h"

namespace mixer {

class Streamer : public juce::AudioSource,
                 public juce::Timer
{
public:
    Streamer();
    ~Streamer() override;

    // AudioSource
    void prepareToPlay(int samplesPerBlock,
                       double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(
        const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Playback control
    bool addNext(AudioTrack& track);
    void start();
    void stop();

    // Timer
    void timerCallback() override;


private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::MixerAudioSource mixer;

    std::vector<AudioTrack*> trackList;


    // sample-accurate scheduling
    bool nextScheduled = false;
    int64_t samplesUntilNextStart = -1;
    double currentSampleRate = 44100.0;


};

} // namespace mixer
