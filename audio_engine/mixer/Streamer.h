#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace mixer {
class Streamer : public juce::AudioSource {
public:
  Streamer(const std::string &trackA);
  ~Streamer();

  bool loadFile(const juce::File &file);
  void prepareToPlay(int samplesPerBlock, double sampleRate);
  void releaseResources();
  bool isPlaying() const; // <-- add this

  void start();
  void stop();

  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);

private:
  juce::AudioFormatManager formatManager;

  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transport;

  std::string trackA_path;
  juce::File trackA_file;

  std::string trackB_path;
  juce::File trackB_file;
  float masterVolume;

  juce::AudioDeviceManager deviceManager;
};
} // namespace mixer

/*

// ====================
// Streamer object
// ====================

Desc:
    The Streamer controls continuous playback.
    It manages two tracks at once so transitions
    (crossfades, cuts, gaps) can happen seamlessly.

class Streamer
    properties:
        trackA              // AudioTrack (currently audible / master)
        trackB              // AudioTrack (next or secondary track)

        activeTrack         // enum { A, B }
        transitionType      // enum { NONE, CUT, CROSSFADE }

        transitionProgress  // float 0.0 → 1.0
        transitionDuration  // time in seconds

        masterVolume        // float 0.0 → 1.0
        isPlaying           // bool

    methods:

        start()
            starts playback of the active track

        stop()
            stops all playback immediately

        loadInitialTrack(track)
            trackA = track
            activeTrack = A

        next(track, transition)
            prepares the next track
            configures how the transition should occur

        update(deltaTime)
            advances playback and handles transitions
            called periodically (e.g. every audio block or tick)

        applyTransition()
            adjusts track volumes based on transition type

        swapTracks()
            makes trackB the new master (trackA)

        isFinished() -> bool
            returns true if no tracks are playing

// ====================
// AudioTrack object
// ====================

class AudioTrack
    properties:
        source          // file, stream, decoder
        volume          // float
        isPlaying       // bool
        remainingTime   // seconds

    methods:
        play()
        stop()
        setVolume(value)
        update(deltaTime)


*/
