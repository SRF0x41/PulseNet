#pragma once

#include <string>

// ====================
// AudioTrack
// ====================
// Represents a single playable audio item.
// Owns playback state but not the audio device.
//
// This is a logical model, not a decoder implementation.
//
class AudioTrack
{
public:
    // --------------------
    // Construction
    // --------------------
    AudioTrack(const std::string& sourcePath);

    // --------------------
    // Playback control
    // --------------------
    void play();                     // Start playback
    void stop();                     // Stop playback immediately
    void setVolume(float value);     // Set volume [0.0 - 1.0]

    // --------------------
    // Update
    // --------------------
    void update(float deltaTime);    // Advance playback time

    // --------------------
    // State queries
    // --------------------
    bool isPlaying() const;
    float getRemainingTime() const;
    float getVolume() const;
    const std::string& getSource() const;

private:
    // --------------------
    // Properties
    // --------------------
    std::string source;      // File path or stream identifier
    float volume;            // Linear gain (0.0 - 1.0)
    bool playing;            // Playback state
    float remainingTime;     // Seconds remaining
};
