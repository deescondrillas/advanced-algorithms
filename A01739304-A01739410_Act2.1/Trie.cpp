/* Actividad 2.1 Definición Trie
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#include "Trie.hpp"

TrieNode::TrieNode() {}

bool TrieNode::insert(const string& s, int idx) {
    if (idx == s.size())
        return this->is_end = true;
    if (children.count(s[idx]) == 0)
        children[s[idx]] = new TrieNode;
    return children[s[idx]]->insert(s, idx + 1);
}

TrieNode* TrieNode::walk(const string& s, int idx) {
    if (idx == s.size())
        return this;
    if (children.count(s[idx]) == 0)
        return NULL;
    return children[s[idx]]->walk(s, idx + 1);
}

bool TrieNode::membershipQuery(const string& s, int idx) {
    TrieNode* node = walk(s);
    return node ? node->is_end : false;
}

void TrieNode::produce(vector<string>& results, const string& pathChars) {
    if (this->is_end)
        results.push_back(pathChars);
    for (auto& kv : children) 
        kv.second->produce(results, pathChars + kv.first);
}

vector<string> TrieNode::complete(const string& prefix) {
    TrieNode* node = walk(prefix);
    if (node == NULL)
        return {};
    vector<string> results;
    node->produce(results, prefix);
    return results;
}
