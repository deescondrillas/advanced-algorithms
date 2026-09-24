/* Actividad Integradora 1 Implementación de las utilidades compartidas
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "SessionText.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace {
/// Lee la primera palabra del archivo, igual que el parse de main.cpp
bool readFile(const string& path, string& text) {
  ifstream file(path);
  return bool(file >> text);
}
}

// Los mismos archivos que ejecuta main con ese test.
bool readTestData(const string& path, vector<string>& transmissions, vector<string>& mcodes) {
  transmissions.assign(2, "");
  mcodes.assign(3, "");

  for (int i = 0; i < 2; ++i)
    if (!readFile(path + "transmission" + to_string(i + 1) + ".txt", transmissions[i]))
      return false;
  for (int i = 0; i < 3; ++i)
    if (!readFile(path + "mcode" + to_string(i + 1) + ".txt", mcodes[i]))
      return false;

  return true;
}

// RESET [índice...]: elige qué archivo del test anima cada campo. Un índice
// fuera de rango o ausente se acota, así que la sesión nunca queda sin texto.
bool parseReset(const string& command, const vector<int>& counts, vector<int>& indices) {
  if (command != "RESET" && command.compare(0, 6, "RESET ") != 0) return false;

  istringstream rest(command.substr(5));
  indices.assign(counts.size(), 0);
  for (size_t i = 0; i < counts.size(); ++i) {
    int value = 0;
    if (rest >> value)
      indices[i] = max(0, min(value, counts[i] - 1));
  }

  return true;
}

void writeJsonText(const string& text) {
  cout << '"';
  for (char current : text) {
    if (current == '\n') cout << "\\n";
    else if (current == '\r') cout << "\\r";
    else if (current == '"' || current == '\\') cout << '\\' << current;
    else cout << current;
  }
  cout << '"';
}
