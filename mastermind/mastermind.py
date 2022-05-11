#!/usr/bin/python3

from core.listener import Handler, Listener, http_server
from http.server import HTTPServer



def main():
    # Create Listener Object
    # Param 1: Listener Name:
    # Param 2: Port Number
    # Param 3: IP Address/ Port (blank uses local host.)

    mylistener = Listener("Listener_4802")

    listener_one = http_server(mylistener)
    print(f"Listener One running on port 9000\n")
    # listener_one.serve_forever()
    # listener_two = HTTPServer(('', 9001), Listener)
    # print(f"Listener Two running on port 9001\n")

    # listener_two = Listener("test listener", 9002, '')
    # print(f"Listener Two running on port 9002\n")

    # Runs until terminated.

    # listener_two.serve_forever()
    # listener_two.webapp.serve_forever()


if __name__ == '__main__':
    main()
