#include "comms/SocketManager.h"
#include "stream/AudioTrack.h"
#include "stream/Streamer.h"
#include <iostream>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

// for convenience
using json = nlohmann::json;

AudioTrackProperties addTrack(json msgProperties) {
  AudioTrackProperties newTrack;
  try {
    newTrack.path = msgProperties["track_path"];
    newTrack.advanceStart = msgProperties["advance_start"];
    newTrack.fadeinDuration = msgProperties["fade_in_duration"];
    newTrack.virtualEndTrim = msgProperties["virtual_end_trim"];
    newTrack.fadeOutDuration = msgProperties["fade_out_duration"];
    newTrack.overlapDuration = msgProperties["overlap_duration"];
  } catch (json::parse_error &e) {
    std::cerr << "JSON parse error: " << e.what() << "\n";
  }
  return newTrack;
}
void printAddTrackProperties(const AudioTrackProperties &props) {
  std::cout << "AddTrackProperties\n";
  std::cout << "------------------\n";
  std::cout << "Path                : " << props.path << "\n";
  std::cout << "Advance Start (s)   : " << props.advanceStart << "\n";
  std::cout << "Fade In Duration (s): " << props.fadeinDuration << "\n";
  std::cout << "Virtual End Trim (s): " << props.virtualEndTrim << "\n";
  std::cout << "Fade Out Duration(s): " << props.fadeOutDuration << "\n";
  std::cout << "Overlap Duration (s): " << props.overlapDuration << "\n";
  std::cout << std::endl;
}

/*TODO: make sure streamer removes tracks and fades in the timer callback! not
 * in the audio buffer callback
 add dynamic auditrack property changes*/

int main() {

  // --- Streamer ----
  mixer::Streamer streamer;

  SocketManager socket;

  if (!socket.connect("127.0.0.1", 8080)) {
    std::cerr << "Failed to connect\n";
    return 1;
  }

  std::cout << "Connected to server\n";

  char buffer[4096];

  while (true) {
    std::memset(buffer, 0, sizeof(buffer)); // clear buffer

    int bytes_received = socket.receiveBytes(buffer, sizeof(buffer) - 1);
    // if (bytes_received <= 0) {
    //   std::cout << "Server disconnected or no data received\n";
    //   break; // exit loop if server disconnects
    // }

    if (bytes_received == 0) {
      // No data received — just stay idle
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    buffer[bytes_received] = '\0'; // null terminate string
    std::cout << "[DATA BUFFER] " << buffer << "\n";

    std::string command;
    std::string track_path;

    try {
      // Parse JSON
      json msg = json::parse(buffer);

      if (msg.contains("command")) {
        std::cout << "[SERVER COMMAND]: " << msg["command"].get<std::string>()
                  << '\n';
        command = msg["command"].get<std::string>();
      }

      if (command == "ADD_NEXT") {
        json props = msg["properties"];
        std::cout << props << '\n';
        AudioTrackProperties newTrack = addTrack(props);
        printAddTrackProperties(newTrack);
        streamer.addNext(std::make_unique<AudioTrack>(newTrack));
      }

    } catch (json::parse_error &e) {
      std::cerr << "JSON parse error: " << e.what() << "\n";
      std::cerr << "Buffer content: " << buffer << "\n";
    }

    // Optionally, you can send a response back
    // const char* msg = "Message received\n";
    // socket.sendBytes(msg, std::strlen(msg));
  }

  // streamer.start();

  std::cout << "Exiting client\n";

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
  if (inputThread.joinable())
    inputThread.join();
  return 0;
}
