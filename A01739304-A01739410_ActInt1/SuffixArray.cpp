/* Actividad Integradora 1 Implementación del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "RangeQueries.hpp"
#include "SuffixArray.hpp"


/// Constructor para el suffix array de dos strings concatenados -- O(|s₁| + |s₂|)
/// Útil cuando el propósito es encontrar Longest Common Substring
SuffixArray::SuffixArray(const string& textA, const string& textB) {
  this->fullText = textA + DELIMITER + textB + DELIMITER;
  this->sortedSa.resize(fullText.size());
  this->lcpRight.resize(fullText.size());
  this->lcpLeft.resize(fullText.size());
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
  this->lcpRight.resize(fullText.size());
  this->lcpLeft.resize(fullText.size());
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


/// Determina si un patrón p se encuentra contenido en el texto s -- O(|p| + log₂|s|)
/// Se emplea una búsqueda binaria sobre el SA con RMQ del LCP
int SuffixArray::bfind(const string& pattern) {
  RangeQueries segmentTree(sa, lcp);
  int rightBound = fullText.size();
  int greaterEqual = 0;
  int leftBound = 0;
  int lessEqual = 0;
  int matches = 0;
  int last = 0;
  int mid = 0;

  // Encontrar el sufijo menor o igual más grande
  mid = (leftBound + rightBound) / 2;
  last = mid;
  iteratePattern(pattern, sa[mid], matches);

  while (leftBound < rightBound) {
    mid = (leftBound + rightBound) / 2;
    if (mid < last) {
      // case 1: lower LCP malo
      if (matches > segmentTree.minQuery(mid, last).minLcp)
        leftBound = mid + 1;
      // case 2: equal 
      else 
        rightBound = mid;
    } else {
      if (matches > segmentTree.minQuery(last, mid).minLcp)
        rightBound = mid - 1;
      else 
        leftBound()
    }
  }

  rightBound = fullText.size();
  leftBound = 0;
  matches = 0;

  // Encontrar el sufijo mayor o igual más pequeño
  
  return segmentTree.minQuery(lessEqual, greaterEqual).minIdx;
}


/// Encuentra el Longest Common Substring de dos strings -- O(|s₁| + |s₂|)
/// Funciona solo si se inicializa con el constructor de dos strings
string SuffixArray::lcs() {
  int idxBest = 0;

  for (int i = 1; i < sa.size(); ++i) {
    int idxCurSuffix = sa[i - 1];
    int idxNextSuffix = sa[i];
    if (
      belongToDistinctStrings(idxCurSuffix, idxNextSuffix)
      && (lcp[i] > lcp[idxBest])
    ) idxBest = i;
  }

  string lcsAnswer = "";
  for (int i = 0; i < lcp[idxBest]; ++i)
    lcsAnswer += fullText[sa[idxBest] + i];
  return lcsAnswer;
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
    pattern[match] == fullText[start + match]
    && match < fullText.size() - start
    && match <  pattern.size()
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


/// Change later to SA-IS
void SuffixArray::sortSuffixArray() {
  sort(sa.begin(), sa.end(), [this](int sufixA, int sufixB) {
		return fullText.compare(sufixA, string::npos, fullText, sufixB, string::npos) < 0;
	});
}


/// Crea el Longest Common Prefix Array, ignorando delimitadores -- O(|s|)
/// Utiliza el algoritmo de Kasai para la construcción lineal
void SuffixArray::buildLcpArray() {
  int matches = 0;
  for (int suffix = 0; suffix < sa.size(); ++suffix)
    if (sortedSa[suffix]) {    
      int prevSuffix = sa[sortedSa[suffix] - 1];
  
      while (
        fullText[suffix + matches] == fullText[prevSuffix + matches]
        && max(suffix, prevSuffix) + matches < sa.size()
        && fullText[suffix + matches] != DELIMITER
      ) matches++;  
      
      lcp[sortedSa[suffix]] = matches;
      matches -= matches ? 1 : 0;
    }
}
