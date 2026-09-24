/* Actividad Integradora 1 Implementación del algoritmo de Manacher 
 *   - Franco De Escondrillas | A01739410
 *   - Octavio Hernández Loyo | A01739304
 * Fecha: 2026-09-24 */

#include "Manacher.hpp"

/// Constructor para encontrar el palíndromo más largo -- O(|s|)
/// Conserva todos los caracteres recibidos
Manacher::Manacher(const string& theFullText) {
  this->fullText = theFullText;
  palindromeText += DELIMITER;
  for (char &current : fullText) {
    palindromeText += current;
    palindromeText += DELIMITER;
}

  radius.resize(palindromeText.size());
  buildPalindromeArray();
}

/// Crea los radios de los palíndromos usando simetría -- O(|s|)
/// center y right localizan el palíndromo más lejano a la derecha
void Manacher::buildPalindromeArray() {
  int center = 0;
  int right = 0;

  for (int i = 0; i < palindromeText.size(); ++i) {
    int matches = 0;
    if (i < right) {
      int mirror = center - (i - center);
      matches = min(radius[mirror], right - i);
    }

    // Comprueba únicamente los caracteres extras de la simetría que pueden haber
    while (
      i - matches - 1 >= 0 && 
      i + matches + 1 < palindromeText.size() && 
      palindromeText[i - matches - 1] == palindromeText[i + matches + 1]
    ) ++matches;

    radius[i] = matches;
    if (i + matches > right) {
      right = i + matches;
      center = i;
    }
    if (matches > radius[idxBest])
      idxBest = i;
  }
}

/// Recupera inicio y fin del palíndromo más largo -- O(1)
pair<int, int> Manacher::longestPalindrome() const {
  if (fullText.empty())
    return {0, 0};
  int start = (idxBest - radius[idxBest]) / 2;
  return {start, start + radius[idxBest] - 1};
}
