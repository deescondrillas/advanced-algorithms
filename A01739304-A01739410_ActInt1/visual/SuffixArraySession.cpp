#include "SuffixArraySession.hpp"
#include "../SuffixArray.hpp"

#include <iostream>
#include "SuffixArrayState.hpp"
#include "SessionText.hpp"

namespace {
// Columnas del suffix array publicadas por paso; la interfaz dibuja solo esa ventana.
const int COLUMNS = 25;

// Interrumpir una sesión destruye el cálculo anterior antes de cargar otros textos.
struct RestartSession { string textA, textB; };
struct EndSession {};

// RESET <hexA> <hexB>: los dos textos del lcs, o el texto y el patrón del find.
bool parseReset(const string& command, string& textA, string& textB) {
  if (command.compare(0, 6, "RESET ") != 0) return false;
  size_t split = command.find(' ', 6);
  if (split == string::npos) return false;
  string first, second;
  if (!decodeText(command.substr(6, split - 6), first)) return false;
  if (!decodeText(command.substr(split + 1), second)) return false;
  if (first.empty() || second.empty()) return false;
  textA = first;
  textB = second;
  return true;
}

// La ventana sigue a la columna activa, como la de radios en la sesión de Manacher.
int windowOffset(const int& active, const int& size) {
  return max(0, min(max(0, active) - COLUMNS / 2, size - COLUMNS));
}

// Cada respuesta contiene solo una ventana del SA, no una copia de los arreglos.
// origin marca de qué texto viene el sufijo: 1 textA, 2 textB, 0 delimitador.
void writeWindow(const vector<int>& sa, const vector<int>& lcp,
                 const int& offset, const int& textASize) {
  int size = static_cast<int>(sa.size());
  int end = min(size, offset + COLUMNS);
  cout << ",\"size\":" << size << ",\"offset\":" << offset << ",\"sa\":[";
  for (int i = offset; i < end; ++i)
    cout << (i != offset ? "," : "") << sa[i];
  cout << "],\"lcp\":[";
  for (int i = offset; i < end; ++i)
    cout << (i != offset ? "," : "") << lcp[i];
  cout << "]";
  if (textASize < 0) return;
  cout << ",\"origin\":[";
  for (int i = offset; i < end; ++i) {
    int start = sa[i];
    int origin = start < textASize ? 1 : (start > textASize && start < size - 1 ? 2 : 0);
    cout << (i != offset ? "," : "") << origin;
  }
  cout << "]";
}

// Los textos viajan una sola vez al cargar; los pasos siguientes solo envían estado.
void writeTexts(const string& textA, const string& textB, const char* nameA, const char* nameB) {
  cout << ",\"" << nameA << "\":";
  writeJsonText(textA);
  cout << ",\"" << nameB << "\":";
  writeJsonText(textB);
}

void writeLcsState(const LcsState& state, const vector<int>& sa, const vector<int>& lcp,
                   const string& textA, const string& textB, bool includeTexts) {
  int length = state.bestLength;
  cout << "{\"phase\":\"" << state.phase << "\",\"i\":" << state.i
       << ",\"lcpHere\":" << state.lcpHere << ",\"idxBest\":" << state.idxBest
       << ",\"distinct\":" << (state.distinct ? "true" : "false")
       << ",\"improved\":" << (state.improved ? "true" : "false")
       << ",\"step\":" << state.step
       << ",\"finished\":" << (state.finished ? "true" : "false")
       << ",\"length\":" << length
       << ",\"startA\":" << (length ? state.startA + 1 : 0)
       << ",\"endA\":" << (length ? state.endA + 1 : 0)
       << ",\"startB\":" << (length ? state.startB + 1 : 0)
       << ",\"endB\":" << (length ? state.endB + 1 : 0);
  writeWindow(sa, lcp, windowOffset(state.i, static_cast<int>(sa.size())),
              static_cast<int>(textA.size()));
  if (includeTexts) writeTexts(textA, textB, "textA", "textB");
  cout << "}" << endl;
}

void writeFindState(const FindState& state, const vector<int>& sa, const vector<int>& lcp,
                    const string& text, const string& pattern, bool includeTexts) {
  cout << "{\"phase\":\"" << state.phase << "\",\"column\":" << state.column
       << ",\"start\":" << state.start << ",\"matches\":" << state.matches
       << ",\"lcpHere\":" << state.lcpHere
       << ",\"comparePattern\":" << state.comparePattern
       << ",\"compareText\":" << state.compareText
       << ",\"comparison\":" << state.comparison
       << ",\"comparisons\":" << state.comparisons << ",\"step\":" << state.step
       << ",\"finished\":" << (state.finished ? "true" : "false")
       << ",\"firstMatch\":" << state.firstMatchIdx + 1;
  writeWindow(sa, lcp, windowOffset(state.column, static_cast<int>(sa.size())), -1);
  if (includeTexts) writeTexts(text, pattern, "text", "pattern");
  cout << "}" << endl;
}

// Publica un paso y espera NEXT antes de devolver el control al mismo for/while
// que utiliza SuffixArray en una ejecución normal.
template <typename Writer>
void awaitNext(const Writer& write, const bool& finished) {
  string command;
  while (getline(cin, command)) {
    if (command == "NEXT" && !finished)
      return;
    if (command == "NEXT" || command == "STATE") {
      write(false);
    } else if (command.compare(0, 6, "RESET ") == 0) {
      RestartSession reset;
      if (parseReset(command, reset.textA, reset.textB))
        throw reset;
      cout << "{\"error\":\"Texto invalido\"}" << endl;
    } else {
      cout << "{\"error\":\"Comando desconocido\"}" << endl;
    }
  }
  throw EndSession{};
}

// Espera el primer RESET válido antes de construir nada.
bool awaitFirstReset(string& textA, string& textB, const char* help) {
  string command;
  while (getline(cin, command)) {
    textA.clear();
    textB.clear();
    if (parseReset(command, textA, textB))
      return true;
    cout << "{\"error\":\"" << help << "\"}" << endl;
  }
  return false;
}
}

int runLcsSession() {
  string textA, textB;
  if (!awaitFirstReset(textA, textB, "Carga dos transmisiones con RESET")) return 0;

  for (;;) {
    try {
      SuffixArray bothTexts(textA, textB);
      bothTexts.lcs([&](const LcsState& state, const vector<int>& sa, const vector<int>& lcp) {
        auto write = [&](bool includeTexts) {
          writeLcsState(state, sa, lcp, textA, textB, includeTexts);
        };
        write(state.phase == "ready");
        awaitNext(write, state.finished);
      });
      return 0;
    } catch (const RestartSession& reset) {
      textA = reset.textA;
      textB = reset.textB;
    } catch (const EndSession&) {
      return 0;
    }
  }
}

int runFindSession() {
  string text, pattern;
  if (!awaitFirstReset(text, pattern, "Carga una transmision y un patron con RESET")) return 0;

  for (;;) {
    try {
      SuffixArray textSa(text);
      textSa.find(pattern, [&](const FindState& state, const vector<int>& sa, const vector<int>& lcp) {
        auto write = [&](bool includeTexts) {
          writeFindState(state, sa, lcp, text, pattern, includeTexts);
        };
        write(state.phase == "ready");
        awaitNext(write, state.finished);
      });
      return 0;
    } catch (const RestartSession& reset) {
      text = reset.textA;
      pattern = reset.textB;
    } catch (const EndSession&) {
      return 0;
    }
  }
}
