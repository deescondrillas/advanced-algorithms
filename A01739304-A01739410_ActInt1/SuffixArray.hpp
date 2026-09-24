/* Actividad Integradora 1 Declaración del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

class SuffixArray {
  public:
  	SuffixArray(const string&, const string&);
  	SuffixArray(const string&);

    int find(const string&);
  	vector<int> lcs();
  	
  private:
  	char DELIMITER = '\0';
  
  	string fullText = "";
  	string textA = "";
  	string textB = "";
  
    vector<int> sortedSa = {};
    vector<int> lcp = {};
  	vector<int> sa = {};

    bool belongToDistinctStrings(const int&, const int&);
    void iteratePattern(const string&, const int&, int&);
    void buildSuffixArray();
  	void buildLcpArray();
};
