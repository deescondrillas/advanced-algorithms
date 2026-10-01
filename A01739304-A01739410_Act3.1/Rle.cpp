/**
 * Descripcion: Implementacion de la clase Rle para la compactacion de corridas consecutivas.
 * Autor:Sandra E Barajas Montiel
 */

#include "Rle.hpp"

using namespace std;

vector<pair<int, int>> Rle::aplicarRle(const vector<int>& datosMtf) {
    vector<pair<int, int>> resultadoRle;
    if (datosMtf.empty()) {
        return resultadoRle;
    }

    int valorActual = datosMtf[0];
    int contador = 1;

    for (size_t i = 1; i < datosMtf.size(); i++) {
        if (datosMtf[i] == valorActual) {
            contador++;
        } else {
            resultadoRle.push_back(make_pair(valorActual, contador));
            valorActual = datosMtf[i];
            contador = 1;
        }
    }
    resultadoRle.push_back(make_pair(valorActual, contador));

    return resultadoRle;
}