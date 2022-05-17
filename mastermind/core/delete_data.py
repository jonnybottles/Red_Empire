#!/usr/bin/python3

from exception_utils import printerr, TracebackContext
from db_utils import delete_data


def main():
    delete_data("Listeners")
    # delete_data("Agents")
    # delete_data("Tasks")


if __name__ == "__main__":
    with TracebackContext(True):
        main()
