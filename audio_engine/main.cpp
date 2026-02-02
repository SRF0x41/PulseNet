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
  AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Call of "
                    "Duty Black Ops - Number lady  [HD].mp3");
  AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Disclosure - Latch ft. Sam Smith.mp3");
  AudioTrack trackC(
      "/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Roosevelt - Moving On "
      "(karlmixclub extended) version 1.mp3");
  trackB.setStartOverlapSeconds(8.0);
  trackC.setStartOverlapSeconds(0.0);

  // --------------------
  // Create streamer
  // --------------------
  // mixer::Streamer streamer(trackA);
   mixer::Streamer streamer;
  // streamer.addNext(trackA);
  streamer.addNext(trackB);
  streamer.addNext(trackC);
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
