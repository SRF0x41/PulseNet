#include "mixer/AudioTrack.h"
#include "mixer/Streamer.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <thread>

int main() {
    // --------------------
    // Create tracks
    // --------------------
    AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Call of Duty Black Ops - Number lady  [HD].mp3");
    AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume - Holdin On.mp3");

    // --------------------
    // Create streamer
    // --------------------
    mixer::Streamer streamer(trackA, trackB);
    streamer.start();

    // --------------------
    // Console input thread
    // --------------------
    std::thread inputThread([&streamer]() {
        std::string userInput;
        std::cout << "Type something (type 'exit' to quit):" << std::endl;

        while (true) {
            std::cout << "> ";
            std::getline(std::cin, userInput);

            if (userInput == "exit") {
                std::cout << "Exiting..." << std::endl;
                streamer.stop(); // stop music playback
                break;
            }

            std::cout << "You typed: " << userInput << std::endl;
        }
    });

    // --------------------
    // Run JUCE message loop in main thread
    // --------------------
    juce::MessageManager::getInstance()->runDispatchLoop();

    // --------------------
    // Wait for console thread to finish
    // --------------------
    if (inputThread.joinable())
        inputThread.join();

    return 0;
}

// int main()
// {
//     juce::ConsoleApplication app;

//     // -------------------------
//     // Setup audio device
//     // -------------------------
//     juce::AudioDeviceManager deviceManager;
//     deviceManager.initialise(
//         0,  // no inputs
//         2,  // stereo output
//         nullptr,
//         true, // select default device
//         {},   // preferred setup
//         nullptr);

//     // -------------------------
//     // Load two MP3s
//     // -------------------------
//     mixer::Streamer song1, song2;

//     juce::File file1("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume
//     - Holdin On.mp3"); juce::File
//     file2("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume -
//     Sleepless feat. Jezzabell Doran.mp3");

//     if (!song1.loadFile(file1) || !song2.loadFile(file2))
//     {
//         juce::Logger::writeToLog("Failed to load one of the MP3s");
//         return 1;
//     }

//     // Prepare each song
//     double sampleRate =
//     deviceManager.getCurrentAudioDevice()->getCurrentSampleRate(); int
//     blockSize = 512;

//     song1.prepareToPlay(blockSize, sampleRate);
//     song2.prepareToPlay(blockSize, sampleRate);

//     // Start playback
//     song1.start();
//     song2.start();

//     // -------------------------
//     // Mixer
//     // -------------------------
//     juce::MixerAudioSource mixerSource;
//     mixerSource.addInputSource(&song1, false); // false = don't delete when
//     mixer is deleted mixerSource.addInputSource(&song2, false);

//     // -------------------------
//     // Audio output
//     // -------------------------
//     juce::AudioSourcePlayer audioSourcePlayer;
//     audioSourcePlayer.setSource(&mixerSource);
//     deviceManager.addAudioCallback(&audioSourcePlayer);

//     juce::Logger::writeToLog("Playing two songs simultaneously...");

//     // Wait until both songs finish
//     while (song1.isPlaying() || song2.isPlaying())
//     {
//         juce::Thread::sleep(100);
//     }

//     juce::Logger::writeToLog("Playback finished.");

//     // Cleanup
//     deviceManager.removeAudioCallback(&audioSourcePlayer);
//     audioSourcePlayer.setSource(nullptr);

//     song1.stop();
//     song2.stop();
//     song1.releaseResources();
//     song2.releaseResources();

//     return 0;
// }
