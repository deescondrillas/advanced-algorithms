"""Servidor local de archivos y puente hacia el proceso C++ de Manacher."""

import argparse
import json
import re
import subprocess
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

ROOT = Path(__file__).resolve().parent
ASSETS = {
    "/": ("index.html", "text/html; charset=utf-8"),
    "/style.css": ("style.css", "text/css; charset=utf-8"),
    "/app.js": ("app.js", "text/javascript; charset=utf-8"),
}


class Engine:
    def __init__(self, executable):
        self.lock = threading.Lock()
        self.process = subprocess.Popen(
            [str(executable), "--manacher-session"],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
            text=True, encoding="ascii", bufsize=1,
        )

    def request(self, command):
        with self.lock:
            if self.process.poll() is not None:
                raise RuntimeError("El proceso C++ se cerro. Reinicia el servidor.")
            self.process.stdin.write(command + "\n")
            self.process.stdin.flush()
            response = self.process.stdout.readline()
            if not response:
                raise RuntimeError("El proceso C++ no respondio.")
            return json.loads(response)

    def close(self):
        if self.process.poll() is None:
            self.process.stdin.close()
            try:
                self.process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                self.process.kill()
                self.process.wait()


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def send_body(self, status, content, content_type):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(content)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("Content-Security-Policy",
                         "default-src 'self'; script-src 'self'; style-src 'self'; "
                         "connect-src 'self'; object-src 'none'; frame-ancestors 'none'")
        self.end_headers()
        try:
            self.wfile.write(content)
        except (BrokenPipeError, ConnectionResetError):
            pass

    def send_json(self, status, value):
        self.send_body(status, json.dumps(value).encode(), "application/json")

    def do_GET(self):
        if self.path not in ASSETS:
            self.send_json(404, {"error": "Ruta no encontrada."})
            return
        filename, content_type = ASSETS[self.path]
        self.send_body(200, (ROOT / filename).read_bytes(), content_type)

    def do_POST(self):
        # Solo la propia pagina local puede controlar la sesion.
        expected_origin = "http://127.0.0.1:" + str(self.server.server_port)
        if self.headers.get("Origin") != expected_origin:
            self.send_json(403, {"error": "Abre la interfaz desde " + expected_origin})
            return
        if self.path not in ("/api/reset", "/api/step"):
            self.send_json(404, {"error": "Ruta no encontrada."})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
            if not 0 < length <= 70000:
                raise ValueError("Solicitud demasiado grande o vacia.")
            body = json.loads(self.rfile.read(length))
            if not isinstance(body, dict):
                raise ValueError("Solicitud invalida.")
            command = "NEXT"
            if self.path == "/api/reset":
                text = body.get("text")
                if not isinstance(text, str) or len(text) > 10000:
                    raise ValueError("Usa hasta 10,000 caracteres.")
                if re.fullmatch(r"[0-9A-F\r\n]*", text) is None:
                    raise ValueError("Solo se admiten 0-9, A-F y saltos de linea.")
                command = "RESET " + text.encode("ascii").hex()
            result = self.server.engine.request(command)
            self.send_json(200, result)
        except (ValueError, UnicodeError) as error:
            self.send_json(400, {"error": str(error)})
        except (RuntimeError, OSError) as error:
            self.send_json(500, {"error": str(error)})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", type=Path, required=True)
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()
    executable = args.exe.resolve()
    if not executable.is_file():
        parser.error("No existe el ejecutable. Usa IniciarVisualizacion.cmd.")
    # Primero reserva el puerto: si esta ocupado, no deja un proceso C++ huerfano.
    server = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    try:
        server.engine = Engine(executable)
        print("Manacher: http://127.0.0.1:" + str(server.server_port), flush=True)
        print("Deja esta ventana abierta. Ctrl+C para cerrar.", flush=True)
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
        if hasattr(server, "engine"):
            server.engine.close()


if __name__ == "__main__":
    main()
