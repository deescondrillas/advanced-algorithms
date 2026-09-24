"use strict";

const $ = (id) => document.getElementById(id);

// Límites de la actividad; el servidor vuelve a comprobarlos.
const ALPHABET = /^[0-9A-F\r\n]*$/;
const MAX_LENGTH = 10000;

function invalidText(text) {
  return text.length > MAX_LENGTH || !ALPHABET.test(text);
}

function visibleChar(character) {
  if (character === "\n") return "↵";
  if (character === "\r") return "␍";
  return character;
}

// Solo esta capa conoce HTTP. Puede sustituirse por un transporte WebSocket.
async function request(path, body) {
  const response = await fetch(path, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
    signal: AbortSignal.timeout(10000)
  });
  const data = await response.json();
  if (!response.ok || data.error) throw new Error(data.error || "Error de conexión.");
  return data;
}

// Control de ejecución común a las tres vistas. Cada vista aporta:
//   mode: prefijo de las rutas /api/<mode>/reset y /api/<mode>/step
//   fields(original): cuerpo del RESET, o null si la entrada local es inválida
//   updateState(data): normaliza el mensaje de C++ y llama a session.publish
//   render(state): dibuja el estado ya normalizado
//   inputs(): campos que se deshabilitan mientras hay una petición en curso
function createSession(view) {
  const session = {
    state: null, busy: false, reading: false, playing: false, dirty: false, timer: null,
    transport: {
      reset: (body) => request("/api/" + view.mode + "/reset", body),
      next: () => request("/api/" + view.mode + "/step", {})
    }
  };
  window.algorithmTransport = session.transport;

  session.updateControls = () => {
    const ready = session.state && !session.dirty;
    $("playButton").textContent = session.playing ? "Pausar" : "Reproducir";
    $("playButton").disabled = !ready || session.reading || session.state.finished
      || (session.busy && !session.playing);
    $("nextButton").disabled = !ready || session.busy || session.reading || session.playing
      || session.state.finished;
    $("resetButton").disabled = session.busy || session.reading || !session.state;
    for (const element of view.inputs()) element.disabled = session.busy || session.reading;
  };

  session.pause = () => {
    session.playing = false;
    clearTimeout(session.timer);
    session.updateControls();
  };

  session.showError = (error) => {
    session.pause();
    $("error").textContent = error.message || String(error);
    $("error").hidden = false;
  };

  // Recibe el estado ya normalizado por la vista y lo dibuja.
  session.publish = (state) => {
    session.state = state;
    view.render(state);
    if (state.finished) session.pause();
    session.updateControls();
  };

  // original: reutiliza las cadenas confirmadas por C++ en lugar de los campos.
  session.load = async (original) => {
    if (session.busy || session.reading) return;
    session.pause();
    const body = view.fields(original === true);
    if (!body) {
      session.dirty = true;
      session.updateControls();
      return;
    }
    session.busy = true;
    $("error").hidden = true;
    session.updateControls();
    try {
      view.updateState(await session.transport.reset(body));
      session.dirty = false;
    } catch (error) {
      session.dirty = true;
      session.showError(error);
    } finally {
      session.busy = false;
      session.updateControls();
    }
  };

  session.advance = async () => {
    if (session.busy || session.reading || session.dirty || !session.state
      || session.state.finished) return;
    session.busy = true;
    session.updateControls();
    try {
      view.updateState(await session.transport.next());
    } catch (error) {
      session.dirty = true;
      session.showError(error);
    } finally {
      session.busy = false;
      session.updateControls();
    }
    if (session.playing)
      session.timer = setTimeout(session.advance, 1400 - Number($("speed").value) * 125);
  };

  // Marca la entrada como modificada: hay que volver a cargar antes de avanzar.
  session.watchInput = (element) => element.addEventListener("input", () => {
    session.dirty = true;
    session.pause();
  });

  // Lee un .txt; la sesión queda bloqueada mientras dura la lectura.
  session.readFile = (fileInput, receive) => {
    const file = fileInput.files[0];
    if (!file) return;
    session.pause();
    if (file.size > 30000) {
      session.showError(new Error("Archivo demasiado grande."));
      fileInput.value = "";
      return;
    }
    session.reading = true;
    session.updateControls();
    const reader = new FileReader();
    reader.onload = () => {
      session.reading = false;
      receive(String(reader.result), file.name);
      fileInput.value = "";
    };
    reader.onerror = () => {
      session.reading = false;
      session.showError(new Error("No se pudo leer el archivo."));
      fileInput.value = "";
    };
    reader.readAsText(file, "UTF-8");
  };

  $("nextButton").addEventListener("click", () => session.advance());
  $("playButton").addEventListener("click", () => {
    if (session.playing) return session.pause();
    if (session.busy || session.reading || session.dirty || !session.state
      || session.state.finished) return;
    session.playing = true;
    session.advance();
  });
  $("resetButton").addEventListener("click", () => session.load(true));
  $("loadButton").addEventListener("click", () => session.load(false));
  $("speed").addEventListener("input", () => { $("speedValue").textContent = $("speed").value; });
  session.updateControls();
  return session;
}
