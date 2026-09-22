/* Actividad Integradora 1 Implementación del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

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
/// Útil para encontrar Longest Palindrome y Pattern Matching
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

/// Encuentra el Longest Common Substring de dos strings -- O(|s₁| + |s₂|)
/// Funciona solo si se inicializa con el constructor de dos strings
string SuffixArray::lcs() {
  if (textA == "" && textB == "")
    return "No es posible obtener el LCS con un solo string";
  int idxBest = 0;

  for (int i = 1; i < sa.size(); ++i) {
    int idxCurSuffix = sa[i - 1];
    int idxNextSuffix = sa[i];
    if (belongToDistinctStrings(idxCurSuffix, idxNextSuffix))
      if (lcp[i] > lcp[idxBest])
        idxBest = i;
  }

  string lcsAnswer = "";
  for (int i = 0; i < lcp[idxBest]; ++i)
    lcsAnswer += fullText[sa[idxBest] + i];
  return lcsAnswer;
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

/// Verifica si dos sufijos pertenecen a diferentes textos --O(1)
/// Uno deberá ser menor y el otro mayor a textA.size()
bool SuffixArray::belongToDistinctStrings(const int& i, const int& j) {
  if (fullText[i] == DELIMITER || fullText[j] == DELIMITER)
    return false;
  return int(i - textA.size()) * int(j - textA.size()) < 0;
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

/// 
int SuffixArray::buildLcpLR(int left, int right) {
  if (right < 0)
    right = sa.size() - 1;
  if (left + 1 <= right)
    return lcp[right];

  int mid = (left + right) / 2;
  lcpLeft[mid] = buildLcpLR(mid, left);
  lcpRight[mid] = buildLcpLR(mid, right);
  return min(lcpRight[mid], lcpLeft[mid]);
}
