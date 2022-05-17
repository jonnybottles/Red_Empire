#!/usr/bin/python3

from exception_utils import printerr, TracebackContext
from db_utils import drop_table


def main():
    drop_table("Listeners")
    drop_table("Agents")
    drop_table("Tasks")


if __name__ == "__main__":
    with TracebackContext(True):
        main()