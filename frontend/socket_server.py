import socket


class SocketServer:
    def __init__(self, host="127.0.0.1", port=8080):
        self.host = host
        self.port = port
        self.server_sock = None
        self.client_sock = None
        self.client_addr = None

    def start(self):
        print(f"[SocketServer] Starting on {self.host}:{self.port}")

        self.server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_sock.bind((self.host, self.port))
        self.server_sock.listen(1)

        print("[SocketServer] Waiting for client...")
        self.client_sock, self.client_addr = self.server_sock.accept()
        print(f"[SocketServer] Client connected: {self.client_addr}")

    def receive(self, size=1024) -> bytes:
        if not self.client_sock:
            return b""

        data = self.client_sock.recv(size)
        print(f"[SocketServer] Received {len(data)} bytes")
        return data

    def send(self, data: bytes):
        if not self.client_sock:
            return

        print(f"[SocketServer] Sending {len(data)} bytes")
        self.client_sock.sendall(data)

    def shutdown(self):
        print("[SocketServer] Shutting down")

        if self.client_sock:
            self.client_sock.close()
            self.client_sock = None

        if self.server_sock:
            self.server_sock.close()
            self.server_sock = None
