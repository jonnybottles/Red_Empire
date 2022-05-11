#!/usr/bin/python3

from core.listener import Listener, http_server



def main():
    # Create Listener Object
    mylistener = Listener("Listener_4802")

    # Instantiate server object.
    http_server(mylistener)


if __name__ == '__main__':
    main()
