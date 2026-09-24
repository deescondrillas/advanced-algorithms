# Visualización de los algoritmos

Ejecuta **IniciarVisualizacion.cmd** dentro de la carpeta **visual** y abre
http://127.0.0.1:8080. Requiere g++ y Python 3.10 o posterior. No instala
dependencias ni requiere internet. Mantén la consola abierta; Ctrl+C termina
el servidor. Una pestaña controla una sesión.

Hay tres páginas, enlazadas entre sí desde la cabecera:

| Página | Algoritmo | Qué se anima |
|---|---|---|
| `/` | `Manacher` | los radios y la simetría, carácter por carácter |
| `/find.html` | `SuffixArray::find` | el patrón comparándose contra el sufijo actual |
| `/lcs.html` | `SuffixArray::lcs` | la ventana deslizante sobre sufijos vecinos |

Ordenar los sufijos con SA-IS y construir el LCP quedan fuera del alcance de la
visualización: ambas páginas los reciben ya calculados en el primer mensaje.

## Entrada

La página inicia vacía. **Abrir .txt** activa un input de archivo oculto.
FileReader.readAsText lee UTF-8 y envía el contenido sin trim ni eliminación
de CR/LF. El botón de carga permite enviar los campos manualmente. Se mantienen
los límites de 10,000 caracteres y el alfabeto 0–9, A–F, CR y LF de la actividad.
Manacher admite texto vacío; lcs y find necesitan al menos un carácter en cada
campo, porque sin sufijos no hay nada que recorrer.

Los archivos conservan CR y LF; se dibujan como ␍ y ↵. El textarea del navegador
puede normalizar CRLF al editar o volver a cargar manualmente su contenido.
**Reiniciar** utiliza los strings originales recibidos, conservando CR y LF.

## Integración

Cada página define la función global **updateState(data)**, que recibe
exclusivamente el estado calculado en C++. JavaScript dibuja los caracteres,
los radios y la rejilla; no implementa ninguno de los tres algoritmos. El objeto
**window.algorithmTransport** concentra las operaciones reset(campos) y next().
Actualmente solicita HTTP al servidor Python, que intercambia comandos y JSON
con la entrada/salida estándar del programa C++. Si se conecta un WebSocket, su
receptor puede llamar a updateState(JSON.parse(event.data)); el servidor debe
respetar este mismo contrato. No se añadió un servidor WebSocket.

Al cargar, el mensaje incluye las cadenas de la sesión; en los mensajes
siguientes pueden omitirse. C++ las transmite solo una vez para no copiar la
transmisión entera en cada paso.

- Manacher: `text`, más `i`, `center`, `right`, `mirror`, `matches`, `radius`.
- find: `text`, `pattern`, más `column`, `start`, `matches`, `lcpHere`,
  `comparePattern`, `compareText`, `comparison`, `comparisons`, `firstMatch`.
- lcs: `textA`, `textB`, más `i`, `lcpHere`, `idxBest`, `distinct`, `improved`,
  `length`, `startA`, `endA`, `startB`, `endB`.

Las dos páginas del suffix array reciben además `sa`, `lcp` y, en lcs, `origin`
(1 textA, 2 textB, 0 delimitador), recortados a la ventana que rodea la columna
activa; `offset` es su índice inicial y `size` el total de sufijos. Todas las
posiciones del resultado llegan desde 1 y son inclusivas; `length` o
`firstMatch` en 0 indican ausencia de resultado.

Manacher.cpp y SuffixArray.cpp conservan el for/while y los índices desde 0
elegidos por el equipo. `Manacher(texto)`, `find(patrón)` y `lcs()` funcionan
como antes: el observador es un argumento opcional que recibe cada operación
del mismo algoritmo, sin duplicar su implementación ni introducir otro
recorrido por estados.

