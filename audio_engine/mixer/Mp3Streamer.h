#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace mixer {
class Mp3Streamer : public juce::AudioSource {
public:
  Mp3Streamer();
  ~Mp3Streamer();

  bool loadFile(const juce::File &file);
  void prepareToPlay(int samplesPerBlock, double sampleRate);
  void releaseResources();
  bool isPlaying() const;  // <-- add this

  void start();
  void stop();

  void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);

private:
  juce::AudioFormatManager formatManager;

  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transport;
};
} // namespace mixer
