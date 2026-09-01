/* Actividad 2.1 Definición Trie
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-08-31 */

#pragma once

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class TrieNode {
public:
    TrieNode();

    // O(|p|)
    bool insert(const string& s, int idx = 0);

    // O(|p|)
    TrieNode* walk(const string& s, int idx = 0);

    // O(|p|)
    bool membershipQuery(const string& s, int idx = 0);

    // O(|n|)
    void produce(vector<string>& results, const string& pathChars = "");

    // O(|n|)
    vector<string> complete(const string& prefix);

    private:
        unordered_map<char, TrieNode*> children;
        bool is_end = false;
};
