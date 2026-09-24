/* Actividad Integradora 1 Utilidades compartidas de las sesiones
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#pragma once

#include <string>
#include <vector>

/// Lee las dos transmisiones y los tres códigos maliciosos del test -- O(|archivos|)
bool readTestData(const std::string&, std::vector<std::string>&, std::vector<std::string>&);

/// Lee los índices de un comando RESET y los acota al número de archivos
bool parseReset(const std::string&, const std::vector<int>&, std::vector<int>&);

/// Escribe el texto entre comillas como valor JSON, escapando CR y LF
void writeJsonText(const std::string&);
