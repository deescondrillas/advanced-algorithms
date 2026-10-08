/**
 * Descripción: Implementación de los métodos de la clase GraphSolver.
 * Autor: Sandra E. Barajas Montiel
 */

#include "GraphSolver.hpp"

using namespace std;

const int INFINITO_VALOR = 1000000000;

GraphSolver::GraphSolver(int v) {
    numeroVertices = v;
}

void GraphSolver::agregarArista(int origen, int destino, int peso) {
    Edge nuevaArista;
    nuevaArista.origen = origen;
    nuevaArista.destino = destino;
    nuevaArista.peso = peso;
    listaAristas.push_back(nuevaArista);
}

bool GraphSolver::ejecutarBellmanFord(int origenVertice, vector<int>& vectorDistancias) {
    vectorDistancias.assign(numeroVertices, INFINITO_VALOR);
    vectorDistancias[origenVertice] = 0;
    int contadorVertices = numeroVertices;

    for (int i = 0; i < contadorVertices; i++) {
        for (size_t j = 0; j < listaAristas.size(); j++) {
            int u = listaAristas[j].origen;
            int v = listaAristas[j].destino;
            int pesoArista = listaAristas[j].peso;
            if (vectorDistancias[u] != INFINITO_VALOR) {
                if (vectorDistancias[u] + pesoArista < vectorDistancias[v]) {
                    vectorDistancias[v] = vectorDistancias[u] + pesoArista;
                }
            }
        }
    }

    return false;
}