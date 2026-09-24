/* Actividad Integradora 1 Declaración del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <vector>
#include <string>

using namespace std;

class SuffixArray {
  public:
    /// Constructor para el suffix array de dos strings concatenados -- O(|s₁| + |s₂|)
  	SuffixArray(const string&, const string&);

   /// Constructor para el suffix array de un solo string -- O(|s|)
  	SuffixArray(const string&);

    /// Determina si un patrón p se encuentra contenido en el texto s -- O(|p| + |s|)
    int find(const string&);

    /// Encuentra el Longest Common Substring de dos strings -- O(|s₁| + |s₂|)
  	vector<int> lcs();
  	
  private:
  	char DELIMITER = '\0';
  
  	string fullText = "";
  	string textA = "";
  	string textB = "";
  
    vector<int> sortedSa = {};
    vector<int> lcp = {};
  	vector<int> sa = {};

    /// Verifica si dos sufijos pertenecen a diferentes textos -- O(1)
    bool belongToDistinctStrings(const int&, const int&);

    /// Encuentra el match más grande de p en s, desde s[start] -- O(|p|)
    void iteratePattern(const string&, const int&, int&);
    
    /// Crea el Suffix Array y el Suffix Array inverso --O(|s|)
    void buildSuffixArray();

    /// Ordena el SA con el algoritmo SA-IS --O(|s|)
    void sortSuffixArray();
    
    /// Crea el Longest Common Prefix Array, ignorando delimitadores -- O(|s|)
  	void buildLcpArray();
};
