/* Actividad Integradora 1 Implementación de la sesión de Manacher
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "ManacherSession.hpp"
#include "../../Manacher.hpp"

#include <iostream>
#include "../ManacherState.hpp"
#include "SessionText.hpp"

namespace {
// Cada respuesta contiene solo una ventana de radios, no una copia del arreglo.
// La cadena viaja una vez al cargar; los pasos siguientes solo envían estado.
void writeState(const ManacherState& state, const vector<int>& radius,
                const string* text = nullptr, const string* test = nullptr) {
  int length = state.length;
  int size = static_cast<int>(radius.size());
  int offset = max(0, min(max(0, state.i) - 15, size - 31));
  int end = min(size, offset + 31);

  cout << "{\"phase\":\"" << state.phase << "\",\"i\":" << state.i
       << ",\"center\":" << state.center << ",\"right\":" << state.right
       << ",\"mirror\":" << state.mirror << ",\"matches\":" << state.matches
       << ",\"compareLeft\":" << state.compareLeft
       << ",\"compareRight\":" << state.compareRight
       << ",\"comparison\":" << state.comparison
       << ",\"comparisons\":" << state.comparisons << ",\"reused\":" << state.reused
       << ",\"step\":" << state.step << ",\"finished\":" << (state.finished ? "true" : "false")
       << ",\"start\":" << (length ? state.start + 1 : 0)
       << ",\"end\":" << (length ? state.end + 1 : 0)
       << ",\"length\":" << length
       << ",\"mirrorRadius\":" << (state.mirror >= 0 ? radius[state.mirror] : 0)
       << ",\"size\":" << size << ",\"offset\":" << offset << ",\"radius\":[";
  for (int i = offset; i < end; ++i) {
    if (i != offset) cout << ',';
    if (i > state.i) cout << "null";
    else cout << (i == state.i ? state.matches : radius[i]);
  }
  cout << "]";
  if (text) {
    cout << ",\"text\":";
    writeJsonText(*text);
    cout << ",\"test\":";
    writeJsonText(*test);
  }
  cout << "}" << endl;
}
}

namespace {
// Interrumpir una sesión destruye el cálculo anterior antes de animar otra transmisión.
// parseReset ya dejó el índice elegido en indices, así que el aviso viaja vacío.
struct RestartSession {};
struct EndSession {};
}

// El observador publica un paso y espera NEXT antes de devolver el control
// al mismo for/while que utiliza Manacher en una ejecución normal.
int runManacherSession(const string& test) {
  vector<string> transmissions, mcodes;
  if (!readTestData(test, transmissions, mcodes)) {
    cout << "{\"error\":\"No se pudieron leer los archivos del test\"}" << endl;
    return 1;
  }

  const vector<int> counts = {static_cast<int>(transmissions.size())};
  vector<int> indices;
  string command;

  // La interfaz pide la primera transmisión al abrirse; no se anima nada antes.
  while (getline(cin, command)) {
    if (parseReset(command, counts, indices))
      break;
    cout << "{\"error\":\"Comando desconocido\"}" << endl;
  }
  if (!cin) return 0;

  for (;;) {
    const string& text = transmissions[indices[0]];
    try {
      Manacher palindrome(text, [&](const ManacherState& state, const vector<int>& radius) {
        writeState(state, radius, state.phase == "ready" ? &text : nullptr, &test);
        while (getline(cin, command)) {
          if (command == "NEXT" && !state.finished)
            return;
          if (command == "NEXT" || command == "STATE") {
            writeState(state, radius);
          } else if (parseReset(command, counts, indices)) {
            throw RestartSession{};
          } else {
            cout << "{\"error\":\"Comando desconocido\"}" << endl;
          }
        }
        throw EndSession{};
      });
      return 0;
    } catch (const RestartSession&) {
    } catch (const EndSession&) {
      return 0;
    }
  }
}
