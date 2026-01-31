#include "Streamer.h" // Include the header for this class definition
#include "AudioTrack.h"

namespace mixer { // Put everything in the mixer namespace to avoid naming
                  // collisions

// ==========================
// Constructor
// ==========================
Streamer::Streamer(const AudioTrack &trackA,const AudioTrack &trackB) {
  formatManager.registerBasicFormats();
  trackA_file = juce::File(trackA.getSource());
  trackB_file = juce::File(trackB.getSource());

  if (!loadFile(trackA_file)) {
    juce::Logger::writeToLog("Failed to load MP3");
    //     return 1;
  }

  deviceManager.initialise(0, // no inputs
                           2, // stereo output
                           nullptr,
                           true, // select default device
                           {},   // preferred setup
                           nullptr);

  this->prepareToPlay(
      512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);
}

// ==========================
// Destructor
// ==========================
Streamer::~Streamer() {
  stopTimer();
  audioSourcePlayer.setSource(nullptr);
  deviceManager.removeAudioCallback(&audioSourcePlayer);
  transport.setSource(nullptr);
}

// ==========================
// Load an audio file (MP3/WAV/FLAC)
// ==========================
bool Streamer::loadFile(const juce::File &file) {
  // Check if the file exists on disk
  if (!file.existsAsFile()) {
    juce::Logger::writeToLog("File does not exist: " + file.getFullPathName());
    return false; // Return false if the file can't be found
  }

  // Create an AudioFormatReader for the file.
  // formatManager knows how to decode MP3, WAV, FLAC, etc.
  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));
  if (reader == nullptr) {
    juce::Logger::writeToLog("Failed to create AudioFormatReader for: " +
                             file.getFullPathName());

    // Optional debug info
    juce::Logger::writeToLog("File extension: " + file.getFileExtension());
    juce::Logger::writeToLog(
        "Supported formats: MP3, WAV, AIFF, FLAC (registerBasicFormats())");

    return false;
  }

  // Wrap the reader in an AudioFormatReaderSource, which is compatible with
  // AudioTransportSource
  readerSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));

  // Connect the reader source to the transport.
  // 0 = default read-ahead buffer, nullptr = default thread pool
  // sampleRate = ensures transport plays at correct speed
  transport.setSource(readerSource.get(), 0, nullptr,
                      readerSource->getAudioFormatReader()->sampleRate);

  // Log success
  juce::Logger::writeToLog("Successfully loaded: " + file.getFullPathName());
  return true;
}

int Streamer::addNext(const AudioTrack &track) {
  juce::File new_track = juce::File(track.getSource());
  if (!new_track.existsAsFile()) {
    juce::Logger::writeToLog("Failed to add new track to track list: " +
                             new_track.getFullPathName());
  }

  track_list.emplace_back(new_track);

  juce::Logger::writeToLog("Added new track to track list: " +
                           new_track.getFullPathName());
  return 0;
}

// ==========================
// Prepare to play (AudioSource override)
// ==========================
void Streamer::prepareToPlay(int samplesPerBlock, double sampleRate) {
  // Forward the call to the transport source
  // This allocates internal buffers and prepares for streaming audio
  transport.prepareToPlay(samplesPerBlock, sampleRate);
}

// ==========================
// Release resources (AudioSource override)
// ==========================
void Streamer::releaseResources() {
  // Free any buffers allocated in prepareToPlay
  transport.releaseResources();
}

// ==========================
// Query if the song is currently playing
// ==========================
bool Streamer::isPlaying() const {
  // Return the playing state of the transport source
  return transport.isPlaying();
}

// ==========================
// Start playback
// ==========================
void Streamer::start() {
  // Starts streaming audio from the beginning (or current transport position)
  transport.start();
  startTimerHz(10);
}

// ==========================
// Stop playback
// ==========================
void Streamer::stop() {
  // Stops audio playback immediately
  transport.stop();
}

// ==========================
// Fill audio buffer (AudioSource override)
// ==========================
void Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  // Let the transport source fill the output buffer for playback
  // bufferToFill contains pointers to output channels and sample ranges

  
  transport.getNextAudioBlock(bufferToFill);
}
void Streamer::timerCallback() {
  




  if (!transport.isPlaying()) {
    if (!track_list.empty()) {
      juce::File next_track = track_list.front();
      track_list.erase(track_list.begin());

      juce::Logger::writeToLog("Loading next track: " +
                               next_track.getFullPathName());

      transport.stop();
      transport.setSource(nullptr);

      if (loadFile(next_track))
        transport.start();
    } else {
      juce::Logger::writeToLog("Playlist finished");
      stopTimer();
    }
  }

}
}


/*
// Pseudo-code
if (crossfadeActive) {
    // Both buffers have audio
    for each channel c:
        for each sample i:
            output[i] = trackA[i] + trackB[i];
} else {
    // Only Track A
    transportA.getNextAudioBlock(outputBuffer);
}


Triggering the Overlap

Use your timer or check inside getNextAudioBlock():

if (!crossfadeActive && remainingSamples(trackA) <= crossfadeTime * sampleRate) {
    startTrackB();
    crossfadeActive = true;
}



*/