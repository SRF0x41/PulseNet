#include "AudioTrack.h"
#include <cstdint>

// =====================
// Construction
// =====================
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
  // t->setPosition(0.0);
  transport.setPosition(0.0);

  remainingTime = transport.getLengthInSeconds();
}

/*#include <string>
struct AudioTrackProperties {
  std::string path;
  double advanceStart;
  double fadeinDuration = 0;
  double virtualEndTrim = 0;
  double fadeOutDuration = 0;
  double overlapDuration = 0;
};*/


AudioTrack::AudioTrack(AudioTrackProperties audioTrack) : source(audioTrack.path){

  formatManager.registerBasicFormats();

  juce::File file(source);
  if (!file.existsAsFile())
    return;

  auto *reader = formatManager.createReaderFor(file);
  if (!reader)
    return;

  readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

  transport.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
  // t->setPosition(0.0);
  transport.setPosition(0.0);

  remainingTime = transport.getLengthInSeconds();

  advanceStart(audioTrack.advanceStart);
  setFadeInDuration(audioTrack.fadeinDuration);
  setVirtualEndTrim(audioTrack.virtualEndTrim);
  setFadeOutDuration(audioTrack.fadeOutDuration);
  setStartOverlapSeconds(audioTrack.overlapDuration);

}

// =====================
// Accessors
// =====================
const std::string &AudioTrack::getSource() const { return source; }

float AudioTrack::getBpm() const { return bpm; }
void AudioTrack::setBpm(float newBpm) { bpm = newBpm; }

float AudioTrack::getRemainingTime() const { return remainingTime; }
void AudioTrack::setRemainingTime(float time) { remainingTime = time; }

juce::AudioTransportSource *AudioTrack::getTransport() { return &transport; }

// =====================
// Transport Control
// =====================
void AudioTrack::startTransport() { transport.start(); }
void AudioTrack::stopTransport() { transport.stop(); }

// =====================
// Overlap Control
// =====================
void AudioTrack::setStartOverlapSeconds(double seconds) {
  startOverlapSeconds = juce::jmax(0.0, seconds);
}

double AudioTrack::getStartOverlapSeconds() const {
  return startOverlapSeconds;
}

// =====================
// Timeline / Sample Info
// =====================
int64_t AudioTrack::getTotalSamples(double sampleRate) {
  return static_cast<int64_t>(remainingTime * sampleRate);
}

int64_t AudioTrack::getOverlapSamples(double sampleRate) {
  return static_cast<int64_t>(startOverlapSeconds * sampleRate);
}

// =====================
// Crossfade Control
// =====================
void AudioTrack::setFadeInDuration(double seconds) {
  fadeInDuration_samples = static_cast<int64_t>(seconds * currentSampleRate);
  transport.setGain(0.0f);

  if (fadeInDuration_samples > 0)
    fadeInGainRate = 1.0f / static_cast<float>(fadeInDuration_samples);
  else
    fadeInGainRate = 1.0f;
}

void AudioTrack::setFadeOutDuration(double seconds) {
  fadeOutDuration_samples = static_cast<int64_t>(seconds * currentSampleRate);

  if (fadeOutDuration_samples > 0)
    fadeOutGainRate = -1.0f / static_cast<float>(fadeOutDuration_samples);
  else
    fadeOutGainRate = -1.0f;
}

float AudioTrack::getFadeInGainRate() { return fadeInGainRate; }
float AudioTrack::getFadeOutGainRate() { return fadeOutGainRate; }

int64_t AudioTrack::getFadeInDuration_samples() {
  return fadeInDuration_samples;
}
int64_t AudioTrack::getFadeOutDuration_samples() {
  return fadeOutDuration_samples;
}

// =====================
// Gain Control
// =====================
void AudioTrack::setGain(float gain) {
  transport.setGain(gain); // JUCE AudioTransportSource method
}

float AudioTrack::getGain() { return gain; }

void AudioTrack::trimEnd(double seconds) {
  if (seconds > transport.getLengthInSeconds()) {
    return;
  }

  remainingTime -= seconds;
}

void AudioTrack::setVirtualEndTrim(double seconds) {
  if (seconds > transport.getLengthInSeconds()) {
    return;
  }
  remainingVirtualTime = remainingTime - seconds;
}

double AudioTrack::getVirtualRemainingTime() { return remainingVirtualTime; }

int64_t AudioTrack::getVirtualRemainingSamples(double sampleRate) {
  return static_cast<int64_t>(remainingVirtualTime * sampleRate);
}

void AudioTrack::advanceStart(double seconds) {
  if(seconds > remainingTime){
    return;
  }

  startTime += seconds;
  transport.setPosition(startTime);
  remainingTime -= seconds;

}
