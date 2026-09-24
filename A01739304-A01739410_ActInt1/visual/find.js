"use strict";

// Vista de la búsqueda de un patrón. El control vive en session.js y la
// rejilla en grid.js; aquí solo se interpreta el estado que envía C++.
let text = "";
let pattern = "";
let fullText = "";
let previousColumn = -2;

// Recibe datos de C++; no ordena sufijos ni compara caracteres en JavaScript.
// text y pattern se envían al cargar; los mensajes siguientes pueden omitirlos.
function updateState(data) {
  if (!data || typeof data !== "object") throw new Error("Estado inválido.");
  if (data.text !== undefined) {
    if (typeof data.text !== "string" || typeof data.pattern !== "string")
      throw new Error("Cadenas inválidas.");
    text = data.text;
    pattern = data.pattern;
    fullText = text + "$";
    $("inputText").value = text;
    $("inputPattern").value = pattern;
    previousColumn = -2;
    session.dirty = false;
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
  ready: "Cargado", select: "Sufijo", prune: "Poda por LCP",
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

const session = createSession({
  mode: "find",
  inputs: () => [$("loadButton"), $("fileText"), $("fileInputText"),
                 $("inputText"), $("inputPattern")],
  fields: (original) => {
    const haystack = original ? text : $("inputText").value;
    const needle = original ? pattern : $("inputPattern").value;
    if (!haystack || !needle || invalidText(haystack) || invalidText(needle)) {
      session.showError(new Error(
        "Entrada inválida: transmisión y patrón usan de 1 a 10,000 caracteres; 0–9, A–F, CR y LF."));
      return null;
    }
    return { text: haystack, pattern: needle };
  },
  updateState,
  render
});

session.watchInput($("inputText"));
session.watchInput($("inputPattern"));
$("inputText").addEventListener("input", () => { $("sourceText").textContent = ""; });
$("fileText").addEventListener("click", () => $("fileInputText").click());
$("fileInputText").addEventListener("change", () =>
  session.readFile($("fileInputText"), (content, name) => {
    $("inputText").value = content;
    $("sourceText").textContent = name;
    session.load(false);
  }));
