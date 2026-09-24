"""Servidor local de archivos y puente hacia los procesos C++ de la visualización."""

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
    "/lcs.html": ("lcs.html", "text/html; charset=utf-8"),
    "/find.html": ("find.html", "text/html; charset=utf-8"),
    "/style.css": ("style.css", "text/css; charset=utf-8"),
    "/session.js": ("session.js", "text/javascript; charset=utf-8"),
    "/grid.js": ("grid.js", "text/javascript; charset=utf-8"),
    "/app.js": ("app.js", "text/javascript; charset=utf-8"),
    "/lcs.js": ("lcs.js", "text/javascript; charset=utf-8"),
    "/find.js": ("find.js", "text/javascript; charset=utf-8"),
}
# Cada modo es un proceso C++ con su propia sesion: bandera, campos y minimo.
MODES = {
    "manacher": ("--manacher-session", ("text",), 0),
    "lcs": ("--lcs-session", ("textA", "textB"), 1),
    "find": ("--find-session", ("text", "pattern"), 1),
}
ALPHABET = re.compile(r"[0-9A-F\r\n]*")
MAX_LENGTH = 10000


class Engine:
    def __init__(self, executable, flag):
        self.lock = threading.Lock()
        self.process = subprocess.Popen(
            [str(executable), flag],
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


class Engines:
    """Un proceso por modo, creado la primera vez que esa vista se usa."""

    def __init__(self, executable):
        self.executable = executable
        self.lock = threading.Lock()
        self.engines = {}

    def get(self, mode):
        with self.lock:
            if mode not in self.engines:
                self.engines[mode] = Engine(self.executable, MODES[mode][0])
            return self.engines[mode]

    def close(self):
        for engine in self.engines.values():
            engine.close()


def route(path):
    """/api/reset y /api/step siguen siendo los de Manacher."""
    parts = path.strip("/").split("/")
    if parts[:1] != ["api"] or parts[-1] not in ("reset", "step"):
        return None, None
    if len(parts) == 2:
        return "manacher", parts[1]
    if len(parts) == 3 and parts[1] in MODES:
        return parts[1], parts[2]
    return None, None


def reset_command(mode, body):
    fields, minimum = MODES[mode][1], MODES[mode][2]
    encoded = []
    for field in fields:
        value = body.get(field)
        if not isinstance(value, str) or not minimum <= len(value) <= MAX_LENGTH:
            raise ValueError("Usa de %d a %d caracteres en cada campo." % (minimum, MAX_LENGTH))
        if ALPHABET.fullmatch(value) is None:
            raise ValueError("Solo se admiten 0-9, A-F y saltos de linea.")
        encoded.append(value.encode("ascii").hex())
    return "RESET " + " ".join(encoded)


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
        mode, action = route(self.path)
        if mode is None:
            self.send_json(404, {"error": "Ruta no encontrada."})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
            if not 0 < length <= 70000:
                raise ValueError("Solicitud demasiado grande o vacia.")
            body = json.loads(self.rfile.read(length))
            if not isinstance(body, dict):
                raise ValueError("Solicitud invalida.")
            command = reset_command(mode, body) if action == "reset" else "NEXT"
            result = self.server.engines.get(mode).request(command)
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
        server.engines = Engines(executable)
        print("Visualizacion: http://127.0.0.1:" + str(server.server_port), flush=True)
        print("Deja esta ventana abierta. Ctrl+C para cerrar.", flush=True)
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
        if hasattr(server, "engines"):
            server.engines.close()


if __name__ == "__main__":
    main()
