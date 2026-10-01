/**
 * Descripcion: Declaracion de la clase Rle para la codificacion de corridas (Run-Length Encoding).
 * Autor:Sandra E Barajas Montiel
 */

#pragma once

#include <vector>
#include <utility>

class Rle {
public:
    /**
     * Aplica la codificacion RLE sobre un vector de enteros para compactar corridas repetidas.
     * @param datosMtf Vector de indices numericos (salida de MTF).
     * @return Vector de pares donde cada par representa (valor, frecuencia).
     */
    static std::vector<std::pair<int, int>> aplicarRle(const std::vector<int>& datosMtf);
};