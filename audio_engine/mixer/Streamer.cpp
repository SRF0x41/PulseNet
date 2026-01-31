#include "Streamer.h" // Include the header for this class definition
#include "AudioTrack.h"

namespace mixer { // Put everything in the mixer namespace to avoid naming
                  // collisions

// ==========================
// Constructor
// ==========================
Streamer::Streamer(const AudioTrack &trackA, const AudioTrack &trackB) {
  formatManager.registerBasicFormats();
  trackA_file = juce::File(trackA.getSource());
  trackB_file = juce::File(trackB.getSource());

  if (!loadFile(trackA_file)) {
    juce::Logger::writeToLog("Failed to load track A");
  }

  if (!loadNextFile(trackB_file)) {
    juce::Logger::writeToLog("Failed to load track B");
  }

  // Setting up the system to play samples
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

bool Streamer::loadNextFile(const juce::File &file) {
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
  nextReaderSource.reset(
      new juce::AudioFormatReaderSource(reader.release(), true));

  // Connect the reader source to the transport.
  // 0 = default read-ahead buffer, nullptr = default thread pool
  // sampleRate = ensures transport plays at correct speed
  nextTransport.setSource(nextReaderSource.get(), 0, nullptr,
                          nextReaderSource->getAudioFormatReader()->sampleRate);

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
  nextTransport.prepareToPlay(samplesPerBlock, sampleRate);
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
// void Streamer::getNextAudioBlock(
//     const juce::AudioSourceChannelInfo &bufferToFill) {
//   // First, fill the main transport into the buffer
//   transport.getNextAudioBlock(bufferToFill);

//   // Calculate remaining seconds of the current track
//   double remainingSeconds =
//       transport.getLengthInSeconds() - transport.getCurrentPosition();

//   // Check if we should start crossfade
//   if (!crossfadeActive && remainingSeconds <= crossfadeDuration &&
//       nextReaderSource) {
//     juce::AudioSourceChannelInfo nextBuffer = bufferToFill;
//     nextTransport.getNextAudioBlock(nextBuffer); // fill next track buffer

//     int numSamples = bufferToFill.numSamples;

//     for (int channel = 0; channel < bufferToFill.buffer->getNumChannels();
//          ++channel) {
//       float *mainData = bufferToFill.buffer->getWritePointer(
//           channel, bufferToFill.startSample);
//       const float *nextData =
//           nextBuffer.buffer->getReadPointer(channel, nextBuffer.startSample);

//       for (int i = 0; i < numSamples; ++i) {
//         float crossfadePos =
//             static_cast<float>(i) / numSamples; // 0.0 -> 1.0 across this
//             block
//         mainData[i] =
//             mainData[i] * (1.0f - crossfadePos) + nextData[i] * crossfadePos;
//       }
//     }

//     crossfadeActive = true;  // mark that crossfade is in progress
//     crossfadeProgress = 0.0; // optional if tracking time across blocks
//   }

//   // If the main track finishes, swap in the next track
//   if (!transport.isPlaying() && crossfadeActive) {
//     transport.setSource(nextReaderSource.get());
//     readerSource = std::move(nextReaderSource);

//     transport.start();
//     crossfadeActive = false;
//     juce::Logger::writeToLog("Crossfade complete.");
//   }
// }

void Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    // Fill the main track into the output buffer
    transport.getNextAudioBlock(bufferToFill);

    if (crossfadeActive && nextReaderSource) {
        // Create a temporary buffer for the next track
        juce::AudioSourceChannelInfo nextBuffer = bufferToFill;
        nextTransport.getNextAudioBlock(nextBuffer); // fill next track buffer

        int numSamples = bufferToFill.numSamples;

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
            float* mainData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            const float* nextData = nextBuffer.buffer->getReadPointer(channel, nextBuffer.startSample);

            // Just add the samples of the next track on top of the main track
            for (int i = 0; i < numSamples; ++i) {
                mainData[i] += nextData[i]; // overlap without changing volume
            }
        }
    }
}

void Streamer::timerCallback() {
  // How many seconds are left in the current track
  double remainingSeconds =
      transport.getLengthInSeconds() - transport.getCurrentPosition();
  std::cout << remainingSeconds << '\n';
  if (remainingSeconds <= crossfadeDuration) {
    // nextTransport.start();
    nextTransport.setPosition(0.0); // start at beginning
    nextTransport.start();
    crossfadeActive = true;
  }

  // Trigger crossfade if not already active and near the end
  // if (!crossfadeActive && nextReaderSource &&
  //     remainingSeconds <= crossfadeDuration) {
  //   juce::Logger::writeToLog("Starting crossfade with next track");

  //   nextTransport.start();   // Start the next track
  //   crossfadeActive = true;  // Flag for getNextAudioBlock to mix buffers
  //   crossfadeProgress = 0.0; // Optional: reset crossfade progress
  // }

  // // If main track finished, swap next track in
  // if (!transport.isPlaying() && crossfadeActive) {
  //   transport.setSource(
  //       nextReaderSource.get()); // Make next track the main transport
  //   readerSource = std::move(nextReaderSource); // Move ownership
  //   transport.start();                          // Start playback

  //   crossfadeActive = false;
  //   juce::Logger::writeToLog("Crossfade complete.");
  // }
}

} // namespace mixer

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

if (!crossfadeActive && remainingSamples(trackA) <= crossfadeTime * sampleRate)
{ startTrackB(); crossfadeActive = true;
}



*/