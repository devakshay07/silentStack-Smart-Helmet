import json
import os
from http.server import BaseHTTPRequestHandler, HTTPServer
from datetime import datetime

# In-Memory State for Real-Time Dashboard (No Database)
latest_state = {
    "status": "OFFLINE",
    "worker_id": "UNASSIGNED",
    "last_seen": "Never",
    "temp": 0.0,
    "hum": 0.0,
    "gas_dev_pct": 0.0,
    "g_force": 1.0,
    "active_alerts": []
}

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            if self.path == '/':
                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                with open('dashboard.html', 'rb') as f:
                    self.wfile.write(f.read())
            elif self.path == '/status':
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps(latest_state).encode('utf-8'))
            else:
                self.send_response(404)
                self.end_headers()
        except Exception as e:
            print(f"GET Error: {e}")
            self.send_response(500)
            self.end_headers()

    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)
        
        try:
            data = json.loads(post_data.decode('utf-8'))
            ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            if "/telemetry" in self.path:
                latest_state.update(data)
                latest_state["last_seen"] = ts
                latest_state["status"] = "ONLINE"
                
            elif "/events" in self.path:
                hazards = json.dumps(data.get("active_events", []))
                
                print(f"\n🚨 [CRITICAL EVENT] {data.get('severity')} - {hazards}")
                latest_state["active_alerts"] = data.get("active_events", [])
                latest_state["last_seen"] = ts
                
                if data.get("severity") == "NORMAL":
                    latest_state["active_alerts"] = []
                
            elif "/heartbeat" in self.path:
                latest_state["status"] = data.get("status")
                latest_state["last_seen"] = ts

            print(f"[{ts}] {self.path.split('/')[-1].upper()} payload received.")

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status": "success"}')

        except json.JSONDecodeError:
            print("Error: Received malformed JSON data.")
            self.send_response(400)
            self.end_headers()
        except Exception as e:
            print(f"Server Error: {e}")
            self.send_response(500)
            self.end_headers()

    def log_message(self, format, *args):
        pass # Suppress default noisy HTTP logs

if __name__ == '__main__':
    # Ensure script always looks in its own directory for dashboard.html
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    server = HTTPServer(('0.0.0.0', 8000), RequestHandler)
    print("="*60)
    print("🔥 silentStack ADVANCED Command Center (In-Memory Mode) 🔥")
    print("1. Dashboard Web UI: http://localhost:8000")
    print("2. Memory: Ephemeral (No database, pure RAM-based speed)")
    print("="*60)
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server gracefully...")
        server.server_close()
