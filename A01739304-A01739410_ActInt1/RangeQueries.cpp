/* Actividad Integradora 1 Implementación de la estructura para RMQ
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include "RangeQueries.hpp"

#define right(index) (index + ((mid - leftBound + 1) << 1))
#define mid ((leftBound + rightBound) >> 1)
#define left(index) (index + 1)

/// Inicializa la estructura para RMQ con una dfs -- O(n)
RangeQueries::RangeQueries(const vector<int>& sa, const vector<int>& lcp) {
  this->arraySize = sa.size();
  segmentTree.resize(2 * arraySize);
  dfsInit(sa, lcp, 0, 0, arraySize - 1);
}

/// Hace las llamadas recursivas, por un total de 2*n -- O(n)
void RangeQueries::dfsInit(const vector<int>& sa, const vector<int>& lcp, int index, int leftBound, int rightBound) {
  if (leftBound == rightBound) {
    segmentTree[index].minIdx = sa[leftBound];
    segmentTree[index].minLcp = lcp[leftBound];
    return;
  }
  dfsInit(sa, lcp, left(index), leftBound, mid);
  dfsInit(sa, lcp, right(index), mid + 1, rightBound);
  segmentTree[index] = segmentTree[left(index)] + segmentTree[right(index)];
}

/// Construye el mínimo de un rango con segmentos -- O(log₂n)
Node RangeQueries::minQuery(int index, int leftBound, int rightBound) {
  if (rangeStart <= leftBound && rightBound <= rangeEnd) 
    return segmentTree[index];
  return minQuery(left(index), leftBound, mid) + minQuery(right(index), mid + 1, rightBound);
}

/// Sobrecarga del método minQuery para llamadas sencillas -- O(log₂n)
Node RangeQueries::minQuery(int rangeStart, int rangeEnd) {
  if (rangeStart == rangeEnd) 
    return Node(-1, arraySize);
  this->rangeStart = rangeStart;
  this->rangeEnd = rangeEnd;
  return minQuery(0, 0, arraySize - 1);
}
