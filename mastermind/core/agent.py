#!/usr/bin/python3
import os
import time

from shutil import rmtree


class Agent:
    def __init__(self, listener_name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.listener_name = listener_name
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version
        self.path = f"data/listeners/{self.listener_name}/agents/{self.name}/"
        self.tasksPath = "{}tasks".format(self.path, self.name)

        if os.path.exists(self.path) == False:
            os.mkdir(self.path)

    
