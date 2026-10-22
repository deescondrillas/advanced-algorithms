# Fuentes de los fragmentos literarios

Fecha de descarga: 21 de octubre de 2026.

Las tres obras se descargaron en texto plano desde Project Gutenberg, en sus
idiomas originales. Sus fichas las identifican como de dominio público en
Estados Unidos. Se conserva aquí la atribución y el enlace a cada edición.
La licencia del proveedor puede consultarse en https://www.gutenberg.org/policy/license.html.

## Procedimiento de extracción

1. Decodificar el texto fuente como UTF-8 y normalizar CRLF y CR a LF.
2. Localizar la primera frase de la narración, excluyendo el prólogo editorial,
   la tabla de contenido y la información del proveedor anterior al relato.
3. Conservar un fragmento continuo desde esa frase hasta el límite de párrafo
   más cercano a 20 000 bytes. No se duplicaron párrafos ni se añadió relleno.
4. Guardar el fragmento como UTF-8 sin BOM, terminado con un salto de línea.

El tamaño se mide después de codificar en UTF-8: veinte mil caracteres no
necesariamente equivalen a veinte mil bytes. Se aceptó una diferencia inferior
a mil bytes para terminar en un párrafo completo. Los saltos y marcas editoriales
que aparecen dentro del fragmento se conservaron tal como están en la fuente
normalizada. Las obras completas descargadas no se incluyen en la entrega.

## Don Quijote

- Autor: Miguel de Cervantes Saavedra.
- Idioma: espanol.
- Archivo: `espanol/10_don_quijote.txt`.
- Tamaño del fragmento: 19538 bytes.
- Ficha y atribución: [Don Quijote](https://www.gutenberg.org/ebooks/2000).
- [Texto fuente completo](https://www.gutenberg.org/cache/epub/2000/pg2000.txt).
- Inicio del fragmento: «En un lugar de la Mancha, de cuyo nombre no quiero acordarme, no ha mucho».
- Intervalo de caracteres en la fuente normalizada: [39998, 59104); índices desde cero. Se retiran blancos finales y se agrega un LF.
- SHA-256 del archivo fuente descargado: `534f41d59f7142163fa0964076ac6351845c006ea6433778be637fac6d5b04d7`.
- SHA-256 del fragmento entregado: `59b5433e342a12fa121d5ff28586ad0d7be19acc715679c1eb27d1410a80404f`.

## Candide, ou l’optimisme

- Autor: Voltaire.
- Idioma: frances.
- Archivo: `frances/10_candide.txt`.
- Tamaño del fragmento: 20654 bytes.
- Ficha y atribución: [Candide, ou l’optimisme](https://www.gutenberg.org/ebooks/4650).
- [Texto fuente completo](https://www.gutenberg.org/cache/epub/4650/pg4650.txt).
- Inicio del fragmento: «Il y avait en Vestphalie, dans le château de M. le baron de».
- Intervalo de caracteres en la fuente normalizada: [7639, 27388); índices desde cero. Se retiran blancos finales y se agrega un LF.
- SHA-256 del archivo fuente descargado: `1cc10ab3268a76aa01bd3f9722ae2ae0e5e36b869a18b0bb0832b75bc0327806`.
- SHA-256 del fragmento entregado: `b337c4f47a9f864f1d5d56daa5e12c5c0f7a560176329457cc38a97cfaddce49`.

## Pride and Prejudice

- Autor: Jane Austen.
- Idioma: ingles.
- Archivo: `ingles/10_pride_and_prejudice.txt`.
- Tamaño del fragmento: 19964 bytes.
- Ficha y atribución: [Pride and Prejudice](https://www.gutenberg.org/ebooks/1342).
- [Texto fuente completo](https://www.gutenberg.org/cache/epub/1342/pg1342.txt).
- Inicio del fragmento: «It is a truth universally acknowledged, that a single man in possession».
- Intervalo de caracteres en la fuente normalizada: [35259, 54822); índices desde cero. Se retiran blancos finales y se agrega un LF.
- SHA-256 del archivo fuente descargado: `3f6bb9d6f78e0293b56acd4714dd68cb7d6d1d293402031ce9d5a216bcaf9d75`.
- SHA-256 del fragmento entregado: `96bdfb17039e218360c7b4ff544b57ab291b1e13c9e58a0554d6f67b32b940ab`.

