/**
 * Descripción: Implementación de los métodos de la clase GraphSolver.
 * Autor: Sandra E. Barajas Montiel
 * Última modificación: Octavio & Franco | 2026-10-08
 */

#include "GraphSolver.hpp"

using namespace std;

const int INF = 2e9;

GraphSolver::GraphSolver(int v) {
  numeroVertices = v;
}

void GraphSolver::agregarArista(int origen, int destino, int peso) {
  Edge nuevaArista(origen, destino, peso);
  listaAristas.push_back(nuevaArista);
}

bool GraphSolver::ejecutarBellmanFord(int origenVertice, vector<int>& vectorDistancias) {
  vectorDistancias.assign(numeroVertices, INF);
  vectorDistancias[origenVertice] = 0;

  for (int i = 0; i < numeroVertices - 1; ++i)
    for (Edge arista : listaAristas)
      if (vectorDistancias[arista.origen] != INF)
        if (vectorDistancias[arista.origen] + arista.peso < vectorDistancias[arista.destino])
          vectorDistancias[arista.destino] = vectorDistancias[arista.origen] + arista.peso;

  for (Edge e : listaAristas) 
    if (vectorDistancias[e.origen] + e.peso < vectorDistancias[e.destino])
      return true;
    
  return false;
}
