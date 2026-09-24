/* Actividad Integradora 1 Programa de las sesiones de visualización
 *   - Octavio Hernández Loyo | A01739304
 *   - Franco De Escondrillas | A01739410
 * Fecha: 2026-09-24 */

#include <string>

#include "ManacherSession.hpp"
#include "SuffixArraySession.hpp"

using namespace std;

/// server.py lanza un proceso por modo, con la bandera y la carpeta del test
int main(int argc, char* argv[]) {
  string mode = argc > 1 ? argv[1] : "";
  string test = argc > 2 ? argv[2] : "tests/test1/";

  if (mode == "--manacher-session") return runManacherSession(test);
  if (mode == "--lcs-session") return runLcsSession(test);
  if (mode == "--find-session") return runFindSession(test);

  return 1;
}
