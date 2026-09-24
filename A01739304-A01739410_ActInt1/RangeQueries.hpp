/* Actividad Integradora 1 Declaración de la estructura para RMQ
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <vector>

using namespace std;

struct Node {
  int minIdx, minLcp;
  Node (int idx = -1, int lcs = -1): minIdx(idx), minLcp(lcs) {};
  Node operator + (const Node &other) {
    return Node (min(minIdx, other.minIdx), min(minLcp, other.minLcp));
  }
};

class RangeQueries {
  public:
    RangeQueries(const vector<int>&, const vector<int>&);
    Node minQuery(int, int);
  private:
    void dfsInit(const vector<int>&, const vector<int>&, int, int, int);
    Node minQuery(int, int, int);
    
    vector<Node> segmentTree;
    int rangeStart = 0;
    int arraySize = 0;
    int rangeEnd = 0;
};
