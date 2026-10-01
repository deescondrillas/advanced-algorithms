/**
 * Descripcion: Programa principal para ejecutar unicamente el pipeline de compresion (MTF -> RLE -> Huffman).
 * Autor: Sandra E Barajas Montiel
 * Ultima actualizacion: Franco De Escondrillas & Octavio Hernández -- 2026-10-01
 */

#include "Huffman.hpp"
#include "Mtf.hpp"
#include "Rle.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

/// Funcion para la lectura de telemetria -- O(1)
int parse(string&);

/// Funcion para la codificacion del mensaje -- O (max(n log₂n ))
map<int, string> encode(const string&, string&);

string decode(const string&, const map<int, string>&);

int main() {
  string originalText, compressedText;

  // Leer el input
  if (parse(originalText))
    return 1;

  // Codificar el mensaje
  map<int, string> encodingTable = encode(originalText, compressedText);

  // Decodificar el mensaje
  string decodedText = decode(compressedText, encodingTable);
  
  return 0;
}

int parse(string &originalText) {
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
  return 0;
}

map<int, string> encode(const string &originalText, string &compressedText) {
  // Paso 1: Move-to-Front
  vector<int> mtfOutput = Mtf::aplicarMtf(originalText);
  cout << "Paso 1 (MTF Aplicado): " << mtfOutput.size() << " elementos generados." << endl;

  // Paso 2: Run-Length Encoding
  vector<pair<int, int>> rleOutput = Rle::aplicarRle(mtfOutput);
  cout << "Paso 2 (RLE Aplicado): " << rleOutput.size() << " bloques de corridas compactados." << endl;

  // Paso 3: Huffman Codification
  map<int, string> huffmanCodeTable;
  compressedText = Huffman::aplicarHuffman(rleOutput, huffmanCodeTable);

  double finalByteSize = static_cast<double>(compressedText.size()) / 8.0;
  cout << "Paso 3 (Huffman Aplicado): " << compressedText.size() << " bits totales (" << finalByteSize << " bytes)." << endl;
  cout << "Pipeline de compresion ejecutado exitosamente." << endl;
  return huffmanCodeTable;
}

string decode(const string &compressedText, const map<int, string> &encodingTable) {
  map<string, int> decodingTable;
  for (pair<int, string> kv : encodingTable)
    decodingTable[kv.second] = kv.first;

  vector<pair<int, int>> rleOutput = Huffman::invertirHuffman(compressedText, decodingTable);
}
