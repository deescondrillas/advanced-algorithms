/* Actividad Integradora 1 Rejilla de sufijos compartida
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

"use strict";

// Una columna por entrada del suffix array; cada sufijo se escribe hacia abajo.
const DEPTH = 12;

function textCell(tag, text, className) {
  const cell = document.createElement(tag);
  cell.textContent = text;
  if (className) cell.className = className;
  return cell;
}

// Último índice visible de un sufijo: el delimitador lo cierra, igual que al LCP.
function suffixLimit(fullText, start) {
  const stop = fullText.indexOf("$", start);
  return stop === -1 ? fullText.length - 1 : stop;
}

// spec = { fullText, sa, lcp, origin, offset, columnClass(column),
//          cellClass(column, depth), pattern: [{ character, className }] | null }
// Los arreglos llegan recortados a la ventana que envió C++; offset es su inicio.
function drawGrid(table, spec) {
  const body = document.createElement("tbody");
  const count = spec.sa.length;
  const limits = spec.sa.map((start) => suffixLimit(spec.fullText, start));

  const addRow = (label, patternCell, className, fill) => {
    const row = document.createElement("tr");
    row.className = className;
    row.appendChild(textCell("th", label, "row-label"));
    if (spec.pattern) row.appendChild(patternCell);
    for (let local = 0; local < count; ++local) {
      const column = spec.offset + local;
      const cell = fill(local, column);
      cell.className = (cell.className + " " + spec.columnClass(column)).trim();
      row.appendChild(cell);
    }
    body.appendChild(row);
  };

  const character = (local, depth) => {
    const index = spec.sa[local] + depth;
    if (index > limits[local]) return "";
    if (depth === DEPTH - 1 && index < limits[local]) return "…";
    return visibleChar(spec.fullText[index]);
  };

  addRow("índice", textCell("th", ""), "numbers index", (local, column) => textCell("td", column));
  addRow("lcp", textCell("th", ""), "numbers", (local) => textCell("td", spec.lcp[local]));
  addRow("sa", textCell("th", "patrón", "pattern-title"), "numbers",
    (local) => textCell("td", spec.sa[local]));
  for (let depth = 0; depth < DEPTH; ++depth) {
    const entry = spec.pattern ? spec.pattern[depth] : null;
    addRow(depth ? "" : "sufijos",
      textCell("th", entry ? entry.character : "", "pattern-cell " + (entry ? entry.className : "")),
      "letters",
      (local, column) => textCell("td", character(local, depth), spec.cellClass(column, depth)));
  }
  if (spec.origin)
    addRow("origen", textCell("th", ""), "numbers",
      (local) => textCell("td", spec.origin[local] ? spec.origin[local] : "·"));

  table.replaceChildren(body);
}

// Centra la columna activa sin arrastrar la página al desplazarse.
function centerColumn(viewport, table) {
  const active = table.querySelector("td.col-active");
  if (active) viewport.scrollLeft = active.offsetLeft - viewport.clientWidth / 2;
}
