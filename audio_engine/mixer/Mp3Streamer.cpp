#include "Mp3Streamer.h" // Include the header for this class definition

namespace mixer {      // Put everything in the mixer namespace to avoid naming collisions

// ==========================
// Constructor
// ==========================
Mp3Streamer::Mp3Streamer() {
    // Register standard audio formats (MP3, WAV, AIFF, FLAC, etc.)
    // This allows the AudioFormatManager to create readers for these types.
    formatManager.registerBasicFormats();
}

// ==========================
// Destructor
// ==========================
Mp3Streamer::~Mp3Streamer() {
    // Detach the transport source from any reader to safely release resources.
    // This ensures no dangling pointers remain when object is destroyed.
    transport.setSource(nullptr);
}

// ==========================
// Load an audio file (MP3/WAV/FLAC)
// ==========================
bool Mp3Streamer::loadFile(const juce::File &file) {
    // Check if the file exists on disk
    if (!file.existsAsFile()) {
        juce::Logger::writeToLog("File does not exist: " + file.getFullPathName());
        return false; // Return false if the file can't be found
    }

    // Create an AudioFormatReader for the file.
    // formatManager knows how to decode MP3, WAV, FLAC, etc.
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(file));

    // If we couldn't create a reader (unsupported format, corrupted file, etc.)
    if (reader == nullptr) {
        juce::Logger::writeToLog("Failed to create AudioFormatReader for: " +
                                 file.getFullPathName());

        // Optional debug info
        juce::Logger::writeToLog("File extension: " + file.getFileExtension());
        juce::Logger::writeToLog(
            "Supported formats: MP3, WAV, AIFF, FLAC (registerBasicFormats())");

        return false;
    }

    // Wrap the reader in an AudioFormatReaderSource, which is compatible with AudioTransportSource
    readerSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));

    // Connect the reader source to the transport.
    // 0 = default read-ahead buffer, nullptr = default thread pool
    // sampleRate = ensures transport plays at correct speed
    transport.setSource(readerSource.get(),
                        0,
                        nullptr,
                        readerSource->getAudioFormatReader()->sampleRate);

    // Log success
    juce::Logger::writeToLog("Successfully loaded: " + file.getFullPathName());
    return true;
}

// ==========================
// Prepare to play (AudioSource override)
// ==========================
void Mp3Streamer::prepareToPlay(int samplesPerBlock, double sampleRate) {
    // Forward the call to the transport source
    // This allocates internal buffers and prepares for streaming audio
    transport.prepareToPlay(samplesPerBlock, sampleRate);
}

// ==========================
// Release resources (AudioSource override)
// ==========================
void Mp3Streamer::releaseResources() {
    // Free any buffers allocated in prepareToPlay
    transport.releaseResources();
}

// ==========================
// Query if the song is currently playing
// ==========================
bool Mp3Streamer::isPlaying() const {
    // Return the playing state of the transport source
    return transport.isPlaying();
}

// ==========================
// Start playback
// ==========================
void Mp3Streamer::start() {
    // Starts streaming audio from the beginning (or current transport position)
    transport.start();
}

// ==========================
// Stop playback
// ==========================
void Mp3Streamer::stop() {
    // Stops audio playback immediately
    transport.stop();
}

// ==========================
// Fill audio buffer (AudioSource override)
// ==========================
void Mp3Streamer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    // Let the transport source fill the output buffer for playback
    // bufferToFill contains pointers to output channels and sample ranges
    transport.getNextAudioBlock(bufferToFill);
}

} // namespace mixer
