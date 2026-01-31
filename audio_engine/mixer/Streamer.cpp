#include "Streamer.h"
#include "AudioTrack.h"

namespace mixer {

Streamer::Streamer(const AudioTrack &trackA, const AudioTrack &trackB) {
    formatManager.registerBasicFormats();
    trackA_file = juce::File(trackA.getSource());
    trackB_file = juce::File(trackB.getSource());

    if (!loadFile(trackA_file))
        juce::Logger::writeToLog("Failed to load track A");

    if (!loadNextFile(trackB_file))
        juce::Logger::writeToLog("Failed to load track B");

    deviceManager.initialise(0, 2, nullptr, true, {}, nullptr);

    this->prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

    audioSourcePlayer.setSource(this);
    deviceManager.addAudioCallback(&audioSourcePlayer);


    mixer.addInputSource(&transport, false);
    mixer.addInputSource(&nextTransport, false);
}

Streamer::~Streamer() {
    stopTimer();
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    transport.setSource(nullptr);
}

bool Streamer::loadFile(const juce::File &file) {
    if (!file.existsAsFile())
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return false;

    readerSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));
    transport.setSource(readerSource.get(), 0, nullptr, readerSource->getAudioFormatReader()->sampleRate);
    return true;
}

bool Streamer::loadNextFile(const juce::File &file) {
    if (!file.existsAsFile())
        return false;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr)
        return false;

    nextReaderSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));
    nextTransport.setSource(nextReaderSource.get(), 0, nullptr, nextReaderSource->getAudioFormatReader()->sampleRate);
    return true;
}

int Streamer::addNext(const AudioTrack &track) {
    juce::File new_track = juce::File(track.getSource());
    track_list.emplace_back(new_track);
    return 0;
}
void Streamer::prepareToPlay (int samplesPerBlock, double sampleRate)
{
    transport.prepareToPlay(samplesPerBlock, sampleRate);
    nextTransport.prepareToPlay(samplesPerBlock, sampleRate);
    mixer.prepareToPlay(samplesPerBlock, sampleRate);
}



void Streamer::releaseResources() {
    transport.releaseResources();
}

bool Streamer::isPlaying() const {
    return transport.isPlaying();
}

void Streamer::start() {
    transport.start();
    //nextTransport.start();
    startTimerHz(10);
}

void Streamer::stop() {
    transport.stop();
}
void Streamer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    mixer.getNextAudioBlock(bufferToFill);
}




void Streamer::timerCallback() {
    double remainingSeconds = transport.getLengthInSeconds() - transport.getCurrentPosition();
    std::cout << remainingSeconds << '\n';
    if (remainingSeconds <= crossfadeDuration) {
        // nextTransport.setPosition(0.0);
        // nextTransport.start();
        nextTransport.start();
        crossfadeActive = true;
    }

    if(remainingSeconds <= 0){
       transport.stop();         // stop old track
      transport = nextTransport;
    //std::swap(transport, nextTransport);
    }


}

} // namespace mixer
