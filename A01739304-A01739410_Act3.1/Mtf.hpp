/**
 * Descripcion: Declaracion de la clase Mtf para la transformacion Move-to-Front.
 * Autor:Sandra E Barajas Montiel
 */

#pragma once

#include <vector>
#include <string>

class Mtf {
public:
    /**
     * Aplica la transformacion Move-to-Front sobre una cadena de texto para agrupar simbolos locales.
     * @param textoOriginal Cadena de entrada a transformar.
     * @return Vector de enteros con los indices correspondientes del alfabeto dinamico.
     */
    static std::vector<int> aplicarMtf(const std::string& textoOriginal);
    /**
     * …
     * @param …
     * @return …
     */
    static std::string invertirMtf(const std::vector<int>& alfabeto);
};
