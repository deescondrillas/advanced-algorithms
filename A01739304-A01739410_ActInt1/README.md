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
g++ -O3 main.cpp SuffixArray.cpp Manacher.cpp SaIs.cpp ./visual/ManacherSession.cpp -o x
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
```zsh
```
