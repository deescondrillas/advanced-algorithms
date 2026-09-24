/* Actividad Integradora 1 Programa principal 
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include <iostream>
#include <fstream>
#include <string>

#include "SuffixArray.hpp"
#include "Manacher.hpp"

using namespace std;

/// Lee los archivos de los tests, de acuerdo con PATH
void parse(const string&, vector<string>&, vector<string>&);

/// Busca la aparición de patrones p en secuencias s -- O(|s| + |p|)
void part1(vector<string>&, vector<string>&);

/// Busca el palíndromo más grande en cada secuencia s -- O(|s|)
void part2(vector<string>&, vector<string>&);

/// Busca el LCS de dos secuencias s₁ y s₂ -- O(|s₁| + |s₂|)
void part3(vector<string>&, vector<string>&);

int main() {
  // Seleccionar test
  string PATH = "tests/test1/";

  vector<string> transmissions, mcodes;
  parse(PATH, transmissions, mcodes);

  part1(transmissions, mcodes);
  cout << endl;
  part2(transmissions, mcodes);
  cout << endl;
  part3(transmissions, mcodes);

  return 0;
}

void parse(const string& PATH, vector<string>& transmissions, vector<string>& mcodes) {
  string transmission1, transmission2, mcode1, mcode2, mcode3;
  ifstream t1(PATH + "transmission1.txt");
  ifstream t2(PATH + "transmission2.txt");
  ifstream mc1(PATH + "mcode1.txt");
  ifstream mc2(PATH + "mcode2.txt");
  ifstream mc3(PATH + "mcode3.txt");

  t1 >> transmission1;
  t2 >> transmission2;
  mc1 >> mcode1;
  mc2 >> mcode2;
  mc3 >> mcode3;

  transmissions = {transmission1, transmission2};
  mcodes = {mcode1, mcode2, mcode3};
}

void part1(vector<string>& transmissions, vector<string>& mcodes) {
  cout << "=== Parte 1 ===" << endl;
  for (string& transmission : transmissions) {
    SuffixArray transmissionSA(transmission);
    for (string& mcode : mcodes) {
      int answer = transmissionSA.find(mcode);
      if (answer < 0)
        cout << "false" << endl;
      else
        cout << "true " << answer + 1 << endl;
    }
  }
}

void part2(vector<string>& transmissions, vector<string>& mcodes) {
  cout << "=== Parte 2 ===" << endl;
  for (string& transmission : transmissions) {
    Manacher palindromeFinder(transmission);
    pair<int, int> answer = palindromeFinder.longestPalindrome();
    cout << answer.first + 1 << ' ' << answer.second + 1 << endl;
  }
}

void part3(vector<string>& transmissions, vector<string>& mcodes) {
  cout << "=== Parte 3 ===" << endl;
  SuffixArray bothTransmissions(transmissions[0], transmissions[1]);
  vector<int> answer = bothTransmissions.lcs();
  cout << answer[0] + 1 << ' ' << answer[1] + 1 << " --transmission1" << endl;
  cout << answer[2] + 1 << ' ' << answer[3] + 1 << " --transmission2" << endl;
}
