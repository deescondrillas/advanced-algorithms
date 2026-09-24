"""Servidor local de archivos y puente hacia los procesos C++ de la visualización."""

import argparse
import json
import subprocess
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

ROOT = Path(__file__).resolve().parent
# La URL no cambia aunque los archivos vivan en web/.
ASSETS = {
    "/": ("web/index.html", "text/html; charset=utf-8"),
    "/lcs.html": ("web/lcs.html", "text/html; charset=utf-8"),
    "/find.html": ("web/find.html", "text/html; charset=utf-8"),
    "/style.css": ("web/style.css", "text/css; charset=utf-8"),
    "/session.js": ("web/session.js", "text/javascript; charset=utf-8"),
    "/grid.js": ("web/grid.js", "text/javascript; charset=utf-8"),
    "/app.js": ("web/app.js", "text/javascript; charset=utf-8"),
    "/lcs.js": ("web/lcs.js", "text/javascript; charset=utf-8"),
    "/find.js": ("web/find.js", "text/javascript; charset=utf-8"),
}
# Cada modo es un proceso C++ con su propia sesion: bandera y archivos que elige.
MODES = {
    "manacher": ("--manacher-session", ("transmission",)),
    "lcs": ("--lcs-session", ()),
    "find": ("--find-session", ("transmission", "mcode")),
}


class Engine:
    def __init__(self, executable, flag, test):
        self.lock = threading.Lock()
        self.process = subprocess.Popen(
            [str(executable), flag, test],
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

    def __init__(self, executable, test):
        self.executable = executable
        self.test = test
        self.lock = threading.Lock()
        self.engines = {}

    def get(self, mode):
        with self.lock:
            if mode not in self.engines:
                self.engines[mode] = Engine(self.executable, MODES[mode][0], self.test)
            return self.engines[mode]

    def close(self):
        for engine in self.engines.values():
            engine.close()


def route(path):
    """/api/<modo>/reset y /api/<modo>/step; cualquier otra ruta es 404."""
    parts = path.strip("/").split("/")
    if len(parts) == 3 and parts[0] == "api" and parts[1] in MODES and parts[2] in ("reset", "step"):
        return parts[1], parts[2]
    return None, None


def reset_command(mode, body):
    """RESET con el indice de cada archivo del test; C++ los acota a los que hay."""
    indices = [str(int(body.get(field, 0))) for field in MODES[mode][1]]
    return " ".join(["RESET"] + indices)


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
            if not 0 < length <= 1000:
                raise ValueError("Solicitud demasiado grande o vacia.")
            body = json.loads(self.rfile.read(length))
            if not isinstance(body, dict):
                raise ValueError("Solicitud invalida.")
            command = reset_command(mode, body) if action == "reset" else "NEXT"
            result = self.server.engines.get(mode).request(command)
            self.send_json(200, result)
        except (ValueError, TypeError) as error:
            self.send_json(400, {"error": str(error)})
        except (RuntimeError, OSError) as error:
            self.send_json(500, {"error": str(error)})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", type=Path, required=True)
    parser.add_argument("--test", default="tests/test1/")
    parser.add_argument("--port", type=int, default=8080)
    args = parser.parse_args()
    executable = args.exe.resolve()
    if not executable.is_file():
        parser.error("No existe el ejecutable. Usa make visual o IniciarVisualizacion.cmd.")
    # Las sesiones animan estos archivos, los mismos que ejecuta ./x con ese test.
    missing = [name for name in ("transmission1.txt", "transmission2.txt",
                                 "mcode1.txt", "mcode2.txt", "mcode3.txt")
               if not (Path(args.test) / name).is_file()]
    if missing:
        parser.error("Faltan archivos en %s: %s" % (args.test, ", ".join(missing)))
    # Primero reserva el puerto: si esta ocupado, no deja un proceso C++ huerfano.
    server = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    try:
        server.engines = Engines(executable, args.test)
        print("Visualizacion: http://127.0.0.1:" + str(server.server_port), flush=True)
        print("Datos: " + args.test, flush=True)
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
