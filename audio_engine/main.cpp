#include "Stream/AudioTrack.h"
#include "Stream/Streamer.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <thread>

int main() {

  /*TODO: make sure streamer removes tracks and fades in the timer callback! not
   * in the audio buffer callback*/

  mixer::Streamer streamer;
  AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/SNOW STRIPPERS - UNDER YOUR SPELL.mp3");
  trackA.setFadeInDuration(7);
  trackA.setFadeOutDuration(7);
  trackA.virtualEndTrim(20);

  AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/MDK.mp3");
  trackB.setStartOverlapSeconds(27.458);
  trackB.setFadeInDuration(7);

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
