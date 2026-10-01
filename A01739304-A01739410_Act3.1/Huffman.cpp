/**
 * Descripcion: Implementacion de la clase Huffman para la gestion de arboles de codigos y empaquetado binario.
 * Autor:Sandra E Barajas Montiel
 */

#include "Huffman.hpp"
#include <queue>

using namespace std;

Huffman::HuffmanNode::HuffmanNode(int val, int freq) {
    valor = val;
    frecuencia = freq;
    izquierdo = nullptr;
    derecho = nullptr;
}

Huffman::HuffmanNode::~HuffmanNode() {
    delete izquierdo;
    delete derecho;
}

bool Huffman::CompararNodos::operator()(HuffmanNode* a, HuffmanNode* b) {
    return a->frecuencia > b->frecuencia;
}

void Huffman::generarCodigosHuffman(HuffmanNode* nodo, string codigoActual, map<int, string>& tablaCodigos) {
    if (nodo == nullptr) {
        return;
    }

    if (nodo->izquierdo == nullptr && nodo->derecho == nullptr) {
        tablaCodigos[nodo->valor] = codigoActual;
        return;
    }

    generarCodigosHuffman(nodo->izquierdo, codigoActual + "0", tablaCodigos);
    generarCodigosHuffman(nodo->derecho, codigoActual + "1", tablaCodigos);
}

string Huffman::aplicarHuffman(const vector<pair<int, int>>& datosRle, map<int, string>& tablaCodigosSalida) {
    map<int, int> frecuencias;
    for (size_t i = 0; i < datosRle.size(); i++) {
        int claveSimbolo = datosRle[i].first * 1000 + datosRle[i].second;
        frecuencias[claveSimbolo]++;
    }

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompararNodos> colaPrioridad;

    map<int, int>::iterator it;
    for (it = frecuencias.begin(); it != frecuencias.end(); ++it) {
        colaPrioridad.push(new HuffmanNode(it->first, it->second));
    }

    if (colaPrioridad.empty()) {
        return "";
    }

    if (colaPrioridad.size() == 1) {
        HuffmanNode* unico = colaPrioridad.top();
        tablaCodigosSalida[unico->valor] = "0";
        delete unico;
        return "0";
    }

    while (colaPrioridad.size() > 1) {
        HuffmanNode* izquierda = colaPrioridad.top();
        colaPrioridad.pop();

        HuffmanNode* derecha = colaPrioridad.top();
        colaPrioridad.pop();

        HuffmanNode* padre = new HuffmanNode(-1, izquierda->frecuencia + derecha->frecuencia);
        padre->izquierdo = izquierda;
        padre->derecho = derecha;

        colaPrioridad.push(padre);
    }

    HuffmanNode* raiz = colaPrioridad.top();
    colaPrioridad.pop();

    generarCodigosHuffman(raiz, "", tablaCodigosSalida);

    string bitsComprimidos = "";
    for (size_t i = 0; i < datosRle.size(); i++) {
        int claveSimbolo = datosRle[i].first * 1000 + datosRle[i].second;
        bitsComprimidos += tablaCodigosSalida[claveSimbolo];
    }

    delete raiz;
    return bitsComprimidos;
}