#!/usr/bin/python3

from .implant import Implant
import cgi
from uuid import uuid4


def register_implant(listener):
    listener.send_response(200)
    listener.send_header('content-type', 'text/html')
    # Always have to close the header.
    listener.end_headers()

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

    listener.wfile.write(output.encode())
    # ctype = content type, pdict = parameter dictionary
    # ctype will scan the above code within /new if block, it will see
    # enctype="multipar/form-data" and add that to the value of ctype
    # the input data is then added to the pdict.
    ctype, pdict = cgi.parse_header(listener.headers.get('content-type'))
    pdict['boundary'] = bytes(pdict['boundary'], "utf-8")
    # Obtain content length of post that is being submitted.
    content_len = int(listener.headers.get('Content-length'))
    pdict['CONTENT_LENGTH'] = content_len
    if ctype == 'multipart/form-data':
        # This reads each part of the form
        fields = cgi.parse_multipart(listener.rfile, pdict)
        # Grab the "task" field that was input by the post request.
        uuid = "garfield"
        tgt_ip = listener.client_address
        tgt_hostname = fields.get('hostname')
        tgt_os = fields.get('os type')
        tgt_os_version = fields.get('os version')
        print(f"Implant UUID:        {uuid}")
        print(f"Target IP Address:   {tgt_ip}")
        print(f"Target Hostname:     {tgt_hostname}")
        print(f"Target OS:           {tgt_os}")
        print(f"Target OS Version:   {tgt_os_version}\n")
        Implant(uuid, tgt_ip, tgt_hostname, tgt_os, tgt_os_version)
        # Come back to this later to add to dict ******************************************
        # tasklist.append(new_task[0])

        # 301 is a redirect status response. This case, we want the user
        # to be redirected to the tasklist/new page after submitting a task.
        listener.send_response(201, f"@{uuid}")
        # listener.send_header('content-type', 'text/html')
        # listener.send_header('Location', '/new')
        # Always have to close the header.
        listener.end_headers()