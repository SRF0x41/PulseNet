#pragma once

#include <cstdint>
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

    // Global timeline
    double getGlobalPositionSeconds();
    double getSumSamplesSeconds();

private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    juce::MixerAudioSource mixer;

    //std::vector<AudioTrack*> trackList;



    // sample-accurate scheduling
    double currentSampleRate = 44100.0;
    

    // Global sample timeline scheduling
    int64_t globalSamplePosition = 0;
    int64_t sumTrackSamples = -1;

    struct TimelineEvent {
        int64_t startSample;        // When this event triggers
        AudioTrack* track;          // Which track
        //enum EventType { START, FADE_IN, FADE_OUT } type;
        double durationSamples;     // Optional: for fades
    };

    size_t currentEventIndex = 0;
    std::vector<TimelineEvent> globalTimelineEvents;

};

} // namespace mixer
