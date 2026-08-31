import json
import sqlite3
import os
from http.server import BaseHTTPRequestHandler, HTTPServer
from datetime import datetime

# Setup Database
DB_FILE = 'silentStack.db'
conn = sqlite3.connect(DB_FILE, check_same_thread=False)
c = conn.cursor()
c.execute('''CREATE TABLE IF NOT EXISTS telemetry (timestamp TEXT, helmet_id TEXT, worker_id TEXT, temp REAL, hum REAL, gas REAL, g_force REAL)''')
c.execute('''CREATE TABLE IF NOT EXISTS events (timestamp TEXT, event_id TEXT, helmet_id TEXT, worker_id TEXT, severity TEXT, hazards TEXT, evidence TEXT)''')
conn.commit()

# In-Memory State for Real-Time Dashboard
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

    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)
        
        try:
            data = json.loads(post_data.decode('utf-8'))
            ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            if "/telemetry" in self.path:
                c.execute("INSERT INTO telemetry VALUES (?, ?, ?, ?, ?, ?, ?)",
                          (ts, data.get("helmet_id"), data.get("worker_id"), data.get("temp"), data.get("hum"), data.get("gas_dev_pct"), data.get("g_force")))
                conn.commit()
                latest_state.update(data)
                latest_state["last_seen"] = ts
                latest_state["status"] = "ONLINE"
                
                # Clear active alerts if back to normal telemetry (since RiskEngine only sends events on state change)
                # We assume if we are getting normal telemetry, the helmet is safe (offline queue aside).
                # Actually, let's keep it simple and just log.
                
            elif "/events" in self.path:
                hazards = json.dumps(data.get("active_events", []))
                evidence = json.dumps(data.get("evidence", {}))
                c.execute("INSERT INTO events VALUES (?, ?, ?, ?, ?, ?, ?)",
                          (ts, data.get("event_id"), data.get("helmet_id"), data.get("worker_id"), data.get("severity"), hazards, evidence))
                conn.commit()
                
                print(f"\n🚨 [CRITICAL EVENT] {data.get('severity')} - {hazards}")
                latest_state["active_alerts"] = data.get("active_events", [])
                latest_state["last_seen"] = ts
                
                if data.get("severity") == "NORMAL":
                    latest_state["active_alerts"] = []
                
            elif "/heartbeat" in self.path:
                latest_state["status"] = data.get("status")
                latest_state["last_seen"] = ts

            print(f"[{ts}] {self.path.split('/')[-1].upper()} payload stored successfully.")

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status": "success"}')

        except Exception as e:
            print("Error parsing data:", e)
            self.send_response(400)
            self.end_headers()

    def log_message(self, format, *args):
        pass # Suppress default noisy HTTP logs

if __name__ == '__main__':
    # Change dir to where the script is, so it finds dashboard.html
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    server = HTTPServer(('0.0.0.0', 8000), RequestHandler)
    print("="*60)
    print("🔥 silentStack ADVANCED Command Center 🔥")
    print("1. Dashboard Web UI: http://localhost:8000")
    print("2. Database: Saving all sensor data to silentStack.db")
    print("="*60)
    server.serve_forever()
