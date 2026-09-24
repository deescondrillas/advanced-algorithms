@echo off
REM Actividad Integradora 1 Lanzador de la visualización
REM   - Octavio Hernández Loyo | A01739304
REM   - Franco De Escondrillas | A01739410
REM Fecha: 2026-09-24
setlocal
pushd "%~dp0.."
where g++ >nul 2>&1
if errorlevel 1 goto missing
where python >nul 2>&1
if errorlevel 1 goto missing
if not exist "visual\build" mkdir "visual\build"
echo Compilando el proyecto...
g++ -std=c++11 -O2 -static visual\session\SessionMain.cpp visual\session\ManacherSession.cpp visual\session\SuffixArraySession.cpp visual\session\SessionText.cpp SuffixArray.cpp SaIs.cpp Manacher.cpp -o "visual\build\algoritmos.exe"
if errorlevel 1 goto failed
echo Abre http://127.0.0.1:8080 en tu navegador.
python visual\server.py --exe "visual\build\algoritmos.exe" --test tests/test1/
if errorlevel 1 goto failed
popd
exit /b 0
:missing
echo Se necesitan g++ y Python 3 disponibles en PATH.
:failed
echo No se pudo iniciar la visualizacion. Revisa el mensaje anterior.
pause
popd
exit /b 1
