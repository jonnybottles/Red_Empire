#!/usr/bin/python3

from .listener import Listener, http_server

from collections import OrderedDict
from .common import *
import netifaces

listeners = OrderedDict()


def checkListenersEmpty(s):
    
    if len(listeners) == 0:
        
        if s == 1:
            error("There are no active listeners.")
            return True
        else:
            return True
    
    else:
        return False

def isValidListener(name, s):

    vListeners = ulisteners()

    if name in vListeners:
        return True
    else:
        if s == 1:
            error("Invalid listener.")
            return False
        else:
            return False

def view_listeners():

    if checkListenersEmpty(1) == False:
        
        success("Active listeners:")
        
        print(YELLOW)
        print(" Name                         IP:Port                                  Status")
        print("------                       ------------------                       --------")
        
        for i in listeners:
 
            if listeners[i].is_running == True:
                status = "Running"
            else:
                status = "Stopped"

            print(" {}".format(listeners[i].name) + " " * (29 - len(listeners[i].name)) + "{}:{}".format(listeners[i].ip_addr, str(listeners[i].port)) + " " * (41 - (len(str(listeners[i].port)) + len(":{}".format(listeners[i].ip_addr)))) + status)
        
        print(cRESET)

def ulisteners():
    
    l = []
    
    for listener in listeners:
        l.append(listeners[listener].name)
    
    return l

def start_listener(args):

    if len(args) == 1:
        name = args[0]
        if listeners[name].is_running == False:
            try:
                listeners[name].start()
                success(f"Started listener {name}")
            except:
                error("Invalid listener.")
        else:
            error(f"Listener {name} is already running.")
    else:
        if len(args) != 3:
            error("Invalid arguments.")
        else:
            name = args[0]

            try:
                port = int(args[1])
            except:
                error("Invalid port.")
                return 0
            
            iface = args[2]

            try:
                netifaces.ifaddresses(iface)
                ipaddress = netifaces.ifaddresses(iface)[netifaces.AF_INET][0]['addr']
            except:
                error("Invalid interface.")
                return 0

            if isValidListener(name, 0):
                error(f"Listener {name} already exists.")
            else:
            
                listeners[name] = Listener(name, ipaddress, port)
                progress(f"Starting listener {name} on {port}:{ipaddress}.")

                try:
                    http_server(listeners[name])
                    # listeners[name].start()
                    success("Listener started.")
                except:
                    error("Failed. Check your options.")
                    del listeners[name]

def stop_listener(args):

    if len(args) != 1:
        error("Invalid arguments.")
    else:
        
        name = args[0]
        
        if isValidListener(name, 1):
            
            if listeners[name].is_running == True:
                progress(f"Stopping listener {name}")
                listeners[name].stop()
                success("Stopped.")
            else:
                error(f"Listener {name} is already stopped.")
        else:
            pass