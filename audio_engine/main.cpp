#include "Stream/AudioTrack.h"
#include "Stream/Streamer.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <thread>

int main() {
  // --------------------
  // Create tracks
  // --------------------
  // AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/echohotel.mp3");
  // trackA.setFadeInDuration(7);
  // trackA.setFadeOutDuration(7);

  // AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/gong.mp3");
  // trackB.setStartOverlapSeconds(7);
  // trackB.setFadeInDuration(7);
  // trackB.setFadeOutDuration(7);

  // AudioTrack trackD("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/MDK.mp3");
  // trackD.setStartOverlapSeconds(7);
  // trackD.setFadeInDuration(7);
  // // AudioTrack trackE("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/SNOW STRIPPERS - UNDER YOUR SPELL.mp3");


  // // --------------------
  // // Create streamer
  // // --------------------
  // // mixer::Streamer streamer(trackA);
  //  mixer::Streamer streamer;
  // streamer.addNext(trackA);
  // streamer.addNext(trackB);
  // streamer.addNext(trackD);
  // // streamer.addNext(trackE);

  mixer::Streamer streamer;
  AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume - Holdin On.mp3");
  trackA.setFadeOutDuration(10);

  AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/This Summer (JBAG Remix).mp3");
  trackB.setStartOverlapSeconds(10);
  trackB.setFadeInDuration(10);

  streamer.addNext(trackA);
  streamer.addNext(trackB);
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
