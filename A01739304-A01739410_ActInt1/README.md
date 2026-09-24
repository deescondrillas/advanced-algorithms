# Ejecutar el código

Para probar nuestro programa, desarrollamos 5 tests diferentes con datos desde n = 10 hasta n = 10⁵. Por defecto, el test activo es el primero, que tiene datos pequeños amigables para visualizar su ejecución.

Para cambiar el test activo, solo se necesita cambiar una línea en `main.cpp`
```c++
38 | string PATH = "tests/test1/"; 
```
reemplazando `test1/` por el nombre de cualquier otro test (por ejemplo, `test1e5/`). Creamos dos formas principales de ejecutar el programa: compilar solo C++ para medir tiempos de ejecución (con los tests más grandes) y la ejecución visual, para seguir el paso a paso de cada algoritmo (con el test pequeño).

---

## Ejecución directa para medir desempeño
```zsh
g++ -O3 main.cpp SuffixArray.cpp SaIs.cpp Manacher.cpp -o x
```
Mac & Linux
```zsh
time ./x
```
Windows
```zsh
Measure-Command { .\x.exe } # Agregar también .exe al renombrar
```

---

## Ejecución visual
En Windows, ejecuta `visual\IniciarVisualizacion.cmd`, mientras que para Mac y Linux:
```zsh
make visual
```
Posteriormente, accede al localhost.

---

## Referencias

Para implementar el algoritmo SA-IS para ordenar el suffix array en O(n), nos basamos en el paper *Linear Suﬃx Array Construction by Almost Pure Induced-Sorting* (Nong et al, 2009). Además, utilizamos inteligencia artificial generativa (GenAI) para generar los tests de mayor volumen (`tests/test1e2`, `tests/test1e3`, `tests/test1e4`, `tests/test1e5`) y asistirnos en la conexión de los algoritmos en C++ a su visualización en web.


Nong, G., Zhang, S., & Chan, W. H. (2009). Linear suffix array construction by almost pure induced-sorting. 2009 Data Compression Conference, 193-202. <u>https://doi.org/10.1109/DCC.2009.42</u>
