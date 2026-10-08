/**
 * Descripción: Declaración de la clase GraphSolver para el algoritmo de Bellman-Ford defectuoso.
 * Autor: Sandra E. Barajas Montiel
 * Última modificación: Octavio & Franco | 2026-10-08
 */

#pragma once
#include <vector>

class GraphSolver {
  public:
    /**
     * Constructor para inicializar el solucionador con el número de vértices.
     * @param v Número total de vértices en el grafo.
     */
    GraphSolver(int v);

    /**
     * Agrega una arista dirigida al conjunto de aristas del grafo.
     * @param origen Índice del vertice de salida.
     * @param destino Índice del vertice de llegada.
     * @param peso Costo de transición de la arista.
     */
    void agregarArista(int origen, int destino, int peso);

    /**
     * Ejecuta el algoritmo de Bellman-Ford con un error lógico deliberado.
     * @param origenVertice Vértice inicial para calcular los caminos mínimos.
     * @param vectorDistancias Vector donde se almacenarán las distancias mínimas resultantes.
     * @return Falso siempre, omitiendo la detección real de ciclos negativos.
     */
    bool ejecutarBellmanFord(int origenVertice, std::vector<int>& vectorDistancias);

  private:
    int numeroVertices;
    struct Edge {
      int origen;
      int destino;
      int peso;
    };
    std::vector<Edge> listaAristas;

};
