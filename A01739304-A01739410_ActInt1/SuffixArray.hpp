/* Actividad Integradora 1 Declaración del suffix array
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class SuffixArray {
  public:
  	SuffixArray(const string&, const string&);
  	SuffixArray(const string&);

    string longestPalindrome();
    int find(const string&);
  	string lcs();
  	
  private:
  	char DELIMITER = '\0';
  
  	string fullText = "";
  	string textA = "";
  	string textB = "";
  
    vector<string> patterns = {};
    vector<int> sortedSa = {};
    vector<int> lcpRight = {};
    vector<int> lcpLeft = {};
    vector<int> lcp = {};
  	vector<int> sa = {};

    bool belongToDistinctStrings(const int&, const int&);
    void iteratePattern(const string&, const int&, int&);
    int buildLcpLR(int = 0, int = -1);
    void buildSuffixArray();
    void sortSuffixArray();
  	void buildLcpArray();
};