Los archivos específicos de visualización están en esta carpeta:
- ManacherState.hpp y SuffixArrayState.hpp: datos observables de una operación.
- ManacherSession y SuffixArraySession: serialización y control por E/S estándar.
- SessionText: decodificación hexadecimal y escritura de strings JSON.
- IniciarVisualizacion.cmd: compilación y arranque desde cualquier directorio.
- server.py: archivos estáticos y un proceso C++ por modo, creado al usarse.
- index.html, lcs.html, find.html: una página por algoritmo.
- session.js: transporte, pasos, reproducción, errores y lectura de archivos.
- grid.js: rejilla de sufijos compartida por lcs y find.
- app.js, lcs.js, find.js: el dibujo propio de cada algoritmo.
- style.css: estilo común a las tres páginas.

Las sesiones publican el estado y esperan NEXT dentro del observador. Solo
entonces el for/while de C++ continúa hasta la siguiente operación. RESET
interrumpe y destruye la sesión anterior, y construye una nueva. Manacher usa
`RESET <hex>`; lcs y find usan `RESET <hexA> <hexB>`. No se guardan pasos
precalculados ni se ejecuta una segunda versión de los algoritmos.

El resultado interno se convierte a posiciones desde 1 exclusivamente en las
sesiones. main conserva parse, part1, part2 y part3; solo atiende la interfaz
al recibir `--manacher-session`, `--lcs-session` o `--find-session`.

El arreglo de Manacher usa # para representar el separador interno y devuelve
una ventana de 31 radios. La rejilla de sufijos devuelve 25 columnas y recorta
cada sufijo a 12 caracteres, marcando … cuando continúa; el delimitador entre
los dos textos se dibuja como $ y cierra el sufijo, igual que hace el LCP.
Los punteros y comparaciones usan intensidades de gris; el resto es monocromo.
Reproducir ejecuta los mismos pasos que Siguiente paso; la velocidad visual no
mide rendimiento.

## Pruebas

Desde la carpeta de la actividad, después de compilar con el lanzador:

~~~text
g++ -std=c++11 -O2 -static -Wall -Wextra -Werror -pedantic visual/test_manacher.cpp Manacher.cpp -o visual/.build/test_manacher.exe
visual/.build/test_manacher.exe
g++ -std=c++11 -O2 -static -Wall -Wextra -Werror -pedantic -Wno-sign-compare visual/test_suffix_array.cpp SuffixArray.cpp SaIs.cpp -o visual/.build/test_suffix.exe
visual/.build/test_suffix.exe
python visual/test_server.py
~~~

Las pruebas del algoritmo utilizan índices desde 0; las de HTTP comprueban
la conversión a base 1, las cadenas recibidas desde C++, archivos de la interfaz,
reinicios, texto vacío, ventanas, límites y validación.

test_suffix_array.cpp contrasta 14,941 casos: todos los pares de cadenas de
hasta cuatro caracteres sobre un alfabeto de tres, 300 pares aleatorios y los
casos de la actividad. Comprueba que `lcs()` y `find()` devuelven exactamente lo
mismo con y sin observador, que la longitud coincide con fuerza bruta y con
`std::string::find`, que los dos rangos del lcs contienen el mismo substring, y
que cada comparación publicada corresponde a los caracteres reales.

## Revisión tras el pull

Se corrigió de nuevo la construcción del texto transformado de Manacher:
`palindromeText += current + DELIMITER` suma dos valores char y añade un solo
carácter, así que el arreglo perdía los separadores y solo encontraba
palíndromos impares. El carácter y el separador vuelven a añadirse por separado.
El resto del enfoque del equipo permanece: simetría, expansión, primer resultado
en empates y posiciones internas desde 0.

`lcs()` devolvía `sa[idxBest - 1]` con `idxBest` en 0 cuando los dos textos no
comparten ningún substring, leyendo fuera del arreglo. La traducción del
resultado se extrajo a `lcsResult`, que en ese caso devuelve un rango vacío;
la interfaz lo muestra como longitud 0.
