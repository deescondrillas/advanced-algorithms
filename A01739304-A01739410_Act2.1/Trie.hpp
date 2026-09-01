/* Actividad 2.1 Definición Trie
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class TrieNode {
  public:
    TrieNode();
  
    // Transforma patrón a minúsculas sin acentos --O(|p|)
    void transform(string& s);
    
    // Insertar patrón en el Trie --O(|p|)
    bool insert(const string& s, int idx = 0);
  
    // Eliminar patrón del Trie --O(|p|)
    bool remove(const string& s, int idx = 0);
  
    // Seguir un patrón a través del Trie --O(|p|)
    TrieNode* walk(const string& s, int idx = 0);
  
    // Revisar si un string es parte del Trie --O(|p|)
    bool membershipQuery(const string& s, int idx = 0);
  
    // Recuperar todos los strings miembros del Trie --O(|n|)
    void produce(vector<string>& results, const string& pathChars = "");
  
    // Recuperar todos los string miembros de un Subtrie --O(|n|)
    vector<string> complete(const string& prefix);

  private:
    unordered_map<char, TrieNode*> children;
    // ASCII de los acentos (segundo byte)
    unordered_map<char, char> dictionary {
      {char(161), 'a'}, // á --> a
      {char(169), 'e'}, // é --> e
      {char(173), 'i'}, // í --> i
      {char(177), '~'}, // ñ --> ~
      {char(179), 'o'}, // ó --> o
      {char(186), 'u'}, // ú --> u
      {char(188), 'u'}  // ü --> u
    };
    bool is_end = false;
};
