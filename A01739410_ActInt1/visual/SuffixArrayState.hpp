/* Actividad Integradora 1 Estados observables del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <string>

// Datos de una operación de lcs; los índices internos comienzan en 0.
struct LcsState {
  std::string phase = "ready";
  int i = -1, lcpHere = 0;
  int idxBest = 0, bestLength = 0;
  bool distinct = false, improved = false;
  int startA = 0, endA = -1, startB = 0, endB = -1;
  int step = 0;
  bool finished = false;
};

// Datos de una operación de find; los índices internos comienzan en 0.
struct FindState {
  std::string phase = "ready";
  int column = -1, start = -1;
  int matches = 0, lcpHere = 0;
  int comparePattern = -1, compareText = -1, comparison = -1;
  int comparisons = 0, step = 0;
  int firstMatchIdx = -1;
  bool finished = false;
};
