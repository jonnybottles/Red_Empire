#!/usr/bin/python3 

# ref https://www.youtube.com/watch?v=3QiPPX-KeSc

import socket
import threading
import time

# Since we don't know the length of the message being sent to the server
# need to created a HEADER of 64 bytes. This will be the first message
# sent to the server, which will provide the size of the follow on message.
HEADER = 64
PORT = 5052
# Obtains local host ip address.
# SERVER = socket.gethostbyname(socket.gethostname())
SERVER = "192.168.92.128"
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'
# Message the server will be looking for from the client to close connection
# and terminate thread.
DISCONNECT_MESSAGE = "!DISCONNECT"

# Create socket and return file descriptor of socket.
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind port.
server.bind(ADDR)


# Defines how to handle each new connection.
def handle_connection(conn, addr):
    print(f"[NEW CONNECTION] {addr} connected")
    connected = True
    while connected:
        # Need to specify how many bytes to accept within the recv command.
        # HEADER is defined as fixed length of 64 bytes above. The first
        # message from the client will tell how long the follow on message is
        # that way the server knows how much to read thereafter.

        # Decode, decodes the message from byte format into a string, using
        # FORMAT (UTF-8)
        msg_length = conn.recv(HEADER).decode(FORMAT)
        # Check for returned FD first, or else trying to convert msg_length
        # prematurely will result in an errro.
        if msg_length:
            # Store incoming message length as int.
            msg_length = int(msg_length)
            msg = conn.recv(msg_length).decode(FORMAT)
            if msg == DISCONNECT_MESSAGE:
                connected = False
            print(f"[{addr}] {msg}")
            # Send message back to client
            conn.send("Msg received.".encode(FORMAT))
    conn.close()




# Start handles new client socket creation / connections.
def start():
    server.listen()
    print(f"[LISTENING] Server is listening on {SERVER}")
    while True:
        # This line blocks / waits for a new connection to the port.
        # Accept creates a socket object. It returns the new socket
        # fd and the ip address of the host connecting.
        conn, addr = server.accept()

        # When a new connection occurs, create a thread, pass it the 
        # handle_connection function with the newly created socket
        # object.
        thread = threading.Thread(target=handle_connection, args=(conn, addr))
        thread.start()

        # One is subtractred as there is always one thread running, this
        # provides an accurate count of all connections.
        print(f"[ACTIVE CONNECTIONS] {threading.activeCount() -1}")

print("[STARTING] server is starting...")
start()
