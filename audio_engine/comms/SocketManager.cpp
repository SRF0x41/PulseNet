#include "SocketManager.h"

#include <cstring>
#include <iostream>

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
  std::cout << "[SocketManager] Closing socket (Windows)\n";
  closesocket(s);
#else
  std::cout << "[SocketManager] Closing socket (POSIX)\n";
  ::close(s);
#endif
}

#ifdef _WIN32
void SocketManager::ensureWinsock() {
  if (winsockInitialized)
    return;

  std::cout << "[SocketManager] Initializing WinSock\n";
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  winsockInitialized = true;
}
#endif

// ---------- Lifetime ----------

SocketManager::SocketManager() {
  std::cout << "[SocketManager] Constructor\n";
#ifdef _WIN32
  ensureWinsock();
#endif
}

SocketManager::~SocketManager() {
  std::cout << "[SocketManager] Destructor\n";
  disconnect();
}

SocketManager::SocketManager(SocketManager &&other) noexcept {
  std::cout << "[SocketManager] Move constructor\n";
  socketHandle = other.socketHandle;
  other.socketHandle = invalidSocket();
}

SocketManager &SocketManager::operator=(SocketManager &&other) noexcept {
  std::cout << "[SocketManager] Move assignment\n";
  if (this != &other) {
    disconnect();
    socketHandle = other.socketHandle;
    other.socketHandle = invalidSocket();
  }
  return *this;
}

// ---------- Public API ----------

bool SocketManager::connect(const std::string &host, uint16_t port) {
  std::cout << "[SocketManager] connect(" << host << ":" << port << ")\n";

  disconnect();

  socketHandle = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socketHandle == invalidSocket()) {
    std::cerr << "[SocketManager] socket() failed\n";
    return false;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

#ifdef _WIN32
  if (InetPtonA(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
    std::cerr << "[SocketManager] InetPtonA failed\n";
    return false;
  }
#else
  if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
    std::cerr << "[SocketManager] inet_pton failed\n";
    return false;
  }
#endif

  if (::connect(socketHandle,
                reinterpret_cast<sockaddr *>(&addr),
                sizeof(addr)) < 0) {
    std::cerr << "[SocketManager] connect() failed\n";
    disconnect();
    return false;
  }

  std::cout << "[SocketManager] Connected successfully\n";
  return true;
}

void SocketManager::disconnect() {
  if (socketHandle != invalidSocket()) {
    std::cout << "[SocketManager] disconnect()\n";
    closeSocket(socketHandle);
    socketHandle = invalidSocket();
  }
}

bool SocketManager::sendBytes(const void *data, size_t size) {
  if (!isConnected()) {
    std::cerr << "[SocketManager] sendBytes() called while disconnected\n";
    return false;
  }

  std::cout << "[SocketManager] sendBytes(" << size << " bytes)\n";

#ifdef _WIN32
  return ::send(socketHandle,
                static_cast<const char *>(data),
                static_cast<int>(size),
                0) >= 0;
#else
  return ::send(socketHandle, data, size, 0) >= 0;
#endif
}

int SocketManager::receiveBytes(void *buffer, size_t size) {
  if (!isConnected()) {
    std::cerr << "[SocketManager] receiveBytes() called while disconnected\n";
    return -1;
  }

  std::cout << "[SocketManager] receiveBytes(" << size << " bytes)\n";

#ifdef _WIN32
  return ::recv(socketHandle,
                static_cast<char *>(buffer),
                static_cast<int>(size),
                0);
#else
  return ::recv(socketHandle, buffer, size, 0);
#endif
}

bool SocketManager::isConnected() const {
  bool connected = socketHandle != invalidSocket();
  std::cout << "[SocketManager] isConnected() -> "
            << (connected ? "true" : "false") << "\n";
  return connected;
}
