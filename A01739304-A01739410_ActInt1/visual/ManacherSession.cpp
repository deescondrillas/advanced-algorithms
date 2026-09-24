#include "ManacherSession.hpp"
#include "../Manacher.hpp"

#include <iostream>
#include "ManacherState.hpp"

namespace {
// Cada respuesta contiene solo una ventana de radios, no una copia del arreglo.
void writeState(const ManacherState& state, const vector<int>& radius, const string* text = nullptr) {
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
  // La cadena se transmite una vez al cargar; los pasos siguientes solo envían estado.
  if (text) {
    cout << ",\"text\":\"";
    for (char current : *text) {
      if (current == '\n') cout << "\\n";
      else if (current == '\r') cout << "\\r";
      else cout << current; // decodeText solo admite 0-9, A-F, CR y LF.
    }
    cout << '"';
  }
  cout << "}" << endl;
}

int hexDigit(char value) {
  if (value >= '0' && value <= '9') return value - '0';
  if (value >= 'a' && value <= 'f') return value - 'a' + 10;
  if (value >= 'A' && value <= 'F') return value - 'A' + 10;
  return -1;
}

bool decodeText(const string& encoded, string& text) {
  if (encoded.size() % 2 || encoded.size() > 20000) return false;
  for (size_t i = 0; i < encoded.size(); i += 2) {
    int high = hexDigit(encoded[i]), low = hexDigit(encoded[i + 1]);
    if (high < 0 || low < 0) return false;
    char current = static_cast<char>(16 * high + low);
    if (!(current >= '0' && current <= '9') && !(current >= 'A' && current <= 'F')
        && current != '\n' && current != '\r') return false;
    text += current;
  }
  return true;
}
}

namespace {
// Interrumpir una sesión destruye el cálculo anterior antes de cargar otro texto.
struct RestartSession { string text; };
struct EndSession {};
}

// El observador publica un paso y espera NEXT antes de devolver el control
// al mismo for/while que utiliza Manacher en una ejecución normal.
int runManacherSession() {
  string text, command;
  while (getline(cin, command)) {
    text.clear();
    if (command.compare(0, 6, "RESET ") == 0 && decodeText(command.substr(6), text))
      break;
    cout << "{\"error\":\"Carga una transmision con RESET\"}" << endl;
  }
  if (!cin) return 0;

  for (;;) {
    try {
      Manacher palindrome(text, [&](const ManacherState& state, const vector<int>& radius) {
        writeState(state, radius, state.phase == "ready" ? &text : nullptr);
        while (getline(cin, command)) {
          if (command == "NEXT" && !state.finished)
            return;
          if (command == "NEXT" || command == "STATE") {
            writeState(state, radius);
          } else if (command.compare(0, 6, "RESET ") == 0) {
            string nextText;
            if (decodeText(command.substr(6), nextText))
              throw RestartSession{nextText};
            cout << "{\"error\":\"Texto invalido\"}" << endl;
          } else {
            cout << "{\"error\":\"Comando desconocido\"}" << endl;
          }
        }
        throw EndSession{};
      });
      return 0;
    } catch (const RestartSession& reset) {
      text = reset.text;
    } catch (const EndSession&) {
      return 0;
    }
  }
}
