#!/usr/bin/python3 

# ref https://www.youtube.com/watch?v=3QiPPX-KeSc

import socket

# Since we don't know the length of the message being sent to the server
# need to created a HEADER of 64 bytes. This will be the first message
# sent to the server, which will provide the size of the follow on message.
HEADER = 64
PORT = 5052
FORMAT = 'utf-8'
# Message the server will be looking for from the client to close connection
# and terminate thread.
DISCONNECT_MESSAGE = "!DISCONNECT"
SERVER = "192.168.92.128"
ADDR = (SERVER, PORT)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(ADDR)


def send(msg):
    # Encode string as a bytes, FORMAT (UTF-8)
    message = msg.encode(FORMAT)
    msg_length = len(message)
    # Obtain length of message.
    send_length = str(msg_length).encode(FORMAT)
    # Pad the rest of the message with zeros to abide to the sever standard.
    # that we previously established.
    send_length += b' ' * (HEADER - len(send_length))
    client.send(send_length)
    client.send(message)
    # Lazy way of receiving message back from server, statically setting
    # header size to 2048.
    print(client.recv(2048).decode(FORMAT))


send("Hello, world!")
# Input will require the client to press enter for the message to send.
input()
send("Hello everyone")
input()
send(DISCONNECT_MESSAGE)
