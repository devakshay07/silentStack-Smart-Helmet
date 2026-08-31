import json
import os
import logging
from http.server import BaseHTTPRequestHandler, HTTPServer
from datetime import datetime

# Setup robust flat-file logging
LOG_FILE = 'helmet.logs'
logging.basicConfig(
    filename=LOG_FILE,
    level=logging.INFO,
    format='[%(asctime)s] %(levelname)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# In-Memory State for Real-Time Dashboard (No Database)
latest_state = {
    "status": "OFFLINE",
    "worker_id": "UNAUTHORIZED",
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
            err_msg = f"GET Error handling path {self.path}: {str(e)}"
            print(f"Error: {err_msg}")
            logging.error(err_msg)
            self.send_response(500)
            self.end_headers()

    def do_POST(self):
        try:
            content_length = int(self.headers.get('Content-Length', 0))
            if content_length == 0:
                self.send_response(400)
                self.end_headers()
                return

            post_data = self.rfile.read(content_length)
            
            try:
                data = json.loads(post_data.decode('utf-8'))
            except json.JSONDecodeError:
                print("Error: Received malformed JSON data.")
                logging.error(f"Malformed JSON payload received: {post_data}")
                self.send_response(400)
                self.end_headers()
                return

            ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            
            if "/telemetry" in self.path:
                latest_state.update(data)
                latest_state["last_seen"] = ts
                latest_state["status"] = "ONLINE"
                
                log_msg = f"[TELEMETRY] Worker: {data.get('worker_id', 'N/A')} | Temp: {data.get('temp', 0)}C | Hum: {data.get('hum', 0)}% | Gas: {data.get('gas_dev_pct', 0)}% | GForce: {data.get('g_force', 1)}G"
                logging.info(log_msg)
                
            elif "/events" in self.path:
                hazards = json.dumps(data.get("active_events", []))
                severity = data.get('severity', 'UNKNOWN')
                
                print(f"\n🚨 [CRITICAL EVENT] {severity} - {hazards}")
                log_msg = f"[EVENT] Severity: {severity} | Hazards: {hazards} | Evidence: {json.dumps(data.get('evidence', {}))}"
                logging.warning(log_msg)
                
                latest_state["active_alerts"] = data.get("active_events", [])
                latest_state["last_seen"] = ts
                
                if severity == "NORMAL":
                    latest_state["active_alerts"] = []
                
            elif "/heartbeat" in self.path:
                latest_state["status"] = data.get("status", "UNKNOWN")
                latest_state["last_seen"] = ts
                logging.info(f"[HEARTBEAT] Status: {latest_state['status']} | Uptime: {data.get('uptime_s', 0)}s")

            print(f"[{ts}] {self.path.split('/')[-1].upper()} payload parsed & logged.")

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status": "success"}')

        except Exception as e:
            err_msg = f"Server Crash Prevented during POST: {str(e)}"
            print(f"Error: {err_msg}")
            logging.error(err_msg)
            
            # Attempt to gracefully close the connection if an exception occurred midway
            try:
                self.send_response(500)
                self.end_headers()
            except:
                pass

    def log_message(self, format, *args):
        pass # Suppress default noisy HTTP logs

if __name__ == '__main__':
    # Ensure script always looks in its own directory for dashboard.html and helmet.logs
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    server = HTTPServer(('0.0.0.0', 8000), RequestHandler)
    print("="*60)
    print("🔥 silentStack ADVANCED Command Center (Robust Mode) 🔥")
    print("1. Dashboard Web UI: http://localhost:8000")
    print(f"2. Logging everything safely to: {os.path.abspath(LOG_FILE)}")
    print("="*60)
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server gracefully...")
        logging.info("Server manually shut down.")
        server.server_close()
