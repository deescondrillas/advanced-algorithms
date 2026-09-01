/* Actividad 2.1 Implementación de búsqueda de prefijos con fuerza bruta
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#pragma once

#include <vector>
#include <string>

using namespace std;

// Búsqueda de prefijos con fuerza bruta --O(|n|)
vector<string> bruteForce(const vector<string>& words, const string& prefix) {
  vector<string> result;
  for (string word : words)
    for (int i = 0; i < min(word.size(), prefix.size()); ++i)
      if (word[i] != prefix[i])
        break;
      else if (i == prefix.size() - 1)
        result.push_back(word);
  return result;
}
