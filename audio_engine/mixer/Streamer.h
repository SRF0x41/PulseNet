#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>
#include <memory>
#include "AudioTrack.h"

namespace mixer {

class Streamer : public juce::AudioSource, public juce::Timer {
public:
    Streamer(const AudioTrack &trackA, const AudioTrack &trackB);
    ~Streamer() override;

    int addNext(const AudioTrack &track);
    void start();
    void stop();
    bool isPlaying() const;

    // AudioSource overrides
    void prepareToPlay(int samplesPerBlock, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    // Timer callback
    void timerCallback() override;

private:
    bool loadFile(const juce::File &file);
    bool loadNextFile(const juce::File &file);

    juce::AudioDeviceManager deviceManager;
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transport;
    juce::AudioTransportSource nextTransport;

    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    std::unique_ptr<juce::AudioFormatReaderSource> nextReaderSource;


 

    juce::AudioSourcePlayer audioSourcePlayer;

    std::vector<juce::File> track_list;

    
    double crossfadeDuration = 30.0;
    
    bool crossfadeActive = false;

    juce::File trackA_file;
    juce::File trackB_file;



    juce::MixerAudioSource mixer;
};

} // namespace mixer
