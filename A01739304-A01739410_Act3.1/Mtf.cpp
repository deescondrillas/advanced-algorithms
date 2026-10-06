/**
 * Descripcion: Implementacion de la clase Mtf para la transformacion Move-to-Front.
 * Autor: Sandra E Barajas Montiel
 * Ultima actualizacion: Franco De Escondrillas & Octavio Hernández -- 2026-10-04
 */
#include "Mtf.hpp"

using namespace std;

vector<int> Mtf::aplicarMtf(const string& originalText, string& alphabet) {
    alphabet = "";
    vector<int> answerMtf;
    for (int i = 0; i < 256; i++)
      alphabet.push_back(static_cast<char>(i));

    for (size_t i = 0; i < originalText.size(); i++) {
      int idx = static_cast<int>(alphabet.find(originalText[i]));
      answerMtf.push_back(idx);
      alphabet.erase(idx, 1);
      alphabet = originalText[i] + alphabet;
    }

    return answerMtf;
}

string Mtf::invertirMtf(const std::vector<int>& encodedMessage, string& alphabet) {
  string originalText;
  
  for (int i = encodedMessage.size() - 1; i >= 0; --i) {
    originalText += alphabet[0];
    alphabet.erase(alphabet.begin());
    alphabet.insert(alphabet.begin() + encodedMessage[i], originalText.back());
  }
  
  reverse(originalText.begin(), originalText.end());
  return originalText;
}
