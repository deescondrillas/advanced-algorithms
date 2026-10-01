/**
 * Descripcion: Declaracion de la clase Huffman para la construccion de arboles de codigos prefijo.
 * Autor:Sandra E Barajas Montiel
 */

#pragma once

#include <vector>
#include <utility>
#include <string>
#include <map>

class Huffman {
private:
    struct HuffmanNode {
        int valor;
        int frecuencia;
        HuffmanNode* izquierdo;
        HuffmanNode* derecho;

        HuffmanNode(int val, int freq);
        ~HuffmanNode();
    };

    struct CompararNodos {
        bool operator()(HuffmanNode* a, HuffmanNode* b);
    };

    static void generarCodigosHuffman(HuffmanNode* nodo, std::string codigoActual, std::map<int, std::string>& tablaCodigos);

public:
    /**
     * Aplica la codificacion de Huffman sobre los bloques RLE generando una cadena binaria y su tabla de codigos.
     * @param datosRle Vector de pares (valor, frecuencia).
     * @param tablaCodigosSalida Mapa de referencia donde se almacenaran los codigos de cada simbolo compuesto.
     * @return Cadena de texto formada por ceros y unos ('0', '1') que representa el flujo comprimido.
     */
    static std::string aplicarHuffman(const std::vector<std::pair<int, int>>& datosRle, std::map<int, std::string>& tablaCodigosSalida);
};