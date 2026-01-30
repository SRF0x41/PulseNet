#include "mixer/Mp3Streamer.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

int main()
{
    juce::ConsoleApplication app;

    // Setup audio device manager
    juce::AudioDeviceManager deviceManager;
    deviceManager.initialise(
        0, // no inputs
        2, // stereo output
        nullptr,
        true, // select default device
        {},   // preferred setup
        nullptr);

    mixer::Mp3Streamer streamer;

    juce::File mp3File("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume - Holdin On.mp3");
    if (!streamer.loadFile(mp3File))
    {
        juce::Logger::writeToLog("Failed to load MP3");
        return 1;
    }

    // Prepare the streamer for playback
    streamer.prepareToPlay(
        512, deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());
    streamer.start();

    // Hook streamer into device callback
    juce::AudioSourcePlayer audioSourcePlayer;
    audioSourcePlayer.setSource(&streamer);
    deviceManager.addAudioCallback(&audioSourcePlayer);

    juce::Logger::writeToLog("Playing MP3...");

    // --- Wait until playback finishes ---
    while (streamer.isPlaying()) // <-- automatically stops when song ends
    {
        juce::Thread::sleep(100); // poll every 100 ms
    }

    juce::Logger::writeToLog("Playback finished.");

    // Cleanup
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(nullptr);
    streamer.stop();
    streamer.releaseResources();

    return 0;
}
