/* Actividad Integradora 1 Rejilla de sufijos compartida
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

"use strict";

// Una columna por entrada del suffix array; cada sufijo se escribe hacia abajo.
const DEPTH = 12;

// Medidas de respaldo, las que fija style.css: solo sirven antes del primer
// dibujo, porque el contenido ensancha las celdas y después se miden las reales.
const CELL_WIDTH = 25;      // table.grid td
const LEADING_WIDTH = 106;  // th.row-label mas th.pattern-cell
const SCROLL_PADDING = 14;  // .grid-scroll, su margen derecho
// Ni tan pocas que se pierda el contexto ni tantas que la ventana pese de más.
const MIN_COLUMNS = 5;
const MAX_COLUMNS = 200;

// Lo que ensancha una columna es el número más largo que puede caer en ella, el
// índice size-1; se mide con la misma fuente que usan las filas numéricas y más
// los bordes colapsados. Depende solo del tamaño del arreglo, no del paso.
function columnWidth(size) {
  const ruler = document.createElement("span");
  ruler.style.cssText = "position:absolute; visibility:hidden; white-space:pre;"
    + " font: 12px Consolas, monospace";
  ruler.textContent = String(Math.max(0, size - 1));
  document.body.appendChild(ruler);
  const width = ruler.getBoundingClientRect().width;
  ruler.remove();
  return Math.max(CELL_WIDTH, Math.ceil(width) + 4);
}

// Las etiquetas de fila y la columna del patrón van pegadas a la izquierda y no
// son columnas del suffix array, así que su ancho se descuenta.
function leadingWidth(table) {
  const row = table.querySelector("tr");
  if (!row) return LEADING_WIDTH;
  let width = 0;
  for (const cell of row.children)
    if (cell.tagName === "TH") width += cell.getBoundingClientRect().width;
  return width;
}

// Columnas que caben en el ancho visible; C++ recorta su ventana a ese número,
// así la rejilla llega al borde de la tarjeta en vez de cortarse antes.
function fittingColumns(viewport, table, size) {
  const free = viewport.clientWidth - SCROLL_PADDING - leadingWidth(table);
  return Math.max(MIN_COLUMNS,
    Math.min(MAX_COLUMNS, Math.floor(free / columnWidth(size))));
}

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

// spec = { fullText, sa, lcp, origin, offset, size, columnClass(column),
//          cellClass(column, depth), pattern: [{ character, className }] | null }
// Los arreglos llegan recortados a la ventana que envió C++; offset es su inicio.
function drawGrid(table, spec) {
  const body = document.createElement("tbody");
  // Todas las columnas miden igual, tantas como pidió fittingColumns.
  table.style.setProperty("--column-width", columnWidth(spec.size) + "px");
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
