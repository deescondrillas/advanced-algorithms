/**
 * Descripción: Programa principal para probar el funcionamiento del algoritmo de Bellman-Ford defectuoso.
 * Autor: Sandra E. Barajas Montiel
 */

#include <iostream>
#include <vector>
#include "GraphSolver.hpp"

using namespace std;

/**
 * Función principal que configura el grafo mediante entrada estándar y ejecuta el solver.
 * @return código de estado de salida del programa (0 si es exitoso).
 */
int main() {
    int totalVertices = 0;
    int totalAristas = 0;
    int verticeOrigen = 0;

    cin >> totalVertices >> totalAristas >> verticeOrigen;

    GraphSolver solverGrafo(totalVertices);

    for (int i = 0; i < totalAristas; i++) {
        int origen = 0;
        int destino = 0;
        int peso = 0;
        cin >> origen >> destino >> peso;
        solverGrafo.agregarArista(origen, destino, peso);
    }

    vector<int> resultadosDistancias;
    bool cicloNegativoDetectado = solverGrafo.ejecutarBellmanFord(verticeOrigen, resultadosDistancias);

    if (cicloNegativoDetectado) {
        cout << "Error: Se detectó un ciclo de peso negativo." << endl;
    } else {
        cout << "Distancias mínimas calculadas desde el origen:" << endl;
        for (size_t i = 0; i < resultadosDistancias.size(); i++) {
            cout << "Vértice " << i << " : " << resultadosDistancias[i] << endl;
        }
    }

    return 0;
}