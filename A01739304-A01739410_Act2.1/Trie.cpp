/* Actividad 2.1 Definición Trie
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#include "Trie.hpp"

TrieNode::TrieNode() {}

void TrieNode::transform(string& s) {
  for (int i = 0; i < s.size(); ++i) {
    // Detectar caracteres especiales (primer byte = 195)
    if (s[i] == char(195) and i + 1 < s.size() and dictionary.count(s[i + 1])) {
      s[i] = dictionary[s[i + 1]];
      s.erase(i + 1, 1);
    }
    s[i] = tolower(s[i]);
  }
}

// Insertar patrón en el Trie --O(|p|)
bool TrieNode::insert(const string& s, int idx) {
  if (idx == s.size())
    return is_end = true;
  if (not children.count(s[idx]))
    children[s[idx]] = new TrieNode;
  return children[s[idx]]->insert(s, idx + 1);
}

// Eliminar patrón del Trie --O(|p|)
bool TrieNode::remove(const string&s, int idx) {
  if (idx == s.size()) {
    is_end = false;
    return children.empty();
  }
  if (not children.count(s[idx]))
    return false;
  if (children[s[idx]]->remove(s, idx + 1)) {
    delete children[s[idx]];
    children.erase(s[idx]);
  }
  return children.empty() and not is_end;
}

// Seguir un patrón a través del Trie --O(|p|)
TrieNode* TrieNode::walk(const string& s, int idx) {
  if (idx == s.size())
    return this;
  if (not children.count(s[idx]))
    return nullptr;
  return children[s[idx]]->walk(s, idx + 1);
}

// Revisar si un string es parte del Trie --O(|p|)
bool TrieNode::membershipQuery(const string& s, int idx) {
  TrieNode* node = walk(s);
  return node ? node->is_end : false;
}

// Recuperar todos los strings miembros del Trie --O(|n|)
void TrieNode::produce(vector<string>& results, const string& pathChars) {
  if (is_end)
    results.push_back(pathChars);
  for (auto& kv : children) 
    kv.second->produce(results, pathChars + kv.first);
}

// Recuperar todos los string miembros de un Subtrie --O(|n|)
vector<string> TrieNode::complete(const string& prefix) {
  TrieNode* node = walk(prefix);
  if (not node)
    return {};
  vector<string> results;
  node->produce(results, prefix);
  return results;
}
