#!/usr/bin/python3

class Agent:
    def __init__(self, listener, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.listener = listener
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version

    
