#include "AudioTrack.h"
#include <cstdint>

// --------------------
// Construction
// --------------------
AudioTrack::AudioTrack(const std::string &sourcePath) : source(sourcePath) {
  formatManager.registerBasicFormats();

  juce::File file(source);
  if (!file.existsAsFile())
    return;

  auto *reader = formatManager.createReaderFor(file);
  if (!reader)
    return;

  readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

  transport.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
}

// --------------------
// Accessors
// --------------------
const std::string &AudioTrack::getSource() const { return source; }

float AudioTrack::getBpm() const { return bpm; }

void AudioTrack::setBpm(float newBpm) { bpm = newBpm; }

float AudioTrack::getRemainingTime() const { return remainingTime; }

void AudioTrack::setRemainingTime(float time) { remainingTime = time; }

juce::AudioTransportSource *AudioTrack::getTransport() { return &transport; }

void AudioTrack::startTransport() { transport.start(); }
void AudioTrack::stopTransport() { transport.stop(); }

void AudioTrack::setStartOverlapSeconds(double seconds) {
  startOverlapSeconds = juce::jmax(0.0, seconds);
}

double AudioTrack::getStartOverlapSeconds() const {
  return startOverlapSeconds;
}

int64_t AudioTrack::getTotalSamples(double sampleRate) {
  return static_cast<int64_t>(transport.getLengthInSeconds() * sampleRate);
}

int64_t AudioTrack::getOverlapSamples(double sampleRate) {
  return static_cast<int64_t>(startOverlapSeconds * sampleRate);
}

void AudioTrack::setFadeInDuration(double seconds) {}
void AudioTrack::setFadeOutDuration(double seconds) {}

// juce::AudioSource* AudioTrack::getAudioSource()
// {
//     return gainSource.get();
// }