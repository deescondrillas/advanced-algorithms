/* Actividad Integradora 1 Implementación de las sesiones del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "SuffixArraySession.hpp"
#include "../../SuffixArray.hpp"

#include <iostream>
#include <sstream>
#include "../SuffixArrayState.hpp"
#include "SessionText.hpp"

namespace {
// Columnas del suffix array publicadas por paso; la interfaz dibuja solo esa ventana.
// La interfaz mide cuántas caben en su ancho y las pide con COLUMNS, para que la
// rejilla llegue al borde de la tarjeta en lugar de cortarse antes.
const int MIN_COLUMNS = 5, MAX_COLUMNS = 200;
int columns = 25;

// COLUMNS <n>: el número llega medido por la interfaz y se acota al rango útil.
bool parseColumns(const string& command) {
  if (command.compare(0, 8, "COLUMNS ") != 0) return false;

  istringstream rest(command.substr(8));
  int value = 0;
  if (rest >> value)
    columns = max(MIN_COLUMNS, min(value, MAX_COLUMNS));

  return true;
}

// Interrumpir una sesión destruye el cálculo anterior antes de animar otros archivos.
// parseReset ya dejó los índices elegidos en indices, así que el aviso viaja vacío.
struct RestartSession {};
struct EndSession {};

// La ventana sigue a la columna activa, como la de radios en la sesión de Manacher.
int windowOffset(const int& active, const int& size) {
  return max(0, min(max(0, active) - columns / 2, size - columns));
}

// Cada respuesta contiene solo una ventana del SA, no una copia de los arreglos.
// origin marca de qué texto viene el sufijo: 1 textA, 2 textB, 0 delimitador.
void writeWindow(const vector<int>& sa, const vector<int>& lcp,
                 const int& offset, const int& textASize) {
  int size = static_cast<int>(sa.size());
  int end = min(size, offset + columns);
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
void writeTexts(const string& textA, const string& textB, const char* nameA, const char* nameB,
                const string& test) {
  cout << ",\"" << nameA << "\":";
  writeJsonText(textA);
  cout << ",\"" << nameB << "\":";
  writeJsonText(textB);
  cout << ",\"test\":";
  writeJsonText(test);
}

void writeLcsState(const LcsState& state, const vector<int>& sa, const vector<int>& lcp,
                   const string& textA, const string& textB, const string& test,
                   bool includeTexts) {
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
  if (includeTexts) writeTexts(textA, textB, "textA", "textB", test);
  cout << "}" << endl;
}

void writeFindState(const FindState& state, const vector<int>& sa, const vector<int>& lcp,
                    const string& text, const string& pattern, const string& test,
                    bool includeTexts) {
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
  if (includeTexts) writeTexts(text, pattern, "text", "pattern", test);
  cout << "}" << endl;
}

// Publica un paso y espera NEXT antes de devolver el control al mismo for/while
// que utiliza SuffixArray en una ejecución normal.
template <typename Writer>
void awaitNext(const Writer& write, const bool& finished,
               const vector<int>& counts, vector<int>& indices) {
  string command;
  while (getline(cin, command)) {
    if (command == "NEXT" && !finished)
      return;
    if (command == "NEXT" || command == "STATE" || parseColumns(command)) {
      write(false);
    } else if (parseReset(command, counts, indices)) {
      throw RestartSession{};
    } else {
      cout << "{\"error\":\"Comando desconocido\"}" << endl;
    }
  }
  throw EndSession{};
}

// La interfaz pide los primeros archivos al abrirse; no se anima nada antes.
bool awaitFirstReset(const vector<int>& counts, vector<int>& indices) {
  string command;
  while (getline(cin, command)) {
    if (parseReset(command, counts, indices))
      return true;
    // El ancho llega antes del primer RESET; aún no hay paso que describir.
    if (parseColumns(command))
      cout << "{}" << endl;
    else
      cout << "{\"error\":\"Comando desconocido\"}" << endl;
  }
  return false;
}

// Los cinco archivos del test, leídos una vez al arrancar el proceso.
bool readTest(const string& test, vector<string>& transmissions, vector<string>& mcodes) {
  if (readTestData(test, transmissions, mcodes)) return true;
  cout << "{\"error\":\"No se pudieron leer los archivos del test\"}" << endl;
  return false;
}
}

// lcs siempre compara las dos transmisiones del test: RESET no lleva índices.
int runLcsSession(const string& test) {
  vector<string> transmissions, mcodes;
  if (!readTest(test, transmissions, mcodes)) return 1;

  const vector<int> counts;
  vector<int> indices;
  if (!awaitFirstReset(counts, indices)) return 0;

  const string& textA = transmissions[0];
  const string& textB = transmissions[1];
  for (;;) {
    try {
      SuffixArray bothTexts(textA, textB);
      bothTexts.lcs([&](const LcsState& state, const vector<int>& sa, const vector<int>& lcp) {
        auto write = [&](bool includeTexts) {
          writeLcsState(state, sa, lcp, textA, textB, test, includeTexts);
        };
        write(state.phase == "ready");
        awaitNext(write, state.finished, counts, indices);
      });
      return 0;
    } catch (const RestartSession&) {
    } catch (const EndSession&) {
      return 0;
    }
  }
}

// RESET <transmisión> <mcode>: cualquiera de los seis pares del test.
int runFindSession(const string& test) {
  vector<string> transmissions, mcodes;
  if (!readTest(test, transmissions, mcodes)) return 1;

  const vector<int> counts = {static_cast<int>(transmissions.size()),
                              static_cast<int>(mcodes.size())};
  vector<int> indices;
  if (!awaitFirstReset(counts, indices)) return 0;

  for (;;) {
    const string& text = transmissions[indices[0]];
    const string& pattern = mcodes[indices[1]];
    try {
      SuffixArray textSa(text);
      textSa.find(pattern, [&](const FindState& state, const vector<int>& sa, const vector<int>& lcp) {
        auto write = [&](bool includeTexts) {
          writeFindState(state, sa, lcp, text, pattern, test, includeTexts);
        };
        write(state.phase == "ready");
        awaitNext(write, state.finished, counts, indices);
      });
      return 0;
    } catch (const RestartSession&) {
    } catch (const EndSession&) {
      return 0;
    }
  }
}
