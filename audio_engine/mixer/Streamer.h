#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "AudioTrack.h"

namespace mixer {

class Streamer : public juce::AudioSource,
                 public juce::Timer {
public:
    Streamer();
    ~Streamer() override;

    void prepareToPlay(int samplesPerBlock, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo&) override;
    void timerCallback() override;

    bool addNext(AudioTrack& track);
    void start();
    void stop();

private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;

    juce::MixerAudioSource mixer;

    std::vector<AudioTrack*> trackList;
    int currentIndex = -1;


    double timerStartSeconds = 0.0;

};

}
