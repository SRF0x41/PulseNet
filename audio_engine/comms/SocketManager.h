#pragma once

#include <cstdint>
#include <cstring> // for memset
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using SocketHandle = SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // for close()
using SocketHandle = int;
#endif

class SocketManager {
public:
  SocketManager();
  ~SocketManager();

  // Non-copyable
  SocketManager(const SocketManager &) = delete;
  SocketManager &operator=(const SocketManager &) = delete;

  // Movable
  SocketManager(SocketManager &&other) noexcept;
  SocketManager &operator=(SocketManager &&other) noexcept;

  bool connect(const std::string &host, uint16_t port);
  void disconnect();

  bool sendBytes(const void *data, size_t size);
  int receiveBytes(void *buffer, size_t size);

  bool isConnected() const;

private:
  SocketHandle socketHandle = invalidSocket();

  static SocketHandle invalidSocket();
  static void closeSocket(SocketHandle s);

#ifdef _WIN32
  static bool winsockInitialized;
  static void ensureWinsock();
#endif
};
