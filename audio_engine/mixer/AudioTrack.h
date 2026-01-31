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
class AudioTrack {
public:
  // --------------------
  // Construction
  // --------------------
  AudioTrack(const std::string &sourcePath);

private:
  // --------------------
  // Properties
  // --------------------
  const std::string source;  // File path or stream identifier
  float remainingTime; // Seconds remaining
};
