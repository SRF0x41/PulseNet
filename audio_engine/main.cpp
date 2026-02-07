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
    if (bytes_received <= 0) {
      std::cout << "Server disconnected or no data received\n";
      break; // exit loop if server disconnects
    }

    buffer[bytes_received] = '\0'; // null terminate string
    std::cout << "[SERVER] " << buffer << "\n";

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
        AudioTrackProperties newTrack = addTrack(props);
        printAddTrackProperties(newTrack);

        AudioTrack addNextTrack = AudioTrack(newTrack);
        streamer.addNext(addNextTrack);

      }

    } catch (json::parse_error &e) {
      std::cerr << "JSON parse error: " << e.what() << "\n";
      std::cerr << "Buffer content: " << buffer << "\n";
    }

    // Optionally, you can send a response back
    // const char* msg = "Message received\n";
    // socket.sendBytes(msg, std::strlen(msg));

    streamer.start();
  }

  std::cout << "Exiting client\n";
  return 0;

  //   while (true) {
  //     // const char* msg = "hello from C++\n";
  //     // socket.sendBytes(msg, std::strlen(msg));

  //     char buffer[4096] = {};
  //     int bytes = socket.receiveBytes(buffer, sizeof(buffer) - 1);

  //     if (bytes <= 0) {
  //       std::cout << "Server disconnected\n";
  //       break;
  //     }
  //     buffer[bytes] = '\0';
  //     std::cout << "Server says: " << buffer << '\n';

  //     std::string command;
  //     std::string track_path;

  //     try {
  //       // Parse JSON
  //       json msg = json::parse(buffer);

  //       if (msg.contains("command")) {
  //         std::cout << "[SERVER COMMAND]: " <<
  //         msg["command"].get<std::string>()
  //                   << '\n';
  //         command = msg["command"].get<std::string>();
  //       }

  //       if (msg.contains("track_path")) {
  //         std::cout << "[COMMAND TRACK PATH]: "
  //                   << msg["track_path"].get<std::string>() << '\n';
  //         track_path = msg["track_path"].get<std::string>();
  //       }

  //       if (msg.contains("properties")) {
  //     json props = msg["properties"];

  //     if (props.contains("advance_start"))
  //         std::cout << "[advance_start]: " <<
  //         props["advance_start"].get<double>() << "\n";

  //     if (props.contains("set_fade_in_duration"))
  //         std::cout << "[set_fade_in_duration]: " <<
  //         props["set_fade_in_duration"].get<double>() << "\n";

  //     if (props.contains("set_virtual_end_trim"))
  //         std::cout << "[set_virtual_end_trim]: " <<
  //         props["set_virtual_end_trim"].get<double>() << "\n";

  //     if (props.contains("set_fade_out_duration"))
  //         std::cout << "[set_fade_out_duration]: " <<
  //         props["set_fade_out_duration"].get<double>() << "\n";

  //     if (props.contains("set_overlap_duration"))
  //         std::cout << "[set_overlap_duration]: " <<
  //         props["set_overlap_duration"].get<double>() << "\n";
  // }

  //     } catch (json::parse_error &e) {
  //       std::cerr << "JSON parse error: " << e.what() << "\n";
  //       std::cerr << "Buffer content: " << buffer << "\n";
  //     }

  //     // send ack message
  //     json ackMessage;
  //     ackMessage["type"] = "message";
  //     ackMessage["data"] = "ACK";

  //     std::string ackJson_str = ackMessage.dump();
  //     std::cout << "Serialized JSON: " << ackJson_str << '\n';
  //     socket.sendBytes(ackJson_str.c_str(), ackJson_str.size());

  //     // std::this_thread::sleep_for(std::chrono::seconds(1));
  //   }

  //   return 0;

  /*TODO: make sure streamer removes tracks and fades in the timer callback! not
   * in the audio buffer callback*/

  // mixer::Streamer streamer;
  // AudioTrack trackA("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/Flume
  // - Holdin On.mp3"); trackA.advanceStart(20); trackA.setFadeInDuration(7);
  // trackA.setFadeOutDuration(5);
  // trackA.virtualEndTrim(27);

  // AudioTrack
  // trackB("/home/user1/Desktop/Dev/PulseNet/get_audio/tracks/MDK.mp3");
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
