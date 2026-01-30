#include "Mp3Streamer.h"

namespace mixer {
Mp3Streamer::Mp3Streamer() {
  formatManager.registerBasicFormats(); // MP3, WAV, etc
}

Mp3Streamer::~Mp3Streamer() { transport.setSource(nullptr); }

bool Mp3Streamer::loadFile(const juce::File &file) {
  if (!file.existsAsFile()) {
    juce::Logger::writeToLog("File does not exist: " + file.getFullPathName());
    return false;
  }

  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));

  if (reader == nullptr) {
    juce::Logger::writeToLog("Failed to create AudioFormatReader for: " +
                             file.getFullPathName());

    // Optional: check file extension
    juce::Logger::writeToLog("File extension: " + file.getFileExtension());
    juce::Logger::writeToLog(
        "Supported formats: MP3, WAV, AIFF, FLAC (registerBasicFormats())");

    return false;
  }

  readerSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));

  transport.setSource(readerSource.get(),
                      0, // read ahead buffer (0 = default)
                      nullptr,
                      readerSource->getAudioFormatReader()->sampleRate);

  juce::Logger::writeToLog("Successfully loaded: " + file.getFullPathName());
  return true;
}

void Mp3Streamer::prepareToPlay(int samplesPerBlock, double sampleRate) {
  transport.prepareToPlay(samplesPerBlock, sampleRate);
}

void Mp3Streamer::releaseResources() { transport.releaseResources(); }


bool mixer::Mp3Streamer::isPlaying() const
{
    return transport.isPlaying();
}


void Mp3Streamer::start() { transport.start(); }

void Mp3Streamer::stop() { transport.stop(); }

void Mp3Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  transport.getNextAudioBlock(bufferToFill);
}
} // namespace mixer
