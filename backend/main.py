from http.server import BaseHTTPRequestHandler, HTTPServer
import json
from datetime import datetime

class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)
        
        try:
            data = json.loads(post_data.decode('utf-8'))
            timestamp = datetime.now().strftime("%H:%M:%S")
            print(f"\n[{timestamp}] +++ {self.path.upper()} +++")
            for key, value in data.items():
                print(f"  {key}: {value}")
            print("-" * 40)
        except Exception as e:
            print(f"\n[RAW DATA] {self.path.upper()}")
            print(post_data.decode('utf-8'))
            
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(b'{"status": "success"}')

    # Ignore annoying log messages for every request
    def log_message(self, format, *args):
        pass

if __name__ == '__main__':
    server = HTTPServer(('0.0.0.0', 8000), RequestHandler)
    print("="*50)
    print("Starting silentStack Backend (Offline Python Mode)...")
    print("Listening on http://0.0.0.0:8000")
    print("Waiting for ESP32 connections...")
    print("="*50)
    server.serve_forever()
