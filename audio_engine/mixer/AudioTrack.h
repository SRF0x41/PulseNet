#pragma once

#include <string>
#include <memory>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>



// ====================
// AudioTrack
// ====================
// Represents a single playable audio item.
// Owns its transport and reader lifetime.
// Does NOT own audio devices.
//
class AudioTrack {
public:
    // --------------------
    // Construction
    // --------------------
    explicit AudioTrack(const std::string& sourcePath);

    // --------------------
    // Accessors
    // --------------------
    const std::string& getSource() const;

    float getBpm() const;
    void setBpm(float newBpm);

    float getRemainingTime() const;
    void setRemainingTime(float time);

    juce::AudioTransportSource* getTransport();

    // --------------------
    // Transport control
    // --------------------
    void startTransport();
    void stopTransport();

    // overlap control
    void setStartOverlapSeconds(double seconds);
    double getStartOverlapSeconds() const;

    // basic crossfading
    void setStartTransitionDuration(double seconds);
    void setEndTransitionDuration(double seconds);

    // Global sample timeline

    int64_t getTotalSamples(double sampleRate);


private:
    // --------------------
    // Properties
    // --------------------
    const std::string source;   // File path
    float bpm { 0.0f };
    float remainingTime { 0.0f };

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transport;


    // --------------------
    // Mix Properties
    // --------------------
    double startOverlapSeconds = 0.0; 
    float gain = 1.0f; // default full volume

    // basic crossfading
    double begin_transitionDuration = 0.0;
    double end_transitionDuration = 0.0;


    // Global sample scheduling
    double currentSampleRate = 44100.0;




};
