"""Pruebas de la comunicación HTTP con el ejecutable real, sin dependencias."""
import json
import subprocess
import sys
import urllib.error
import urllib.request
from pathlib import Path

root = Path(__file__).resolve().parent
process = subprocess.Popen(
    [sys.executable, str(root / "server.py"), "--exe",
     str(root / ".build" / "algoritmos.exe"), "--port", "0"],
    stdout=subprocess.PIPE, text=True,
)
try:
    url = process.stdout.readline().strip().removeprefix("Visualizacion: ")
    assert url.startswith("http://127.0.0.1:")

    def post(route, data, origin=url):
        request = urllib.request.Request(
            url + route, data=json.dumps(data).encode(),
            headers={"Content-Type": "application/json", "Origin": origin})
        with urllib.request.urlopen(request, timeout=5) as response:
            return json.load(response)

    with urllib.request.urlopen(url, timeout=5) as response:
        assert "Manacher".encode() in response.read()
    for asset in ("/style.css", "/session.js", "/grid.js", "/app.js",
                  "/lcs.js", "/find.js", "/lcs.html", "/find.html"):
        with urllib.request.urlopen(url + asset, timeout=5) as response:
            assert response.status == 200

    for text, expected in [
        ("FABBAC", (2, 5)), ("FABCBA0", (2, 6)), ("ABBA0CDDC", (1, 4)),
        ("", (0, 0)), ("A", (1, 1)), ("ABBA", (1, 4)),
        ("A\nA", (1, 3)), ("A\r\nA", (1, 1)),
    ]:
        state = post("/api/manacher/reset", {"text": text})
        assert state["phase"] == "ready" and state["step"] == 0
        assert state["text"] == text and state["length"] == 0
        steps = 0
        phases = set()
        while not state["finished"]:
            state = post("/api/step", {})
            steps += 1
            phases.add(state["phase"])
            assert state["step"] == steps
            assert steps < 16 * (2 * len(text) + 1) + 1
        assert (state["start"], state["end"]) == expected
        assert state["length"] == (expected[1] - expected[0] + 1 if text else 0)
        assert "done" in phases
        assert post("/api/step", {}) == state

    # Reiniciar en medio de una comparación abandona solo la sesión anterior.
    state = post("/api/reset", {"text": "ABACABA"})
    while state["phase"] != "compare":
        state = post("/api/step", {})
    state = post("/api/reset", {"text": "FABBAC"})
    assert state["step"] == 0 and state["text"] == "FABBAC"
    while not state["finished"]:
        state = post("/api/step", {})
    assert (state["start"], state["end"]) == (2, 5)

    for text in ("<script>", "ABC DEF", "ABCé", "A" * 10001):
        try:
            post("/api/reset", {"text": text})
            raise AssertionError("Accepted invalid input")
        except urllib.error.HTTPError as error:
            assert error.code == 400
    try:
        post("/api/step", {}, origin="https://example.com")
        raise AssertionError("Accepted external origin")
    except urllib.error.HTTPError as error:
        assert error.code == 403

    state = post("/api/reset", {"text": "A" * 10000})
    assert state["text"] == "A" * 10000
    state = post("/api/step", {})
    assert "text" not in state
    assert len(state["radius"]) <= 31
    assert len(json.dumps(state)) < 1500
    # --- lcs: la ventana deslizante sobre el suffix array de los dos textos ---
    def run(route, body, limit=4000):
        state = post(route + "/reset", body)
        assert state["step"] == 0 and state["phase"] == "ready"
        steps = 0
        phases = {state["phase"]}
        while not state["finished"]:
            state = post(route + "/step", {})
            steps += 1
            phases.add(state["phase"])
            assert state["step"] == steps and steps < limit
            assert len(state["sa"]) <= 25 and len(state["lcp"]) <= 25
            assert len(state.get("origin", [])) <= 25
        assert post(route + "/step", {}) == state
        return state, phases

    state, phases = run("/api/lcs", {"textA": "ABACABADABACABA", "textB": "DABADABACABACABA"})
    assert (state["startA"], state["endA"]) == (5, 15)
    assert (state["startB"], state["endB"]) == (2, 12)
    assert state["length"] == 11 and phases >= {"select", "check", "update", "done"}

    # Sin substring comun el resultado es vacio y idxBest no retrocede antes del SA.
    state, phases = run("/api/lcs", {"textA": "AAAA", "textB": "BBBB"})
    assert state["length"] == 0 and state["startA"] == 0 and state["endB"] == 0

    # --- find: el patron recorre los sufijos comparando caracter por caracter ---
    for pattern, expected in [("ABACA", 1), ("ABADABACA", 5), ("ABABA", 0), ("A", 1)]:
        state, phases = run("/api/find", {"text": "ABACABADABACABA", "pattern": pattern})
        assert state["firstMatch"] == expected
        assert "compare" in phases and "done" in phases
    assert phases >= {"select", "compare", "match", "done"}

    # Reiniciar en medio de una comparacion abandona solo la sesion anterior.
    state = post("/api/find/reset", {"text": "ABACABA", "pattern": "CAB"})
    while state["phase"] != "compare":
        state = post("/api/find/step", {})
    state = post("/api/find/reset", {"text": "ABACABADABACABA", "pattern": "ABACA"})
    assert state["step"] == 0 and state["pattern"] == "ABACA"

    for route, body in [
        ("/api/lcs", {"textA": "", "textB": "AB"}), ("/api/lcs", {"textA": "AB"}),
        ("/api/lcs", {"textA": "AB", "textB": "GG"}),
        ("/api/find", {"text": "AB", "pattern": ""}),
        ("/api/find", {"text": "A" * 10001, "pattern": "A"}),
    ]:
        try:
            post(route + "/reset", body)
            raise AssertionError("Accepted invalid input")
        except urllib.error.HTTPError as error:
            assert error.code == 400
    try:
        post("/api/otro/reset", {"text": "AB"})
        raise AssertionError("Accepted unknown mode")
    except urllib.error.HTTPError as error:
        assert error.code == 404

    # Los mensajes siguen siendo una ventana, no una copia de los arreglos.
    state = post("/api/lcs/reset", {"textA": "A" * 10000, "textB": "B" * 10000})
    assert state["size"] == 20002 and len(state["sa"]) == 25
    state = post("/api/lcs/step", {})
    assert "textA" not in state and len(json.dumps(state)) < 1500

    print("PASS: HTTP, archivos de interfaz, pasos C++ de manacher, lcs y find,")
    print("      reinicios, ventanas, limites y validacion.")
finally:
    process.terminate()
    process.wait(timeout=5)
