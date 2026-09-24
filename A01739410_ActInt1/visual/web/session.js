/* Actividad Integradora 1 Control de sesión de la visualización
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

"use strict";

const $ = (id) => document.getElementById(id);

function visibleChar(character) {
  if (character === "\n") return "↵";
  if (character === "\r") return "␍";
  return character;
}

// Solo esta capa conoce HTTP.
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
//   mode: prefijo de las rutas /api/<mode>/reset, /step y /state
//   fields(): cuerpo del RESET, con el índice de cada archivo elegido
//   columns(): cuántas columnas caben ahora, si la vista dibuja una rejilla
//   updateState(data): normaliza el mensaje de C++ y llama a session.publish
//   render(state): dibuja el estado ya normalizado
function createSession(view) {
  // El ancho medido viaja con cada petición: C++ recorta su ventana a él.
  let lastColumns = null;
  const withColumns = (body) => {
    if (!view.columns) return body;
    lastColumns = view.columns();
    return Object.assign({ columns: lastColumns }, body);
  };
  const session = {
    state: null, busy: false, playing: false, timer: null, resizeTimer: null,
    transport: {
      reset: (body) => request("/api/" + view.mode + "/reset", withColumns(body)),
      next: () => request("/api/" + view.mode + "/step", withColumns({})),
      state: () => request("/api/" + view.mode + "/state", withColumns({}))
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

  session.publish = (state) => {
    session.state = state;
    view.render(state);
    if (state.finished) session.pause();
    session.updateControls();
  };

  // Una sola petición en vuelo: los botones quedan inertes mientras C++ responde.
  session.run = async (ask) => {
    session.busy = true;
    session.updateControls();
    try {
      view.updateState(await ask());
    } catch (error) {
      session.showError(error);
    } finally {
      session.busy = false;
      session.updateControls();
    }
    session.refit();
  };

  // Construye la sesión desde el principio con los archivos elegidos.
  session.load = async () => {
    if (session.busy) return;
    session.pause();
    $("error").hidden = true;
    await session.run(() => session.transport.reset(view.fields()));
  };

  session.advance = async () => {
    if (session.busy || !session.state || session.state.finished) return;
    await session.run(session.transport.next);
    if (session.playing)
      session.timer = setTimeout(session.advance, 1400 - Number($("speed").value) * 125);
  };

  // Vuelve a pedir el paso actual, sin avanzarlo, cuando cambia el ancho útil.
  session.refresh = async () => {
    if (session.busy || !session.state) return;
    await session.run(session.transport.state);
  };

  // El ancho útil solo se conoce del todo con la rejilla ya dibujada: el tamaño
  // del arreglo decide cuánto miden las columnas. Se vuelve a medir después de
  // cada respuesta y de cada cambio de tamaño, y el paso se repite si cambió.
  session.refit = () => {
    if (!view.columns || session.busy || !session.state) return;
    if (view.columns() !== lastColumns) session.refresh();
  };

  // Al cambiar el tamaño de la ventana caben otras columnas.
  if (view.columns)
    window.addEventListener("resize", () => {
      clearTimeout(session.resizeTimer);
      session.resizeTimer = setTimeout(session.refit, 150);
    });

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
