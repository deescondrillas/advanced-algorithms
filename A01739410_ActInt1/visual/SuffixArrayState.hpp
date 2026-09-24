/* Actividad Integradora 1 Estados observables del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <string>
using namespace std;

/// Datos de una operación de lcs; los índices internos comienzan en 0
struct LcsState {
  int i{-1}, step{0}, lcpHere{0}, idxBest{0}, bestLength{0};
  bool distinct{false}, improved{false}, finished{false};
  int startA{0}, endA{-1}, startB{0}, endB{-1};
  string phase{"ready"};
};

/// Datos de una operación de find; los índices internos comienzan en 0
struct FindState {
  int step{0}, lcpHere{0}, column{-1}, start{-1}, firstMatchIdx{-1}, matches{0};
  int comparePattern{-1}, compareText{-1}, comparison{-1}, comparisons{0};
  string phase{"ready"};
  bool finished{false};
};
