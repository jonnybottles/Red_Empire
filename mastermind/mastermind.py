#!/usr/bin/python3

# from core.listener import Listener, http_server
import os
from core.menu import *


def main():

    if not os.path.exists("../data/"):
        os.mkdir("../data/")

    if not os.path.exists("../data/listeners/"):
        os.mkdir("../data/listeners/")

    home()


if __name__ == '__main__':
    main()
