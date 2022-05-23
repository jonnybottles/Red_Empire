#!/usr/bin/python3

# ref: https://www.youtube.com/watch?v=kogOfxg1c_g
# ref: https://www.youtube.com/watch?v=dgvLegLW6ek


# HTTPServer class allows you to define what port for your HTTP svr to
# listen on.
# The BaseHTTPRequestHandler class enables management of your various
# HTTP requests (e.g. GET, POST, etc)
from http.server import HTTPServer, BaseHTTPRequestHandler, CGIHTTPRequestHandler as CGIHandler
import socketserver
from .handler_helper import register_agent, serve_tasks, collect_results, get_agent_uuid
import os
import threading
import subprocess
from multiprocessing import Process, Pool
from subprocess import PIPE, DEVNULL, STDOUT


class Listener:
    def __init__(self, name, ip_addr, port):
        self.name = name
        self.ip_addr = ip_addr
        self.port = port
        self.agents = {}
        self.path = f"../data/listeners/{self.name}/"
        self.agents_path = f"{self.path}/"
        self.is_running = False

        if not os.path.exists(self.path):
            os.mkdir(self.path)

        if not os.path.exists(self.agents_path):
            os.mkdir(self.agents_path)


class http_server:
    def __init__(self, listener):
        try:
            # process = Pool(os.cpu_count())
            Handler.listener = listener
            app = HTTPServer((listener.ip_addr, listener.port), Handler)
            # process.map(app.serve_forever())
            # process.close()
            # process.join()
            # self.server = Process(target=app.serve_forever())
            # print(f"$$Listener running on port 9000\n")
            DETACHED_PROCESS = 8
            self.server = subprocess.Popen(target=app.serve_forever(), creationflags=DETACHED_PROCESS, stdin=None, stdout=None, stderr=None)
            # self.daemon = threading.Thread(name=listener.name, target=self.server.start, args=())
            # self.daemon.daemon = True
            # self.daemon.start()
            listener.is_running = True
        except Exception as e:
            print("Unable to create / start listener", e)
            return

    # def start(self):


    #     self.daemon = threading.Thread(name = self.name,
    #                                     target = self.server.start,
    #                                     args = ())
    #     self.daemon.daemon = True
    #     self.daemon.start()

    #     self.isRunning = True

class Handler(CGIHandler):
    listener = None
    def do_GET(self):
        if self.path.endswith(f'/tasks/{get_agent_uuid(self)}'):
            serve_tasks(self)

    def do_POST(self):
        if self.path.endswith('/reg'):
            register_agent(self)
        if self.path.endswith(f'/results/{get_agent_uuid(self)}'):
            collect_results(self)

    # This silences log messages from the server.
    def log_message(self, format, *args):
        return