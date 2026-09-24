#pragma once

#include <string>

// Utilidades de texto compartidas por las sesiones de visualización.

/// Decodifica el hexadecimal de un comando RESET; valida longitud y alfabeto
bool decodeText(const std::string&, std::string&);

/// Escribe el texto entre comillas como valor JSON, escapando CR y LF
void writeJsonText(const std::string&);
