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
};
