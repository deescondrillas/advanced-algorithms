/* Actividad Integradora 1 Implementación del algoritmo SA-IS
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "SaIs.hpp"

/// Clasifica cada sufijo del texto como tipo S o tipo L -- O(n)
vector<bool> classifyTypes(const vector<int>&);

/// Determina si un caracter es leftmost S-type (LMS) -- O(1)
bool isLms(const vector<bool>&, const int&);

/// Localiza los caracteres LMS en su orden posicional, el arreglo P₁ -- O(n)
vector<int> findLmsPositions(const vector<bool>&);

/// Ordena por inducción a partir de los sufijos LMS recibidos -- O(n + |Σ|)
vector<int> induceSort(const vector<int>&, const vector<bool>&, const vector<int>&, const int&);

/// Localiza el inicio o el final de cada bucket del alfabeto -- O(n + |Σ|)
vector<int> bucketBounds(const vector<int>&, const int&, const bool&);

/// Nombra cada LMS-substring con el índice de su bucket -- O(n)
vector<int> nameLmsSubstrings(const vector<int>&, const vector<bool>&, const vector<int>&, const vector<int>&, int&);

/// Compara dos LMS-substrings por sus caracteres y por sus tipos -- O(n) amortizado
bool sameLmsSubstring(const vector<int>&, const vector<bool>&, const int&, const int&);

/// Construye el suffix array de un texto entero con SA-IS -- O(|s|)
vector<int> saIs(const vector<int>& text, const int& alphabet) {
  if (text.size() == 1)
    return {0};

  vector<bool> isSmall = classifyTypes(text);
  vector<int> lmsPositions = findLmsPositions(isSmall);
  vector<int> sa = induceSort(text, isSmall, lmsPositions, alphabet);

  int names = 0;
  vector<int> reduced = nameLmsSubstrings(text, isSmall, sa, lmsPositions, names);

  // Con todos los nombres distintos el orden ya es único, si no hay que recursar
  vector<int> reducedSa(reduced.size());
  if (names == reduced.size())
    for (int i = 0; i < reduced.size(); ++i)
      reducedSa[reduced[i]] = i;
  else
    reducedSa = saIs(reduced, names);

  vector<int> lmsOrder(reduced.size());
  for (int i = 0; i < reducedSa.size(); ++i)
    lmsOrder[i] = lmsPositions[reducedSa[i]];

  return induceSort(text, isSmall, lmsOrder, alphabet);
}


/// Clasifica cada sufijo del texto como tipo S o tipo L -- O(n)
/// S si suf(i) < suf(i + 1) · el centinela es S y decide los empates
vector<bool> classifyTypes(const vector<int>& text) {
  vector<bool> isSmall(text.size());
  isSmall.back() = true;

  for (int i = text.size() - 2; i >= 0; --i)
    isSmall[i] = text[i] == text[i + 1] ? isSmall[i + 1] : text[i] < text[i + 1];

  return isSmall;
}


/// Determina si un caracter es leftmost S-type (LMS) -- O(1)
/// Es el primer tipo S después de un tipo L, así que abre un LMS-substring
bool isLms(const vector<bool>& isSmall, const int& i) {
  return i > 0 && isSmall[i] && !isSmall[i - 1];
}


/// Localiza los caracteres LMS en su orden posicional, el arreglo P₁ -- O(n)
/// El último siempre es el centinela, el menor de todos los sufijos
vector<int> findLmsPositions(const vector<bool>& isSmall) {
  vector<int> lmsPositions;
  for (int i = 1; i < isSmall.size(); ++i)
    if (isLms(isSmall, i))
      lmsPositions.push_back(i);

  return lmsPositions;
}


/// Ordena por inducción a partir de los sufijos LMS recibidos -- O(n + |Σ|)
/// Cada sufijo siembra a su vecino izquierdo, que es una letra más largo
vector<int> induceSort(
  const vector<int>& text, const vector<bool>& isSmall,
  const vector<int>& lmsOrder, const int& alphabet
) {
  vector<int> sa(text.size(), -1);

  // Coloca los LMS al final de su bucket, conservando el orden recibido
  vector<int> bucket = bucketBounds(text, alphabet, false);
  for (int i = lmsOrder.size() - 1; i >= 0; --i)
    sa[--bucket[text[lmsOrder[i]]]] = lmsOrder[i];

  // Induce los tipo L al inicio de su bucket, recorriendo de izquierda a derecha
  bucket = bucketBounds(text, alphabet, true);
  for (int i = 0; i < sa.size(); ++i)
    if (sa[i] > 0 && !isSmall[sa[i] - 1])
      sa[bucket[text[sa[i] - 1]]++] = sa[i] - 1;

  // Induce los tipo S al final de su bucket, recorriendo de derecha a izquierda
  bucket = bucketBounds(text, alphabet, false);
  for (int i = sa.size() - 1; i >= 0; --i)
    if (sa[i] > 0 && isSmall[sa[i] - 1])
      sa[--bucket[text[sa[i] - 1]]] = sa[i] - 1;

  return sa;
}


/// Localiza el inicio o el final de cada bucket del alfabeto -- O(n + |Σ|)
/// Los sufijos con el mismo primer caracter ocupan posiciones contiguas del SA
vector<int> bucketBounds(const vector<int>& text, const int& alphabet, const bool& heads) {
  vector<int> bound(alphabet, 0);
  for (const int& character : text)
    bound[character]++;

  for (int character = 0, sum = 0; character < alphabet; ++character) {
    int size = bound[character];
    bound[character] = heads ? sum : sum + size;
    sum += size;
  }

  return bound;
}


/// Nombra cada LMS-substring con el índice de su bucket -- O(n)
/// Lee los nombres en el orden de P₁ para formar el texto reducido S₁
vector<int> nameLmsSubstrings(
  const vector<int>& text, const vector<bool>& isSmall,
  const vector<int>& sa, const vector<int>& lmsPositions, int& names
) {
  vector<int> name(text.size(), 0);
  int previous = -1;
  int current = -1;

  for (const int& suffix : sa)
    if (isLms(isSmall, suffix)) {
      if (previous < 0 || !sameLmsSubstring(text, isSmall, previous, suffix))
        current++;
      name[suffix] = current;
      previous = suffix;
    }
  names = current + 1;

  vector<int> reduced;
  for (const int& position : lmsPositions)
    reduced.push_back(name[position]);

  return reduced;
}


/// Compara dos LMS-substrings por sus caracteres y por sus tipos -- O(n) amortizado
/// Avanza hasta que ambos cierran en su siguiente LMS · el centinela es único
bool sameLmsSubstring(const vector<int>& text, const vector<bool>& isSmall, const int& i, const int& j) {
  if (i == text.size() - 1 || j == text.size() - 1)
    return false;

  for (int k = 0; ; ++k) {
    if (k && isLms(isSmall, i + k) && isLms(isSmall, j + k))
      return true;
    if (text[i + k] != text[j + k] || isSmall[i + k] != isSmall[j + k])
      return false;
  }
}
