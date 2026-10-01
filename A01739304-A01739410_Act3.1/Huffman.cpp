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

/* Función Invertir Huffman se recorre el arbol bsucando coincidencias de los caracteres binarios con algun bloque que contenga 
   el caracter y la frecuencia que se retorna al Rle, debido a que la construcción de la tabla de los Codigos ya no es necesaria
   el recorrido de los caracteres binarios se convierte en O(n) una complejidad temporal lineal.
*/
vector<pair<int, int>> Huffman::invertirHuffman(const string& codigo, const map<string, int>& tablaCodigosEntrada) {
    vector<pair<int, int>> resultado;
    string codigoActual;

    for (char bit : codigo) {
        if (bit != '0' && bit != '1') {
            continue;
        }

        codigoActual += bit;
        map<string, int>::const_iterator simbolo = tablaCodigosEntrada.find(codigoActual);
        if (simbolo != tablaCodigosEntrada.end()) {
            int claveSimbolo = simbolo->second;
            int valor = claveSimbolo / 1000;
            int frecuencia = claveSimbolo % 1000;

            resultado.push_back(make_pair(valor, frecuencia));
            codigoActual.clear();
        }
    }

    return resultado;
}