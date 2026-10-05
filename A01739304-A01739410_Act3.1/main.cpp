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

/// Funcion para la codificacion del mensaje -- O(max(n log₂n, n · |Σ|))
map<int, string> encode(const string&, string&);

/// Funcion para la decodificacion del mensaje -- O(n · |Σ|)
string decode(const string&, const map<int, string>&);

int main() {
  string originalText, compressedText;

  // Leer el input (abortar si el archivo no existe)
  if (parse(originalText))
    return 1;

  // Codificar el mensaje
  map<int, string> encodingTable = encode(originalText, compressedText);

  // Decodificar el mensaje
  string decodedText = "";//decode(compressedText, encodingTable);

  // Comparar mensajes
  cout << "Se recuperaron " << decodedText.size() << " / " << originalText.size() << "bytes" << endl;
  cout << "El mensaje decodificado es" << (originalText == decodedText ? "identico" : "diferente") << endl;

  // Imprimir mensajes
  cout << "\nMensaje original:\n" << originalText << endl;
  cout << "\nMensaje decodificado:\n" << decodedText << endl;
  
  return 0;
}

/// Funcion para la lectura de telemetria -- O(1)
int parse(string &originalText) {
  ifstream inputFile("Transmision.txt");
  if (!inputFile.is_open()) {
    cerr << "Error: No se pudo abrir el archivo Transmision.txt" << endl;
    return 1;
  }

  stringstream buffer;
  buffer << inputFile.rdbuf();
  originalText = buffer.str();
  inputFile.close();

  cout << "--- INFORME DE PIPELINE DE COMPRESION ---" << endl;
  cout << "Tamano original: " << originalText.size() << " bytes (" << originalText.size() * 8 << " bits)" << endl;
  return 0;
}

/// Funcion para la codificacion del mensaje -- O(max(n log₂n, n · |Σ|))
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

/// Funcion para la decodificacion del mensaje -- O(n · |Σ|)
string decode(const string &compressedText, const map<int, string> &encodingTable) {
  map<string, int> decodingTable;
  for (pair<int, string> kv : encodingTable)
    decodingTable[kv.second] = kv.first;

  // Paso 1: Invertir Huffman Codification
  vector<pair<int, int>> rleOutput = Huffman::invertirHuffman(compressedText, decodingTable);
  cout << "Paso 1 (Invertir Huffman): " << rleOutput.size() << " / " << "" << " bloques recuperados";

  // Paso 2: Invertir Run-Length Encoding
  vector<int> mtfOutput = Rle::invertirRle(rleOutput);
  cout << "Paso 2 (Invertir RLE): " << mtfOutput.size() << " / " << "" << " elementos recuperados";

  // Paso 3: Invertir Move-to-Front
  string originalText = Mtf::invertirMtf(mtfOutput);
  cout << "Paso 3 (Invertir MTF): Se recupero un mensaje de " << originalText.size() << " bytes";
  return originalText;
}
