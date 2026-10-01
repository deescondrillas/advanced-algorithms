/**
 * Descripcion: Implementacion de la clase Mtf para la transformacion Move-to-Front.
 * Autor:Sandra E Barajas Montiel
 */
#include "Mtf.hpp"

using namespace std;

vector<int> Mtf::aplicarMtf(const string& textoOriginal) {
    string alfabeto = "";
    for (int i = 0; i < 256; i++) {
        alfabeto.push_back(static_cast<char>(i));
    }

    vector<int> resultadoMtf;
    for (size_t i = 0; i < textoOriginal.size(); i++) {
        char caracterActual = textoOriginal[i];
        int indice = static_cast<int>(alfabeto.find(caracterActual));
        resultadoMtf.push_back(indice);
        alfabeto.erase(indice, 1);
        alfabeto = caracterActual + alfabeto;
    }

    return resultadoMtf;
}

string Mtf::invertirMtf(const std::vector<int>& alfabeto) {
  return "";
}
