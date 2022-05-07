#!/usr/bin/python3

# ref: https://www.youtube.com/watch?v=kogOfxg1c_g
# ref: https://www.youtube.com/watch?v=dgvLegLW6ek


# HTTPServer class allows you to define what port for your HTTP svr to
# listen on.
# The BaseHTTPRequestHandler class enables management of your various
# HTTP requests (e.g. GET, POST, etc)
from http.server import BaseHTTPRequestHandler


from .listener_helper import register_implant

tasklist = ['Task 1', 'Task 2', 'Task 3']


# class Listener:
#     def __init__(self, name, port, ipaddress):

#         self.name = name
#         self.port = port
#         self.ipaddress = ipaddress
#         self.webapp = HTTPServer(('', port), requestHandler)
#         self.is_running = False
        

class Listener(BaseHTTPRequestHandler):       
    def do_GET(self):
        if self.path.endswith('/tasks/UUID'):
            self.send_response(200)
            self.send_header('content-type', 'text/html')
            # Always have to close the header.
            self.end_headers()
            output = ''
            # Add opening html body tags
            output += '<html><body>'
            # Add header to task list.
            output += '<h1>Task List</h1>'
            for task in tasklist:
                output += task
                # Add html break so each task is printed on a new line.
                output += '</br>'
            output += '?'
            # Add closing HTML body tags
            output += '</body></html>'
            # Write to the browser window.
            self.wfile.write(output.encode())

        # if self.path.endswith('/remove'):
        #     # Obtain ID from URL to ID which task needs to be removed.
        #     # (see 27:00 in ref video)
        #     listIDpath = self.path.split('/')[2]
        #     self.send_response(200)
        #     self.send_header('content-type', 'text/html')
        #     self.end_headers()

        #     output = ''
        #     # Add opening html body tags
        #     output = '<html><body>'
        #     output += '<h1>Remove task: %s</h1>' % listIDpath.replace('%20', ' ')
        #     # Create new form
        #     output += '<form method="POST" enctype="multipart/form-data" action="tasklist/%s/remove">' % listIDpath
        #     output += '<input type="submit" value="Remove"></form>'
        #     # This line adds the ability to return to task list without making any changes if desired.
        #     output += '<a href="/tasklist">Cancel</a>'
        #     output += '</body></html>'
        #     self.wfile.write(output.encode())

    def do_POST(self):
        if self.path.endswith('/reg'):
            register_implant(self)

        # This if block creates the post ability to remove a task.
        # if self.path.endswith('/remove'):
        #     listIDPath = self.path.split('/')[2]
        #     ctype, pdict = cgi.parse_header(self.headers.get('content-type'))
        #     if ctype == 'multipart/form-data':
        #         list_item = listIDPath.replace('%20', ' ')
        #         # Remove the task from the list.
        #         for task in tasklist:
        #             print(task)
        #         print(f"list item is: {list_item}")
        #         tasklist.remove(list_item)

        #     self.send_response(301)
        #     self.send_header('content-type', 'text/html')
        #     self.send_header('Location', '/tasklist')
        