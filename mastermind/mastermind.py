#!/usr/bin/python3

from core.listener import Listener, http_server
import os
from core.menu import *


def main():

    if not os.path.exists("../data/"):
        os.mkdir("../data/")

    if not os.path.exists("../data/listeners/"):
        os.mkdir("../data/listeners/")

    # if os.path.exists("./data/databases/") == False:
    # 	os.mkdir("./data/databases/")

    # Create Listener Object
    mylistener = Listener("Listener_4802", "", 9000)

    # Instantiate server object.
    http_server(mylistener)

    # home()


if __name__ == '__main__':
    main()
