"use strict";

const $ = (id) => document.getElementById(id);

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

// Grupo de botones que elige uno de los archivos del test. No hay texto
// editable: el índice viaja a C++, que es quien abre el archivo.
function createChooser(id, onChange) {
  const buttons = Array.from($(id).querySelectorAll("button"));
  let chosen = 0;
  const mark = () => buttons.forEach((button, index) =>
    button.setAttribute("aria-pressed", index === chosen ? "true" : "false"));
  buttons.forEach((button, index) => button.addEventListener("click", () => {
    if (index === chosen) return;
    chosen = index;
    mark();
    onChange();
  }));
  mark();
  return { value: () => chosen };
}

// Control de ejecución común a las tres vistas. Cada vista aporta:
//   mode: prefijo de las rutas /api/<mode>/reset y /api/<mode>/step
//   fields(): cuerpo del RESET, con el índice de cada archivo elegido
//   updateState(data): normaliza el mensaje de C++ y llama a session.publish
//   render(state): dibuja el estado ya normalizado
function createSession(view) {
  const session = {
    state: null, busy: false, playing: false, timer: null,
    transport: {
      reset: (body) => request("/api/" + view.mode + "/reset", body),
      next: () => request("/api/" + view.mode + "/step", {})
    }
  };
  window.algorithmTransport = session.transport;

  session.updateControls = () => {
    const ready = Boolean(session.state);
    $("playButton").textContent = session.playing ? "Pausar" : "Reproducir";
    $("playButton").disabled = !ready || session.state.finished
      || (session.busy && !session.playing);
    $("nextButton").disabled = !ready || session.busy || session.playing
      || session.state.finished;
    $("resetButton").disabled = !ready || session.busy;
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

  // Construye la sesión desde el principio con los archivos elegidos.
  session.load = async () => {
    if (session.busy) return;
    session.pause();
    session.busy = true;
    $("error").hidden = true;
    session.updateControls();
    try {
      view.updateState(await session.transport.reset(view.fields()));
    } catch (error) {
      session.showError(error);
    } finally {
      session.busy = false;
      session.updateControls();
    }
  };

  session.advance = async () => {
    if (session.busy || !session.state || session.state.finished) return;
    session.busy = true;
    session.updateControls();
    try {
      view.updateState(await session.transport.next());
    } catch (error) {
      session.showError(error);
    } finally {
      session.busy = false;
      session.updateControls();
    }
    if (session.playing)
      session.timer = setTimeout(session.advance, 1400 - Number($("speed").value) * 125);
  };

  $("nextButton").addEventListener("click", () => session.advance());
  $("playButton").addEventListener("click", () => {
    if (session.playing) return session.pause();
    if (session.busy || !session.state || session.state.finished) return;
    session.playing = true;
    session.advance();
  });
  $("resetButton").addEventListener("click", () => session.load());
  $("speed").addEventListener("input", () => { $("speedValue").textContent = $("speed").value; });
  session.updateControls();
  return session;
}
