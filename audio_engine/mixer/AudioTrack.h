#pragma once

#include <string>

// ====================
// AudioTrack
// ====================
// Represents a single playable audio item.
// Owns playback state but not the audio device.
// This is a logical model, not a decoder implementation.
//
class AudioTrack {
public:
    // --------------------
    // Construction
    // --------------------
    AudioTrack(const std::string &sourcePath)
        : source(sourcePath), bpm(0.0f), remainingTime(0.0f)
    {}

    // --------------------
    // Accessors
    // --------------------
    const std::string& getSource() const { return source; } // Returns file path
    float getBpm() const { return bpm; }                    // Get BPM
    void setBpm(float newBpm) { bpm = newBpm; }            // Set BPM
    float getRemainingTime() const { return remainingTime; } // Get remaining time in seconds
    void setRemainingTime(float time) { remainingTime = time; } // Set remaining time

private:
    // --------------------
    // Properties
    // --------------------
    const std::string source;  // File path or stream identifier
    float bpm;                 // Beats per minute
    float remainingTime;       // Seconds remaining
};
