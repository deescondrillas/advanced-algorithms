/* Actividad Integradora 1 Implementación del algoritmo de Manacher 
 *   - Franco De Escondrillas | A01739410
 *   - Octavio Hernández Loyo | A01739304
 * Fecha: 2026-09-24 */

#include "Manacher.hpp"
#include "visual/ManacherState.hpp"

/// Constructor para encontrar el palíndromo más largo -- O(|s|)
/// Conserva todos los caracteres recibidos
Manacher::Manacher(const string& theFullText, const Observer& observer) {
  this->fullText = theFullText;
  palindromeText.reserve(2 * fullText.size() + 1);
  palindromeText += DELIMITER;
  for (char &current : fullText) {
    palindromeText += current;
    palindromeText += DELIMITER;
  }

  radius.resize(palindromeText.size());
  buildPalindromeArray(observer);
}

/// Crea los radios de los palíndromos usando simetría -- O(|s|)
/// center y right localizan el palíndromo más lejano a la derecha
void Manacher::buildPalindromeArray(const Observer& observer) {
  int center = 0;
  int right = 0;
  int textSize = static_cast<int>(palindromeText.size());
  ManacherState state;

  // El observador recibe el estado real; sin él, el recorrido termina normalmente.
  auto report = [&](const char* phase) {
    if (!observer) return;
    state.phase = phase;
    state.center = center;
    state.right = right;
    state.length = radius[idxBest];
    pair<int, int> result = longestPalindrome();
    state.start = state.length ? result.first : 0;
    state.end = state.length ? result.second : 0;
    observer(state, radius);
    ++state.step;
  };
  report("ready");

  for (int i = 0; i < textSize; ++i) {
    int matches = 0;
    state.i = i;
    state.matches = 0;
    state.mirror = -1;
    state.compareLeft = state.compareRight = -1;
    state.comparison = -1;
    report("select");
    if (i < right) {
      int mirror = center - (i - center);
      matches = min(radius[mirror], right - i);
      state.mirror = mirror;
      state.reused += matches;
    }
    state.matches = matches;
    report("mirror");

    // Comprueba únicamente los caracteres extras de la simetría que pueden haber
    while (
      i - matches - 1 >= 0 && 
      i + matches + 1 < textSize
    ) {
      state.compareLeft = i - matches - 1;
      state.compareRight = i + matches + 1;
      state.comparison = palindromeText[state.compareLeft] == palindromeText[state.compareRight];
      ++state.comparisons;
      report("compare");
      if (!state.comparison) break;
      ++matches;
      state.matches = matches;
      report("expand");
    }
    if (i - matches - 1 < 0 || i + matches + 1 >= textSize) {
      state.compareLeft = i - matches - 1;
      state.compareRight = i + matches + 1;
      state.comparison = -1;
      report("boundary");
    }

    radius[i] = matches;
    if (i + matches > right) {
      right = i + matches;
      center = i;
    }
    if (matches > radius[idxBest])
      idxBest = i;
    report("commit");
  }
  state.finished = true;
  report("done");
}

/// Recupera inicio y fin del palíndromo más largo -- O(1)
pair<int, int> Manacher::longestPalindrome() const {
  if (fullText.empty())
    return {0, 0};
  int start = (idxBest - radius[idxBest]) / 2;
  return {start, start + radius[idxBest] - 1};
}
