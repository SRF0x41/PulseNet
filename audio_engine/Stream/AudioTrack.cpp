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

void AudioTrack::setFadeInDuration(double seconds) {
  fadeInDuration_samples = static_cast<int64_t>(seconds * currentSampleRate);
  transport.setGain(0.0f);

  if (fadeInDuration_samples > 0)
    fadeInGainRate =
        1.0f / static_cast<float>(fadeInDuration_samples); // float division
  else
    fadeInGainRate = 1.0f;
}

void AudioTrack::setFadeOutDuration(double seconds) {
  fadeOutDuration_samples = static_cast<int64_t>(seconds * currentSampleRate);

  if (fadeOutDuration_samples > 0)
    fadeOutGainRate =
        1.0f / static_cast<float>(fadeOutDuration_samples); // float division
  else
    fadeOutGainRate = 1.0f;
}

float AudioTrack::getFadeInGainRate() { return fadeInGainRate; }

float AudioTrack::getFadeOutGainRate() { return fadeOutGainRate; }

int64_t AudioTrack::getFadeInDuration_samples() {
  return fadeInDuration_samples;
}
int64_t AudioTrack::getFadeOutDuration_samples() {
  return fadeOutDuration_samples;
}

void AudioTrack::setGain(float gain) {
  transport.setGain(gain); // JUCE AudioTransportSource has a setGain() method
}

float AudioTrack::getGain(){
    return gain;
}