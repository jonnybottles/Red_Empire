#!/usr/bin/python3

class Agent:
    def __init__(self, listener_name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_version):
        self.listener_name = listener_name
        self.name = uuid
        self.tgt_ip = tgt_ip
        self.hostname = tgt_hostname
        self.tgt_os = tgt_os
        self.tgt_version = tgt_version

    
