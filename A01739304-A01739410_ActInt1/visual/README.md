# Visualización de Manacher

Ejecuta **IniciarVisualizacion.cmd** dentro de la carpeta **visual** y abre
http://127.0.0.1:8080. Requiere g++ y Python 3.10 o posterior. No instala
dependencias ni requiere internet. Mantén la consola abierta; Ctrl+C termina
el servidor. Una pestaña controla una sesión.

## Entrada

La página inicia vacía. **Abrir .txt** activa un input de archivo oculto.
FileReader.readAsText lee UTF-8 y envía el contenido sin trim ni eliminación
de CR/LF. **Cargar texto** permite enviar el campo manualmente. Se mantienen
los límites de 10,000 caracteres y el alfabeto 0–9, A–F, CR y LF de la actividad.

Los archivos conservan CR y LF; se dibujan como ␍ y ↵. El textarea del navegador
puede normalizar CRLF al editar o volver a cargar manualmente su contenido.
**Reiniciar** utiliza el string original recibido, conservando CR y LF.

## Integración

La función global **updateState(data)** recibe exclusivamente el estado
calculado en C++. JavaScript dibuja los caracteres y radios; no implementa
Manacher. El objeto **manacherTransport** concentra las operaciones reset(text)
y next(). Actualmente solicita HTTP al servidor Python, que intercambia
comandos y JSON con la entrada/salida estándar del programa C++.

Al cargar, el mensaje incluye:
- text: cadena original, sin separadores.
- i, center, right, mirror, matches: estado del algoritmo, índices desde 0.
- radius: radios de la ventana; null indica una posición no procesada.
- offset: índice inicial de la ventana; size: longitud transformada total.
- phase, step, comparisons, finished: estado de ejecución.
- start, end, length: resultado inclusivo desde 1; length = 0 indica ausencia.

En los mensajes siguientes puede omitirse text. C++ lo transmite solo una vez
para no copiar la transmisión entera en cada paso. Si se conecta un WebSocket,
su receptor puede llamar a updateState(JSON.parse(event.data)); el servidor
debe respetar este mismo contrato. No se añadió un servidor WebSocket.

Manacher.cpp conserva el for/while y los índices desde 0 elegidos por el equipo.
El constructor Manacher(texto) funciona como antes. Su segundo argumento
opcional es un observador: recibe cada operación del mismo algoritmo,
sin duplicar su implementación ni introducir otro recorrido por estados.

Los archivos específicos de visualización están en esta carpeta:
- ManacherState.hpp: datos observables de una operación.
- ManacherSession.hpp/.cpp: serialización y control por entrada/salida estándar.
- IniciarVisualizacion.cmd: compilación y arranque desde cualquier directorio.
- server.py, index.html, app.js y style.css: conexión e interfaz.

ManacherSession publica el estado y espera NEXT dentro del observador.
Solo entonces el for/while de C++ continúa hasta la siguiente operación.
RESET interrumpe y destruye la sesión anterior, y construye una nueva.
No se guardan pasos precalculados ni se ejecuta una segunda versión de Manacher.

El resultado interno se convierte a posiciones desde 1 exclusivamente en
ManacherSession. El campo length distingue un carácter en el índice 0 de la
ausencia de resultados. main conserva parse, part1, part2 y part3; solo atiende
la interfaz al recibir --manacher-session.

El arreglo usa # para representar el separador interno. Los punteros y
comparaciones usan intensidades de gris; el resto de la página es monocromo.
Para textos largos se devuelve una ventana de 31 radios. Reproducir ejecuta
los mismos pasos que Siguiente paso; la velocidad visual no mide rendimiento.

## Pruebas

Desde la carpeta de la actividad, después de compilar con el lanzador:

~~~text
g++ -std=c++11 -O2 -static -Wall -Wextra -Werror -pedantic visual/test_manacher.cpp Manacher.cpp -o visual/.build/test_manacher.exe
visual/.build/test_manacher.exe
python visual/test_server.py
~~~

Las pruebas del algoritmo utilizan índices desde 0; las de HTTP comprueban
la conversión a base 1, la cadena recibida desde C++, archivos de la interfaz,
reinicios, texto vacío, límites y validación.

## Revisión tras el pull

Se corrigió la construcción del texto transformado: current + DELIMITER
sumaba dos valores char y añadía un solo carácter. Ahora el carácter y el
separador se añaden por separado. El resto del enfoque del equipo permanece:
simetría, expansión, primer resultado en empates y posiciones internas desde 0.

Se contrastaron 9,841 cadenas exhaustivas, 500 aleatorias y cada radio contra
expansión directa. También se revisaron texto vacío, un carácter, pares,
impares, empates, CR/LF, NUL y una cadena de un millón de caracteres.
Las pruebas de conexión incluyen reiniciar durante una comparación y avanzar
después de terminar.
