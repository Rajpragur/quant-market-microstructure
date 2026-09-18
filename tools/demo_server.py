#!/usr/bin/env python3
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
REPORT = ROOT / "reports" / "research_summary.json"

HTML = """<!doctype html><html><head><meta charset='utf-8'><title>Quant Microstructure Lab</title><style>body{font:16px system-ui;max-width:900px;margin:40px auto;padding:0 20px;background:#101318;color:#e8edf2}h1{color:#7dd3fc}.card{border:1px solid #334155;border-radius:10px;padding:18px;margin:14px 0;background:#171c24}code{color:#a7f3d0}</style></head><body><h1>Quant Market Microstructure Lab</h1><div class='card'><b>Read-only research demo</b><p>Order-book replay, queue-aware execution, microstructure features, and Kalshi market-data integration. No live trading credentials.</p></div><div id='report' class='card'>Loading report…</div><script>fetch('/api/report').then(r=>r.json()).then(x=>{document.getElementById('report').innerHTML='<h2>Research status</h2><p>'+x.interpretation+'</p><ul>'+x.components.map(y=>'<li>'+y+'</li>').join('')+'</ul>'})</script></body></html>"""

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/api/health":
            self.send_json({"status": "ok", "project": "quant-market-microstructure"})
        elif self.path == "/api/report":
            self.send_json(json.loads(REPORT.read_text()))
        elif self.path == "/":
            body = HTML.encode()
            self.send_response(200); self.send_header("Content-Type", "text/html"); self.send_header("Content-Length", str(len(body))); self.end_headers(); self.wfile.write(body)
        else:
            self.send_error(404)

    def send_json(self, payload):
        body = json.dumps(payload).encode()
        self.send_response(200); self.send_header("Content-Type", "application/json"); self.send_header("Content-Length", str(len(body))); self.end_headers(); self.wfile.write(body)

    def log_message(self, *_):
        pass

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()
    if not REPORT.exists():
        raise SystemExit("run tools/generate_report.py first")
    print(f"http://127.0.0.1:{args.port}")
    ThreadingHTTPServer(("127.0.0.1", args.port), Handler).serve_forever()
