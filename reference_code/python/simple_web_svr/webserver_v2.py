#!/usr/bin/python3

# ref: https://www.youtube.com/watch?v=kogOfxg1c_g
# ref: https://www.youtube.com/watch?v=dgvLegLW6ek


# HTTPServer class allows you to define what port for your HTTP svr to
# listen on.
# The BaseHTTPRequestHandler class enables management of your various
# HTTP requests (e.g. GET, POST, etc)
from http.server import HTTPServer, BaseHTTPRequestHandler
import cgi

tasklist = ['Task 1', 'Task 2', 'Task 3']

class requestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # This if block handles get requests for /tasklist.
        # Writes basic text/html to the page.
        # That text being the task list which is looped through above.
        if self.path.endswith('/tasklist'):
            self.send_response(200)
            self.send_header('content-type', 'text/html')
            # Always have to close the header.
            self.end_headers()
            output = ''
            # Add opening html body tags
            output += '<html><body>'
            # Add header to task list.
            output += '<h1>Task List</h1>'
            output += '<h3><a href="/tasklist/new">Add New Task</a></h3>'
            for task in tasklist:
                output += task
                # Adds X to page to remove a given task if selected.
                # Once selected it appends /remove to that task in the url
                # Last if block in this function delas with removing the task.
                output += '<a/ href="/tasklist/%s/remove">X</a>' % task
                # Add html break so each task is printed on a new line.
                output += '</br>'
            # Add closing HTML body tags
            output += '</body></html>'
            # Write to the browser window.
            self.wfile.write(output.encode())
        # This if block handles get requests for /tasklist/new .
        # Writes basic text/html to the page.
        # That text being the task list which is looped through above.
        if self.path.endswith('/new'):
            self.send_response(200)
            self.send_header('content-type', 'text/html')
            # Always have to close the header.
            self.end_headers()

            output = ''
            # Add opening html body tags
            output += '<html><body>'
            output += '<h1>Add New Task</h1>'

            # Create new form
            output += '<form method="POST" enctype="multipart/form-data" action="/tasklist/new">'
            output += '<input name="task" type="text" placeholder="Add new task">'
            output += '<input type="submit" value="Add">'
            output += '</form>'
            output += '</body></html>'

            self.wfile.write(output.encode())
        
        if self.path.endswith('/remove'):
            # Obtain ID from URL to ID which task needs to be removed.
            # (see 27:00 in ref video)
            listIDpath = self.path.split('/')[2]
            self.send_response(200)
            self.send_header('content-type', 'text/html')
            self.end_headers()

            output = ''
            # Add opening html body tags
            output = '<html><body>'
            output += '<h1>Remove task: %s</h1>' % listIDpath.replace('%20', ' ')
            # Create new form
            output += '<form method="POST" enctype="multipart/form-data" action="tasklist/%s/remove">' % listIDpath
            output += '<input type="submit" value="Remove"></form>'
            # This line adds the ability to return to task list without making any changes if desired.
            output += '<a href="/tasklist">Cancel</a>'
            output += '</body></html>'
            self.wfile.write(output.encode())

    def do_POST(self):
        if self.path.endswith('/new'):
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
                new_task = fields.get('task')
                tasklist.append(new_task[0])

            # 301 is a redirect status response. This case, we want the user
            # to be redirected to the tasklist/new page after submitting a task.
            self.send_response(301)
            self.send_header('content-type', 'text/html')
            self.send_header('Location', '/tasklist')
            # Always have to close the header.
            self.end_headers()
        # This if block creates the post ability to remove a task.
        if self.path.endswith('/remove'):
            listIDPath = self.path.split('/')[2]
            ctype, pdict = cgi.parse_header(self.headers.get('content-type'))
            if ctype == 'multipart/form-data':
                list_item = listIDPath.replace('%20', ' ')
                # Remove the task from the list.
                for task in tasklist:
                    print(task)
                print(f"list item is: {list_item}")
                tasklist.remove(list_item)

            self.send_response(301)
            self.send_header('content-type', 'text/html')
            self.send_header('Location', '/tasklist')

        


def main():
    PORT = 9000
    # Create HTTPserver object. Requires two parameters.
    # Param 1:Tuple (hostname / port), Param 2 Request Handler Class.
    # Leave it blank here to use local host.
    server = HTTPServer(('', PORT), requestHandler)
    print(f"Server running on port {PORT}")
    # Runs until terminated.
    server.serve_forever()

if __name__ == '__main__':
    main()