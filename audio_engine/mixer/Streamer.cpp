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

  // this->prepareToPlay(
  //     512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  transport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
  nextTransport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
  mixer.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);

  mixer.addInputSource(&transport, false);
  mixer.addInputSource(&nextTransport, false);
}

Streamer::Streamer(const AudioTrack &trackA) {
  formatManager.registerBasicFormats();
  trackA_file = juce::File(trackA.getSource());

  if (!loadFile(trackA_file))
    juce::Logger::writeToLog("Failed to load track A");

  deviceManager.initialise(0, 2, nullptr, true, {}, nullptr);

  // this->prepareToPlay(512,
  // deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  // this->prepareToPlay_transport(
  //     512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
  // this->prepareToPlay_mixer(
  //     512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  transport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
  mixer.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);

  mixer.addInputSource(&transport, false);
}


Streamer::Streamer(){
  formatManager.registerBasicFormats();
  deviceManager.initialise(0, 2, nullptr, true, {}, nullptr);
  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);
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

  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));
  if (reader == nullptr)
    return false;

  readerSource.reset(new juce::AudioFormatReaderSource(reader.release(), true));
  transport.setSource(readerSource.get(), 0, nullptr,
                      readerSource->getAudioFormatReader()->sampleRate);
  return true;
}

bool Streamer::loadNextFile(const juce::File &file) {
  if (!file.existsAsFile())
    return false;

  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));
  if (reader == nullptr)
    return false;

  nextReaderSource.reset(
      new juce::AudioFormatReaderSource(reader.release(), true));
  nextTransport.setSource(nextReaderSource.get(), 0, nullptr,
                          nextReaderSource->getAudioFormatReader()->sampleRate);
  return true;
}

int Streamer::addNext(const AudioTrack &track) {
  juce::File new_track = juce::File(track.getSource());
  track_list.emplace_back(new_track);
  std::cout << "Added " << track.getSource() << '\n';
  return 0;
}
void Streamer::prepareToPlay(int samplesPerBlock, double sampleRate) {
  transport.prepareToPlay(samplesPerBlock, sampleRate);
}

// void Streamer::prepareToPlay_transport(int samplesPerBlock, double sampleRate) {
//   transport.prepareToPlay(samplesPerBlock, sampleRate);
// }
// void Streamer::prepareToPlay_nextTransport(int samplesPerBlock,
//                                            double sampleRate) {
//   nextTransport.prepareToPlay(samplesPerBlock, sampleRate);
// }

// void Streamer::prepareToPlay_mixer(int samplesPerBlock, double sampleRate) {
//   mixer.prepareToPlay(samplesPerBlock, sampleRate);
// }

void Streamer::releaseResources() { transport.releaseResources(); }

bool Streamer::isPlaying() const { return transport.isPlaying(); }

void Streamer::start() {
  if(track_list.empty()){
    std::cout << "Track list is empty." << '\n';
    return;
  }

  if(transport.getLengthInSeconds() == 0 && nextTransport.getLengthInSeconds() == 0 && !track_list.empty()){
    juce::File first_track = track_list.front();
    track_list.erase(track_list.begin());
    loadFile(first_track);
    //transport.start();
  }

  mixer.addInputSource(&transport,false);
  transport.start();
  // nextTransport.start();
  startTimerHz(10);
}

void Streamer::stop() { transport.stop(); }
void Streamer::getNextAudioBlock(
    const juce::AudioSourceChannelInfo &bufferToFill) {
  bufferToFill.clearActiveBufferRegion();
  mixer.getNextAudioBlock(bufferToFill);
}

/*Streamer::Streamer(const AudioTrack &trackA) {
  formatManager.registerBasicFormats();
  trackA_file = juce::File(trackA.getSource());

  if (!loadFile(trackA_file))
    juce::Logger::writeToLog("Failed to load track A");

  deviceManager.initialise(0, 2, nullptr, true, {}, nullptr);

  // this->prepareToPlay(512,
  // deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  this->prepareToPlay_transport(
      512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
  this->prepareToPlay_mixer(
      512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

  audioSourcePlayer.setSource(this);
  deviceManager.addAudioCallback(&audioSourcePlayer);

  mixer.addInputSource(&transport, false);




bool Streamer::loadNextFile(const juce::File &file) {
  if (!file.existsAsFile())
    return false;

  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));
  if (reader == nullptr)
    return false;

  nextReaderSource.reset(
      new juce::AudioFormatReaderSource(reader.release(), true));
  nextTransport.setSource(nextReaderSource.get(), 0, nullptr,
                          nextReaderSource->getAudioFormatReader()->sampleRate);
  return true;
}
}*/

void setNextTrack();


void Streamer::timerCallback() {

  if(track_list.empty()) return;

  double remainingSeconds = transport.getLengthInSeconds() - transport.getCurrentPosition();
  // if (!crossfadeActive && remainingSeconds <= crossfadeDuration) {
  //   std::cout << "crossfade " << '\n';
  //   crossfadeActive = true;
  //   //nextTransport.start();
  // }

  if (!nextLoaded) {
    std::cout << "Loading Next Track" << '\n';

    // Pull the next file from list
    juce::File next_track = track_list.front();
    track_list.erase(track_list.begin());

    loadNextFile(next_track);

    nextTransport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
    mixer.addInputSource(&nextTransport, false);

    nextLoaded = true;
  }


  if (transport.getCurrentPosition() >= transport.getLengthInSeconds()) {
    std::cout << "end of song " << '\n';

    
    nextTransport.start();

    //transport.setSource()
    

    
    

    //transport.stop();
    //mixer.removeInputSource(&transport); // remove old track from mixer

    // Transfer nextTransport's source into transport
    // transport.setSource(nextReaderSource.get(), 0, nullptr, nextReaderSource->getAudioFormatReader()->sampleRate);
    // transport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
    // mixer.addInputSource(&transport, false);

    // Reset nextTransport for future tracks
    // nextReaderSource.reset();

    nextLoaded = false;
    //crossfadeActive = false;
}


  // juce::File = track_list vec.back();
  // track_list.pop_back();
}

} // namespace mixer


/*void Streamer::timerCallback() {
    if (!track_list.empty()) {
        double remainingSeconds = transport.getLengthInSeconds() - transport.getCurrentPosition();
        std::cout << "Remaining: " << remainingSeconds << " seconds\n";

        // CROSSFADE: Start nextTransport when within crossfadeDuration
        if (!isNextTrackScheduled && remainingSeconds <= crossfadeDuration) {
            // Get next track from the list
            juce::File nextTrackFile = track_list.back();
            track_list.pop_back(); // remove from list

            if (loadNextFile(nextTrackFile)) { // prepare nextTransport & nextReaderSource
                nextTransport.prepareToPlay(512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
                mixer.addInputSource(&nextTransport, false); // add to mixer
                nextTransport.start(); // start crossfade
                isNextTrackScheduled = true; // avoid re-triggering
            } else {
                juce::Logger::writeToLog("Failed to load next track: " + nextTrackFile.getFullPathName());
            }
        }

        // END OF TRACK: Switch transports when current track finishes
        if (transport.getCurrentPosition() >= transport.getLengthInSeconds()) {
            transport.stop();
            mixer.removeInputSource(&transport); // remove old track from mixer

            // Make nextTransport the new transport
            std::swap(transport, nextTransport);
            nextReaderSource.reset(); // optional, we've transferred ownership

            isNextTrackScheduled = false; // reset for next track
        }
    }
}
*/