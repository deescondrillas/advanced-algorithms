#pragma once

#include <string>
#include <vector>

// Utilidades compartidas por las sesiones de visualización.

/// Lee las dos transmisiones y los tres códigos maliciosos del test -- O(|archivos|)
bool readTestData(const std::string&, std::vector<std::string>&, std::vector<std::string>&);

/// Lee los índices de un comando RESET y los acota al número de archivos
bool parseReset(const std::string&, const std::vector<int>&, std::vector<int>&);

/// Escribe el texto entre comillas como valor JSON, escapando CR y LF
void writeJsonText(const std::string&);
