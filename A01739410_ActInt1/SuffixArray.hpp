/* Actividad Integradora 1 Declaración del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <functional>
#include <vector>
#include <string>

using namespace std;

struct LcsState;
struct FindState;

class SuffixArray {
  public:
    /// Observadores opcionales; reciben cada operación con el SA y el LCP
    using FindObserver = function<void(const FindState&, const vector<int>&, const vector<int>&)>;
    using LcsObserver = function<void(const LcsState&, const vector<int>&, const vector<int>&)>;

    /// Constructor para el suffix array de dos strings concatenados -- O(|s₁| + |s₂|)
  	SuffixArray(const string&, const string&);

   /// Constructor para el suffix array de un solo string -- O(|s|)
  	SuffixArray(const string&);

    /// Determina si un patrón p se encuentra contenido en el texto s -- O(|p| + |s|)
    int find(const string&, const FindObserver& observer = FindObserver());

    /// Encuentra el Longest Common Substring de dos strings -- O(|s₁| + |s₂|)
  	vector<int> lcs(const LcsObserver& observer = LcsObserver());
  	
  private:
  	const char DELIMITER = '\0';
  
  	string fullText = "";
  	string textA = "";
  	string textB = "";
  
    vector<int> sortedSa = {};
    vector<int> lcp = {};
  	vector<int> sa = {};

    /// Verifica si dos sufijos pertenecen a diferentes textos -- O(1)
    bool belongToDistinctStrings(const int&, const int&);

    /// Traduce la mejor posición del LCP a los rangos del LCS -- O(1)
    vector<int> lcsResult(const int&) const;

    /// Encuentra el match más grande de p en s, desde s[start] -- O(|p|)
    void iteratePattern(const string&, const int&, int&,
                        const function<void(bool)>& report = function<void(bool)>());
    
    /// Crea el Suffix Array y el Suffix Array inverso --O(|s|)
    void buildSuffixArray();

    /// Ordena el SA con el algoritmo SA-IS --O(|s|)
    void sortSuffixArray();
    
    /// Crea el Longest Common Prefix Array, ignorando delimitadores -- O(|s|)
  	void buildLcpArray();
};
