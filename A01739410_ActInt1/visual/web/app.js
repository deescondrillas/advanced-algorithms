/* Actividad Integradora 1 Vista de Manacher
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

"use strict";

// El control de la sesión vive en session.js.
const SVG = "http://www.w3.org/2000/svg";
const colors = {
  text: "#181818", muted: "#707070", amber: "#303030", mint: "#707070",
  blue: "#909090", green: "#555555", red: "#222222", cell: "#ffffff"
};
let state = null;
let currentText = "";
let previousI = -2;

function svgElement(name, attributes, text) {
  const node = document.createElementNS(SVG, name);
  for (const [key, value] of Object.entries(attributes)) node.setAttribute(key, value);
  if (text !== undefined) node.textContent = text;
  return node;
}

// Recibe datos de C++; no calcula radios ni ejecuta Manacher en JavaScript.
// text es el archivo del test que eligió la interfaz: se envía al cargar y los
// mensajes siguientes pueden omitirlo.
// start/end son inclusivos desde 1. i/center/right/mirror son índices desde 0.
function updateState(data) {
  if (!data || typeof data !== "object") throw new Error("Estado inválido.");
  if (data.text !== undefined && typeof data.text !== "string") throw new Error("Cadena inválida.");
  const text = data.text !== undefined ? data.text : currentText;
  const radius = data.radius !== undefined ? data.radius : (state ? state.radius : []);
  if (!Array.isArray(radius)) throw new Error("Arreglo de radios inválido.");
  if (data.text !== undefined) {
    currentText = text;
    $("testLabel").textContent = data.test + " · " + text.length + " caracteres";
    previousI = -2;
  }
  const merged = Object.assign({
    i: -1, center: 0, right: 0, mirror: -1, matches: 0,
    compareLeft: -1, compareRight: -1, comparison: -1,
    phase: "ready", step: 0, comparisons: 0, finished: false,
    start: 0, end: 0, length: 0, offset: 0
  }, data.text !== undefined ? {} : state, data, { text, radius });
  merged.size = data.size !== undefined ? data.size : text.length * 2 + 1;
  session.publish(merged);
}
window.updateState = updateState;

function drawGraph() {
  const graph = $("graph");
  graph.replaceChildren();
  graph.toggleAttribute("hidden", !state);
  if (!state) return;
  const count = state.radius.length;
  const width = count * 56 + 32;
  graph.setAttribute("width", width);
  graph.setAttribute("height", 244);
  graph.setAttribute("viewBox", "0 0 " + width + " 244");
  const defs = graph.appendChild(svgElement("defs", {}));
  defs.appendChild(svgElement("filter", { id: "lift", x: "-50%", y: "-50%", width: "200%", height: "200%" }))
    .appendChild(svgElement("feDropShadow", {
      dx: 0, dy: 2, stdDeviation: 2.5, "flood-color": "#181818", "flood-opacity": ".35"
    }));
  const x = (index) => 16 + (index - state.offset) * 56;
  const inWindow = (index) => index >= state.offset && index < state.offset + count;
  const clampX = (index) => Math.max(20, Math.min(width - 20, x(index) + 23));
  const add = (name, attrs, text) => graph.appendChild(svgElement(name, attrs, text));

  // La banda gris corresponde al palíndromo que fija center y right.
  if (state.right > 0) {
    const left = Math.max(state.offset, 2 * state.center - state.right);
    const right = Math.min(state.offset + count - 1, state.right);
    if (left <= right) add("rect", {
      x: x(left) - 4, y: 95, width: (right - left) * 56 + 54, height: 62,
      rx: 0, fill: "#f3f3f3", stroke: colors.blue, "stroke-dasharray": "4 5", opacity: ".7"
    });
  }
  if (state.mirror >= 0 && state.i !== state.mirror) {
    const start = clampX(state.mirror), end = clampX(state.i);
    const middle = (start + end) / 2;
    add("path", {
      d: "M " + start + " 92 Q " + middle + " -26 " + end + " 92",
      fill: "none", stroke: colors.mint, "stroke-width": "2", opacity: ".75"
    });
    add("text", { x: middle, y: 22, fill: colors.mint, "text-anchor": "middle", "font-size": "11" }, "mirror");
  }
  if (state.matches > 0) {
    const left = Math.max(state.offset, state.i - state.matches);
    const right = Math.min(state.offset + count - 1, state.i + state.matches);
    add("line", {
      x1: x(left), x2: x(right) + 46, y1: 154, y2: 154,
      stroke: colors.amber, "stroke-width": "3", "stroke-linecap": "round"
    });
  }

  const comparing = state.phase === "compare" || state.phase === "expand";
  for (let local = 0; local < count; ++local) {
    const index = state.offset + local;
    const character = index % 2 === 0 ? "#" : visibleChar(currentText[(index - 1) / 2]);
    let stroke = "#d0d0d0", fill = colors.cell, letter = colors.text;
    if (index % 2 === 0) letter = "#777777";
    if (index === state.mirror) { stroke = colors.mint; fill = "#e4e4e4"; }
    if (index === state.i) { stroke = colors.amber; fill = "#d2d2d2"; }
    if (comparing && (index === state.compareLeft || index === state.compareRight)) {
      stroke = state.comparison === 1 ? colors.green : colors.red;
      fill = state.comparison === 1 ? "#eeeeee" : "#e3e3e3";
    }
    add("text", { x: x(index) + 23, y: 86, fill: colors.muted, "text-anchor": "middle", "font-size": "11" }, index);
    const lifted = index === state.i ? { filter: "url(#lift)" } : {};
    add("rect", Object.assign({ x: x(index), y: 102, width: 46, height: 48, rx: 0, fill, stroke, "stroke-width": index === state.i ? 2 : 1.5 }, lifted));
    add("text", { x: x(index) + 23, y: 133, fill: letter, "text-anchor": "middle", "font-size": "21", "font-family": "Consolas, monospace", "font-weight": "600" }, character);
    add("rect", Object.assign({ x: x(index), y: 191, width: 46, height: 32, rx: 0, fill: "#ffffff", stroke: index === state.i ? colors.amber : "#d9d9d9" }, lifted));
    add("text", { x: x(index) + 23, y: 212, fill: index === state.i ? colors.amber : colors.text, "text-anchor": "middle", "font-size": "15", "font-family": "Consolas, monospace", "font-weight": index === state.i ? "600" : "400" }, state.radius[local] === null ? "·" : state.radius[local]);
  }
  add("text", { x: 16, y: 179, fill: colors.muted, "font-size": "10", "letter-spacing": "1" }, "RADIOS");
  if (state.i >= 0) {
    const active = x(state.i) + 23;
    add("path", { d: "M " + (active - 5) + " 65 L " + (active + 5) + " 65 L " + active + " 73 Z", fill: colors.amber });
    add("text", { x: active, y: 58, fill: colors.amber, "font-size": "12", "text-anchor": "middle" }, "i");
  }
  if (state.right > 0 && inWindow(state.right)) add("text", { x: x(state.right) + 23, y: 164, fill: colors.blue, "font-size": "10", "text-anchor": "middle" }, "R");
  if (state.right > 0 && inWindow(state.center)) add("text", { x: x(state.center) + 23, y: 164, fill: colors.blue, "font-size": "10", "text-anchor": "middle" }, state.center === state.right ? "C · R" : "C");

  $("windowLabel").textContent = state.size > count
    ? "Ventana " + state.offset + "–" + (state.offset + count - 1) + " de " + state.size
    : state.size + (state.size === 1 ? " posición" : " posiciones");
  graph.setAttribute("aria-label", "Texto transformado. Centro actual " + state.i + ", radio " + state.matches + ", límite derecho " + state.right);
  if (state.i !== previousI) {
    const viewport = $("graphScroll");
    viewport.scrollLeft = Math.max(0, x(Math.max(0, state.i)) - viewport.clientWidth / 2 + 23);
    previousI = state.i;
  }
}


function render(current) {
  state = current;
  drawGraph();
  for (const [id, key] of [
    ["varI", "i"], ["varCenter", "center"], ["varRight", "right"],
    ["varMirror", "mirror"], ["varMatches", "matches"]
  ]) $(id).textContent = state[key] < 0 ? "—" : state[key];
  $("comparisons").textContent = state.comparisons;
  $("stepNumber").textContent = state.step;
  const phases = {
    ready: "En pausa", select: "Centro", mirror: "Espejo", compare: "Comparación",
    expand: "Expansión", boundary: "Borde", commit: "Actualización", done: "Finalizado"
  };
  $("phase").textContent = phases[state.phase] || state.phase;
  $("bestLabel").textContent = state.finished ? "Palíndromo" : "Palíndromo parcial";
  $("bestLength").textContent = state.length;
  $("bestStart").textContent = state.length || state.finished ? state.start : "—";
  $("bestEnd").textContent = state.length || state.finished ? state.end : "—";
  const best = state.length ? currentText.slice(state.start - 1, state.end) : "";
  $("bestText").textContent = best ? Array.from(best, visibleChar).join("") : "—";
}

const transmission = createChooser("transmissionChoice", () => session.load());

const session = createSession({
  mode: "manacher",
  fields: () => ({ transmission: transmission.value() }),
  updateState,
  render
});

// La transmisión del test se anima desde el primer paso, sin cargarla a mano.
session.load();
