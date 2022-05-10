#!/usr/bin/python3

class Agent:
    def __init__(self, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.name = uuid
        #
        # self.listener = listener
        self.tgt_ip = tgt_ip
        self.hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version

    
