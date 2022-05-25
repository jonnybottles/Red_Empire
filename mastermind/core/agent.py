#!/usr/bin/python3
import os
import time
import red_alert.mastermind.core.menu as men

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

    
