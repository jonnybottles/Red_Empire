#!/usr/bin/python3

# ref: https://www.youtube.com/watch?v=kogOfxg1c_g


# HTTPServer class allows you to define what port for your HTTP svr to
# listen on.
# The BaseHTTPRequestHandler class enables management of your various
# HTTP requests (e.g. GET, POST, etc)
from http.server import HTTPServer, BaseHTTPRequestHandler

class helloHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('content-type', 'text/html')
        # Always have to close the header.
        self.end_headers()
        # This writes to the page.
        self.wfile.write('Hello Jon!'.encode())

def main():
    PORT = 8000
    # Create HTTPserver object. Requires two parameters.
    # Param 1:Tuple (hostname / port), Param 2 Request Handler Class.
    # Leave it blank here to use local host.
    server = HTTPServer(('', PORT), helloHandler)
    print(f"Server running on port {PORT}")
    # Runs until terminated.
    server.serve_forever()

if __name__ == '__main__':
    main()