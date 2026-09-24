/* Actividad Integradora 1 Declaración del algoritmo SA-IS
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <vector>

using namespace std;

/// Tamaño del alfabeto al recodificar cada byte del texto como caracter + 1
const int BYTE_ALPHABET = 257;

/// Construye el suffix array de un texto entero con SA-IS -- O(|s|)
vector<int> saIs(const vector<int>&, const int&);
