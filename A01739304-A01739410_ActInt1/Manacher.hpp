/* Actividad Integradora 1 Algoritmo de Manacher 
 *   - Franco De Escondrillas | A01739410
 *   - Octavio Hernández Loyo | A01739304
 * Fecha: 2026-09-24 */

#pragma once

#include <string>
#include <vector>

using namespace std;

class Manacher {
  public:
    /// Constructor para encontrar el palíndromo más largo -- O(|s|)
    Manacher(const string&);

    /// Recupera inicio y fin del palíndromo más largo -- O(1)
    pair<int, int> longestPalindrome() const;

  private:
    const char DELIMITER = '\0';

    string palindromeText = "";
    string fullText = "";

    vector<int> radius = {};
    int idxBest = 0;

    /// Crea los radios de los palíndromos usando simetría -- O(|s|)
    void buildPalindromeArray();
};
