/* Actividad Integradora 1 Algoritmo de Manacher 
 *   - Franco De Escondrillas | A01739410
 *   - Octavio Hernández Loyo | A01739304
 * Fecha: 2026-09-24 */

#pragma once

#include <algorithm>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Manacher {
  public:

    Manacher(const string&);

    pair<int, int> longestPalindrome() const;

  private:

    char DELIMITER = '\0';

    string fullText = "";
    string palindromeText = "";
    vector<int> radius = {};
    int idxBest = 0;

    void buildPalindromeArray();
};
