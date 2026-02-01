#include "AudioTrack.h"

// --------------------
// Construction
// --------------------
AudioTrack::AudioTrack(const std::string& sourcePath)
    : source(sourcePath)
{
    formatManager.registerBasicFormats();

    juce::File file(source);
    if (!file.existsAsFile())
        return;

    auto* reader = formatManager.createReaderFor(file);
    if (!reader)
        return;

    readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

    transport.setSource(
        readerSource.get(),
        0,
        nullptr,
        reader->sampleRate
    );
}


// --------------------
// Accessors
// --------------------
const std::string &AudioTrack::getSource() const { return source; }

float AudioTrack::getBpm() const { return bpm; }

void AudioTrack::setBpm(float newBpm) { bpm = newBpm; }

float AudioTrack::getRemainingTime() const { return remainingTime; }

void AudioTrack::setRemainingTime(float time) { remainingTime = time; }


juce::AudioTransportSource* AudioTrack::getTransport()
{
    return &transport;
}



void AudioTrack::startTransport(){
    transport.start();
}
void AudioTrack::stopTransport(){
    transport.stop();
}


/*The correct fix (minimal, clean, correct)
Class members (this is non-negotiable)
class AudioTrack {
public:
    explicit AudioTrack(const std::string& sourcePath);

    juce::AudioTransportSource* getTransport();

private:
    std::string source;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transport;
};

Constructor does the loading (ONCE)
AudioTrack::AudioTrack(const std::string& sourcePath)
    : source(sourcePath)
{
    formatManager.registerBasicFormats();

    juce::File file(source);
    if (!file.existsAsFile())
        return;

    auto* reader = formatManager.createReaderFor(file);
    if (!reader)
        return;

    readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

    transport.setSource(
        readerSource.get(),
        0,
        nullptr,
        reader->sampleRate
    );
}

getTransport() becomes boring (GOOD!)
juce::AudioTransportSource* AudioTrack::getTransport()
{
    return &transport;
}


That’s it. No file IO. No allocation. No device calls.

Why this design works perfectly with MixerAudioSource
mixer.addInputSource(track.getTransport(), false);


Transport lives as long as AudioTrack

Reader source lives as long as transport

Mixer pulls audio safely

Crossfades work

No crackles

No “second track choppy” mystery 👻

One last important architectural note

prepareToPlay() must be called by the audio graph, not by AudioTrack.

So your engine should look like:

mixer.prepareToPlay(samplesPerBlock, sampleRate);


JUCE will propagate that call down to every transport automatically.

If you want, next we can:

Make AudioTrack reloadable

Make it crossfade-safe

Remove AudioTransportSource entirely and do sample-accurate fades

Fix your Streamer so tracks chain cleanly without swapping

You’re very close — this was the big lifetime wall 🧱*/