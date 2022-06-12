import pickle
import os
from collections import OrderedDict

RED = "\u001b[31m"
GREEN = "\u001b[32m"
YELLOW = "\u001b[33m"
cRESET = "\u001b[0m"


def prompt(name):

    prompt = "\n" + GREEN + "(" + name + ")" + RED + "::> " + cRESET
    return prompt


def error(message):

    print("\n" + RED + "[!] " + message + cRESET)


def success(message):

    print("\n" + GREEN + "[*] " + message + "\n" + cRESET)


def progress(message):

    print("\n" + YELLOW + "[*] " + message + "\n" + cRESET)
