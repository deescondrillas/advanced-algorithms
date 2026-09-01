/* Actividad 2.1 Implementación ...
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#include "BruteForce.hpp"
#include "Trie.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

int main() {
  const string TEST_FILE = "test_1e4.txt";
  vector<string> list;
  TrieNode trie;
  string word;
  
  ifstream file(TEST_FILE);
  while (file >> word) {
    trie.transform(word);
    list.push_back(word);
    trie.insert(word);
  }

  cout << "Test actual: " << TEST_FILE << "\n" << endl;

  string prefix;
  cout << "Buscar con prefijo: ";
  cin >> prefix;
  trie.transform(prefix);
  cout << endl;
  
  auto inicio = chrono::high_resolution_clock::now();
  auto resultado_fb = bruteForce(list, prefix);
  auto fin = chrono::high_resolution_clock::now();
  cout << "Fuerza bruta encontró " << resultado_fb.size() << " palabras en "
       << chrono::duration_cast<chrono::microseconds>(fin - inicio).count()
       << " µs" << endl;

  inicio = chrono::high_resolution_clock::now();
  auto resultado_trie = trie.complete(prefix);
  fin = chrono::high_resolution_clock::now();
  cout << "Trie encontró " << resultado_trie.size() << " palabras en "
      << chrono::duration_cast<chrono::microseconds>(fin - inicio).count() 
      << " µs" << endl;

  cout << "!!!Dado que Trie es un set, al ignorar acentos existen palabras que colapsan (como robo y robó)" << endl;
  return 0;
}
