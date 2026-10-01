/**
 * Descripcion: Programa principal para ejecutar unicamente el pipeline de compresion (MTF -> RLE -> Huffman).
 * Autor:Sandra E Barajas Montiel
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "Mtf.hpp"
#include "Rle.hpp"
#include "Huffman.hpp"

using namespace std;

/**
 * Funcion principal que lee el archivo de telemetria y ejecuta el pipeline de compresion.
 * @return Codigo de estado del sistema (0 si es exitoso).
 */
int main() {
    ifstream archivoEntrada("Transmision.txt");
    if (!archivoEntrada.is_open()) {
        cerr << "Error: No se pudo abrir el archivo Transmision.txt" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << archivoEntrada.rdbuf();
    string textoOriginal = buffer.str();
    archivoEntrada.close();

    cout << "--- INFORME DE PIPELINE DE COMPRESION ---" << endl;
    cout << "Tamano original: " << textoOriginal.size() << " bytes (" << textoOriginal.size() * 8 << " bits)" << endl;

    // Paso 1: Move-to-Front
    vector<int> salidaMtf = Mtf::aplicarMtf(textoOriginal);
    cout << "Paso 1 (MTF Aplicado): " << salidaMtf.size() << " elementos generados." << endl;

    // Paso 2: Run-Length Encoding
    vector<pair<int, int>> salidaRle = Rle::aplicarRle(salidaMtf);
    cout << "Paso 2 (RLE Aplicado): " << salidaRle.size() << " bloques de corridas compactados." << endl;

    // Paso 3: Huffman Codification
    map<int, string> tablaCodigosHuffman;
    string salidaHuffmanBits = Huffman::aplicarHuffman(salidaRle, tablaCodigosHuffman);

    double tamanoFinalBytes = static_cast<double>(salidaHuffmanBits.size()) / 8.0;
    cout << "Paso 3 (Huffman Aplicado): " << salidaHuffmanBits.size() << " bits totales (" << tamanoFinalBytes << " bytes)." << endl;
    cout << "Pipeline de compresion ejecutado exitosamente." << endl;

    return 0;
}