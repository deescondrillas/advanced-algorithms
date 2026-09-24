/* Actividad Integradora 1 Implementación del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "SuffixArray.hpp"
#include "SaIs.hpp"

/// Constructor para el suffix array de dos strings concatenados -- O(|s₁| + |s₂|)
/// Útil cuando el propósito es encontrar Longest Common Substring
SuffixArray::SuffixArray(const string& textA, const string& textB) {
  this->fullText = textA + DELIMITER + textB + DELIMITER;
  this->sortedSa.resize(fullText.size());
  this->lcp.resize(fullText.size());
  this->sa.resize(fullText.size());
  this->textA = textA;
  this->textB = textB;
  
  buildSuffixArray();
  buildLcpArray();
}


/// Constructor para el suffix array de un solo string -- O(|s|)
/// Útil para encontrar buscar matches de un patrón
SuffixArray::SuffixArray(const string& theFullText) {
  this->fullText = theFullText + DELIMITER;
  this->sortedSa.resize(fullText.size());
  this->lcp.resize(fullText.size());
  this->sa.resize(fullText.size());
  
  buildSuffixArray();
  buildLcpArray();
}


/// Determina si un patrón p se encuentra contenido en el texto s -- O(|p| + |s|)
/// Se emplea una búsqueda secuencial, similar a KMP
int SuffixArray::find(const string& pattern) {
  int firstMatchIdx = -1;
  int matches = 0;
  for (int& start : sa) {
    if (lcp[sortedSa[start]] < matches)
      return firstMatchIdx;
    iteratePattern(pattern, start, matches);
    if (matches == pattern.size())
      firstMatchIdx = firstMatchIdx == -1 ? start : min(firstMatchIdx, start);
  }
  return firstMatchIdx;
} 


/// Encuentra el Longest Common Substring de dos strings -- O(|s₁| + |s₂|)
/// Regresa {inicio en textA, fin en textA, inicio en textB, fin en textB}
vector<int> SuffixArray::lcs() {
  int idxBest = 0;

  for (int i = 1; i < sa.size(); ++i) {
    int idxCurSuffix = sa[i - 1];
    int idxNextSuffix = sa[i];
    if (
      belongToDistinctStrings(idxCurSuffix, idxNextSuffix)
      && (lcp[i] > lcp[idxBest])
    ) idxBest = i;
  }

  int idxA = min(sa[idxBest], sa[idxBest - 1]);
  int idxB = max(sa[idxBest], sa[idxBest - 1]);

  int startA = idxA;
  int endA = startA + lcp[idxBest] - 1;
  int startB = idxB - (textA.size() + 1);
  int endB = startB + lcp[idxBest] - 1;

  return {startA, endA, startB, endB};
}


/// Verifica si dos sufijos pertenecen a diferentes textos -- O(1)
/// Uno deberá ser menor y el otro mayor a textA.size()
bool SuffixArray::belongToDistinctStrings(const int& i, const int& j) {
  if (fullText[i] == DELIMITER || fullText[j] == DELIMITER)
    return false;
  return int(i - textA.size()) * int(j - textA.size()) < 0;
}


/// Encuentra el match más grande de p en s, desde s[start] -- O(|p|)
/// Itera sobre p y s mientras el siguiente caracter coincida
void SuffixArray::iteratePattern(const string& pattern, const int& start, int& match) {
  while (
    match < pattern.size() &&
    match < fullText.size() - start &&
    pattern[match] == fullText[start + match]
  ) match++;
}


/// Crea el Suffix Array y el Suffix Array inverso --O(|s|)
/// SA índice -> posición · sortedSA posición -> índice
void SuffixArray::buildSuffixArray() {
  for (int i = 0; i < sa.size(); ++i)
    sa[i] = i;
  sortSuffixArray();
  for (int i = 0; i < sa.size(); ++i)
    sortedSa[sa[i]] = i;
}


/// Ordena el SA con el algoritmo SA-IS -- O(|s|)
/// Recodifica el texto a enteros
void SuffixArray::sortSuffixArray() {
  vector<int> text(fullText.size());
  for (int i = 0; i < fullText.size(); ++i)
    text[i] = (unsigned char) fullText[i] + 1;
  text.back() = 0;

  sa = saIs(text, BYTE_ALPHABET);
}


/// Crea el Longest Common Prefix Array, ignorando delimitadores -- O(|s|)
/// Utiliza el algoritmo de Kasai para la construcción lineal
void SuffixArray::buildLcpArray() {
  int matches = 0;
  for (int suffix = 0; suffix < sa.size(); ++suffix)
    if (sortedSa[suffix]) {    
      int prevSuffix = sa[sortedSa[suffix] - 1];
  
      while (
        max(suffix, prevSuffix) + matches < sa.size() &&
        fullText[suffix + matches] != DELIMITER &&
        fullText[suffix + matches] == fullText[prevSuffix + matches]
      ) matches++;  
      
      lcp[sortedSa[suffix]] = matches;
      matches -= matches ? 1 : 0;
    }
}
