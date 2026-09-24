/* Actividad Integradora 1 Vista de find
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

"use strict";

// El control vive en session.js y la rejilla en grid.js; aquí solo se
// interpreta el estado que envía C++.
let text = "";
let pattern = "";
let fullText = "";
let previousColumn = -2;

// Recibe datos de C++; no ordena sufijos ni compara caracteres en JavaScript.
// text y pattern son los archivos del test que eligió la interfaz: se envían al
// cargar y los mensajes siguientes pueden omitirlos.
function updateState(data) {
  if (!data || typeof data !== "object") throw new Error("Estado inválido.");
  if (data.text !== undefined) {
    if (typeof data.text !== "string" || typeof data.pattern !== "string")
      throw new Error("Cadenas inválidas.");
    text = data.text;
    pattern = data.pattern;
    fullText = text + "$";
    $("testLabel").textContent = data.test + " · transmisión de " + text.length
      + " caracteres · patrón de " + pattern.length;
    previousColumn = -2;
  }
  const merged = Object.assign({
    column: -1, start: -1, matches: 0, lcpHere: 0,
    comparePattern: -1, compareText: -1, comparison: -1,
    phase: "ready", step: 0, comparisons: 0, finished: false, firstMatch: 0,
    sa: [], lcp: [], offset: 0, size: 0
  }, data.text !== undefined ? {} : session.state, data);
  session.publish(merged);
}
window.updateState = updateState;

const phases = {
  ready: "En pausa", select: "Sufijo", prune: "Poda por LCP",
  compare: "Comparación", match: "Coincidencia", done: "Finalizado"
};

// Clase de la celda a profundidad depth, tanto del patrón como del sufijo activo.
function matchClass(state, depth) {
  if (state.phase === "compare" && depth === state.comparePattern)
    return state.comparison === 1 ? "cell-match cell-current" : "cell-mismatch cell-current";
  return depth < state.matches ? "cell-match" : "";
}

function render(state) {
  const patternColumn = [];
  for (let depth = 0; depth < DEPTH; ++depth) {
    const last = depth === DEPTH - 1 && pattern.length > DEPTH;
    patternColumn.push({
      character: last ? "…" : visibleChar(pattern[depth] !== undefined ? pattern[depth] : ""),
      className: depth < pattern.length ? matchClass(state, depth) : ""
    });
  }

  drawGrid($("grid"), {
    fullText, sa: state.sa, lcp: state.lcp, origin: null, offset: state.offset,
    pattern: patternColumn,
    columnClass: (column) => column === state.column ? "col-active" : "",
    // El patrón solo se compara contra el sufijo de la columna activa.
    cellClass: (column, depth) => column === state.column ? matchClass(state, depth) : ""
  });
  if (state.column !== previousColumn) {
    centerColumn($("gridScroll"), $("grid"));
    previousColumn = state.column;
  }

  $("varColumn").textContent = state.column < 0 ? "—" : state.column;
  $("varStart").textContent = state.start < 0 ? "—" : state.start;
  $("varMatches").textContent = state.matches;
  $("varLcp").textContent = state.column < 0 ? "—" : state.lcpHere;
  $("varPattern").textContent = pattern.length;
  $("stepNumber").textContent = state.step;
  $("comparisons").textContent = state.comparisons;
  $("phase").textContent = phases[state.phase] || state.phase;
  $("windowLabel").textContent = state.size > state.sa.length
    ? "Ventana " + state.offset + "–" + (state.offset + state.sa.length - 1) + " de " + state.size
    : state.size + " sufijos";

  $("bestLabel").textContent = state.finished ? "Resultado" : "Resultado parcial";
  $("bestText").textContent = state.firstMatch ? "true" : (state.finished ? "false" : "—");
  $("bestStart").textContent = state.firstMatch ? state.firstMatch : "—";
}

const transmission = createChooser("transmissionChoice", () => session.load());
const mcode = createChooser("mcodeChoice", () => session.load());

const session = createSession({
  mode: "find",
  fields: () => ({ transmission: transmission.value(), mcode: mcode.value() }),
  updateState,
  render
});

// El par del test se busca desde el primer paso, sin cargarlo a mano.
session.load();
