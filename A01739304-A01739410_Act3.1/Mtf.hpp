/**
 * Descripcion: Declaracion de la clase Mtf para la transformacion Move-to-Front.
 * Autor: Sandra E Barajas Montiel
 * Ultima actualizacion: Franco De Escondrillas & Octavio Hernández -- 2026-10-04
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
    static std::vector<int> aplicarMtf(const std::string& originalText, std::string& alphabet);
    /**
     * Decodifica un mensaje MTF aplicando la transformación inversa Move-to-Front
     * @param mensajeCodificado Vector de enteros que se obtiene al aplicar MTF.
     * @param alfabeto String con los caracteres del alfabeto. Es crucial incluirlo en los metadatos del mensaje.
     * @return String con el mensaje original.
     */
    static std::string invertirMtf(const std::vector<int>& encodedMessage, std::string& alphabet);
};
