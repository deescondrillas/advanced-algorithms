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
    /**
     * Decodifica RLE al descompactar los pares valor - frecuencia.
     * @param vector de pares valor - frecuencia.
     * @return vector de indices numericos correspondiente a la codificacion MTF.
     */
    static std::vector<int> invertirRle(const std::vector<std::pair<int, int>> par_valor_frecuencia);
};
