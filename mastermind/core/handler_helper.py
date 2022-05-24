#!/usr/bin/python3

from .agent import Agent
import cgi
from uuid import uuid4

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
        # Grab the "task" field that was input by the post request.
        uuid = uuid4()
        # uuid = "uuid"
        tgt_ip = self.client_address
        tgt_hostname = fields.get('hostname')
        tgt_os = fields.get('os type')
        tgt_os_version = fields.get('os version')
        # print(f"$$Agent UUID:          {uuid}")
        # print(f"$$Target IP Address:   {tgt_ip}")
        # print(f"$$Target Hostname:     {tgt_hostname}")
        # print(f"$$Target OS:           {tgt_os}")
        # print(f"$$Target OS Version:   {tgt_os_version}\n")
        new_agent = Agent(self.listener.name, uuid, tgt_ip, tgt_hostname, tgt_os, tgt_os_version)

        self.listener.agents[uuid] = new_agent

        # for key, value in self.listener.agents.items():
        #     print(f"$$UUID: {key}\n Listener name: {value.listener_name}\n")

        # print(f"$$Agent listener name: {new_agent.listener_name}")
        
        # Come back to this later to add to dict ******************************************
        # tasklist.append(new_task[0])

        # 301 is a redirect status response. This case, we want the user
        # to be redirected to the tasklist/new page after submitting a task.
        self.send_response(201, f"@{uuid}")
        # listener.send_header('content-type', 'text/html')
        # listener.send_header('Location', '/new')
        # Always have to close the header.
        self.end_headers()


def collect_results(self):
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
        task_results = fields.get('task results')
        print(f"$$Task ID:             {task_id}")
        print(f"$$Task Results:        {task_results}")
        # Come back to this later to add to dict ******************************************
        # tasklist.append(new_task[0])

        # 301 is a redirect status response. This case, we want the user
        # to be redirected to the tasklist/new page after submitting a task.
        self.send_response(201)
        # listener.send_header('content-type', 'text/html')
        # listener.send_header('Location', '/new')
        # Always have to close the header.
        self.end_headers()


def serve_tasks(self):
    self.send_response(200)
    self.send_header('Content-type', 'text/html')
    self.send_header('Content-Disposition', 'attachment; filename="tasks.txt"')
    self.end_headers()

    # Make this a relative path.
    with open('/home/jonathan/oopythonlabs/red_alert/data/tasks.txt', 'rb') as file: 
        self.wfile.write(file.read())

def get_agent_uuid(self):
    for key, value in self.listener.agents.items():
        # print("$$Agent UUID from dict is:\n", key)
        return key