#include "SocketManager.h"

#include <cstring>

#ifdef _WIN32
bool SocketManager::winsockInitialized = false;
#endif

// ---------- Platform helpers ----------

SocketHandle SocketManager::invalidSocket() {
#ifdef _WIN32
  return INVALID_SOCKET;
#else
  return -1;
#endif
}

// Correct definition
void SocketManager::closeSocket(SocketHandle s) {
#ifdef _WIN32
  closesocket(s);
#else
  ::close(s);
#endif
}

#ifdef _WIN32
void SocketManager::ensureWinsock() {
  if (winsockInitialized)
    return;

  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  winsockInitialized = true;
}
#endif

// ---------- Lifetime ----------

SocketManager::SocketManager() {
#ifdef _WIN32
  ensureWinsock();
#endif
}

SocketManager::~SocketManager() { disconnect(); }

SocketManager::SocketManager(SocketManager &&other) noexcept {
  socketHandle = other.socketHandle;
  other.socketHandle = invalidSocket();
}

SocketManager &SocketManager::operator=(SocketManager &&other) noexcept {
  if (this != &other) {
    disconnect();
    socketHandle = other.socketHandle;
    other.socketHandle = invalidSocket();
  }
  return *this;
}

// ---------- Public API ----------

bool SocketManager::connect(const std::string &host, uint16_t port) {
  disconnect();

  socketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socketHandle == invalidSocket())
    return false;

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

#ifdef _WIN32
  if (InetPtonA(AF_INET, host.c_str(), &addr.sin_addr) != 1)
    return false;
#else
  if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1)
    return false;
#endif

  if (::connect(socketHandle, reinterpret_cast<sockaddr *>(&addr),
                sizeof(addr)) < 0) {
    disconnect();
    return false;
  }

  return true;
}

void SocketManager::disconnect() {
  if (socketHandle != invalidSocket()) {
    closeSocket(socketHandle);
    socketHandle = invalidSocket();
  }
}

bool SocketManager::sendBytes(const void *data, size_t size) {
  if (!isConnected())
    return false;

#ifdef _WIN32
  return ::send(socketHandle, static_cast<const char *>(data),
                static_cast<int>(size), 0) >= 0;
#else
  return ::send(socketHandle, data, size, 0) >= 0;
#endif
}

int SocketManager::receiveBytes(void *buffer, size_t size) {
  if (!isConnected())
    return -1;

#ifdef _WIN32
  return ::recv(socketHandle, static_cast<char *>(buffer),
                static_cast<int>(size), 0);
#else
  return ::recv(socketHandle, buffer, size, 0);
#endif
}

bool SocketManager::isConnected() const {
  return socketHandle != invalidSocket();
}
