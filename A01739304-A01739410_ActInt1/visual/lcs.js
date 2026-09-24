"use strict";

// Vista del Longest Common Substring. El control vive en session.js y la
// rejilla en grid.js; aquí solo se interpreta el estado que envía C++.
let textA = "";
let textB = "";
let fullText = "";
let previousColumn = -2;

// El suffix array recorre textA + delimitador + textB + delimitador.
// El delimitador se dibuja como $ y ocupa exactamente una posición.
function joinTexts() {
  fullText = textA + "$" + textB + "$";
}

// Recibe datos de C++; no ordena sufijos ni recorre la ventana en JavaScript.
// textA y textB se envían al cargar; los mensajes siguientes pueden omitirlos.
function updateState(data) {
  if (!data || typeof data !== "object") throw new Error("Estado inválido.");
  if (data.textA !== undefined) {
    if (typeof data.textA !== "string" || typeof data.textB !== "string")
      throw new Error("Cadenas inválidas.");
    textA = data.textA;
    textB = data.textB;
    joinTexts();
    $("inputA").value = textA;
    $("inputB").value = textB;
    previousColumn = -2;
    session.dirty = false;
  }
  const merged = Object.assign({
    i: -1, lcpHere: 0, idxBest: 0, distinct: false, improved: false,
    phase: "ready", step: 0, finished: false,
    length: 0, startA: 0, endA: 0, startB: 0, endB: 0,
    sa: [], lcp: [], origin: [], offset: 0, size: 0
  }, data.textA !== undefined ? {} : session.state, data);
  session.publish(merged);
}
window.updateState = updateState;

const phases = {
  ready: "Cargado", select: "Ventana", check: "Comparación",
  update: "Nuevo máximo", done: "Finalizado"
};

function render(state) {
  // La ventana compara dos columnas vecinas del suffix array: i-1 e i.
  const inWindow = (column) => column === state.i || column === state.i - 1;
  const inBest = (column) => state.length > 0
    && (column === state.idxBest || column === state.idxBest - 1);

  drawGrid($("grid"), {
    fullText, sa: state.sa, lcp: state.lcp, origin: state.origin, offset: state.offset,
    pattern: null,
    columnClass: (column) => [
      inWindow(column) ? "col-window" : "",
      column === state.i ? "col-active" : "",
      inBest(column) ? "col-best" : ""
    ].join(" ").trim(),
    // El prefijo común de la ventana mide lcp[i] caracteres.
    cellClass: (column, depth) => inWindow(column) && depth < state.lcpHere ? "cell-shared" : ""
  });
  if (state.i !== previousColumn) {
    centerColumn($("gridScroll"), $("grid"));
    previousColumn = state.i;
  }

  $("varI").textContent = state.i < 0 ? "—" : state.i;
  $("varLcp").textContent = state.i < 0 ? "—" : state.lcpHere;
  $("varBest").textContent = state.idxBest;
  $("varLength").textContent = state.length;
  $("varDistinct").textContent = state.i < 0 ? "—" : (state.distinct ? "sí" : "no");
  $("stepNumber").textContent = state.step;
  $("phase").textContent = phases[state.phase] || state.phase;
  $("windowLabel").textContent = state.size > state.sa.length
    ? "Ventana " + state.offset + "–" + (state.offset + state.sa.length - 1) + " de " + state.size
    : state.size + " sufijos";

  $("bestLabel").textContent = state.finished ? "Substring común" : "Substring parcial";
  $("bestLength").textContent = state.length;
  const best = state.length ? textA.slice(state.startA - 1, state.endA) : "";
  $("bestText").textContent = best ? Array.from(best, visibleChar).join("") : "—";
  $("rangeA").textContent = state.length ? state.startA + " · " + state.endA : "—";
  $("rangeB").textContent = state.length ? state.startB + " · " + state.endB : "—";
}

const session = createSession({
  mode: "lcs",
  inputs: () => [$("loadButton"), $("fileA"), $("fileB"),
                 $("fileInputA"), $("fileInputB"), $("inputA"), $("inputB")],
  fields: (original) => {
    const first = original ? textA : $("inputA").value;
    const second = original ? textB : $("inputB").value;
    if (!first || !second || invalidText(first) || invalidText(second)) {
      session.showError(new Error(
        "Entrada inválida: cada transmisión usa de 1 a 10,000 caracteres; 0–9, A–F, CR y LF."));
      return null;
    }
    return { textA: first, textB: second };
  },
  updateState,
  render
});

for (const [button, input, area, source] of [
  ["fileA", "fileInputA", "inputA", "sourceA"],
  ["fileB", "fileInputB", "inputB", "sourceB"]
]) {
  session.watchInput($(area));
  $(area).addEventListener("input", () => { $(source).textContent = ""; });
  $(button).addEventListener("click", () => $(input).click());
  $(input).addEventListener("change", () => session.readFile($(input), (text, name) => {
    $(area).value = text;
    $(source).textContent = name;
    session.load(false);
  }));
}
