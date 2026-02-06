#include "stream/AudioTrack.h"
#include "stream/Streamer.h"
#include "comms/SocketManager.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <thread>


int main()
{
    SocketManager socket;

    if (!socket.connect("127.0.0.1", 8080))
    {
        std::cerr << "Failed to connect\n";
        return 1;
    }

    std::cout << "Connected to server\n";

    while (true)
    {
        //const char* msg = "hello from C++\n";
        //socket.sendBytes(msg, std::strlen(msg));

        char buffer[4096] = {};
        int bytes = socket.receiveBytes(buffer, sizeof(buffer) - 1);

        if (bytes <= 0)
        {
            std::cout << "Server disconnected\n";
            break;
        }

        buffer[bytes] = '\0';
        std::cout << "Server says: " << buffer;





        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;

  /*TODO: make sure streamer removes tracks and fades in the timer callback! not
   * in the audio buffer callback*/

  // mixer::Streamer streamer;
  // AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume - Holdin On.mp3");
  // trackA.advanceStart(20);
  // trackA.setFadeInDuration(7);
  // trackA.setFadeOutDuration(5);
  // trackA.virtualEndTrim(27);

  // AudioTrack trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/MDK.mp3");
  // trackB.setStartOverlapSeconds(29.571);                                                         
  // trackB.setFadeInDuration(5);

  // streamer.addNext(trackA);
  // streamer.addNext(trackB);
  // streamer.start();

  // // --------------------
  // // Console input thread
  // // --------------------
  // std::thread inputThread([&streamer]() {
  //   std::string userInput;
  //   std::cout << "Type something (type 'exit' to quit):" << std::endl;

  //   while (true) {
  //     std::cout << "> ";
  //     std::getline(std::cin, userInput);

  //     if (userInput == "exit") {
  //       std::cout << "Exiting..." << std::endl;
  //       streamer.stop(); // stop music playback
  //       break;
  //     }

  //     std::cout << "You typed: " << userInput << std::endl;
  //   }
  // });

  // // --------------------
  // // Run JUCE message loop in main thread
  // // --------------------
  // juce::MessageManager::getInstance()->runDispatchLoop();

  // // --------------------
  // // Wait for console thread to finish
  // // --------------------
  // if (inputThread.joinable())
  //   inputThread.join();

  // return 0;
}
