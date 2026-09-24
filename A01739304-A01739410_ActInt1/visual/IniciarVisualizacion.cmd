@echo off
setlocal
pushd "%~dp0.."
where g++ >nul 2>&1
if errorlevel 1 goto missing
where python >nul 2>&1
if errorlevel 1 goto missing
if not exist "visual\.build" mkdir "visual\.build"
echo Compilando el proyecto...
g++ -std=c++11 -O2 -static main.cpp SuffixArray.cpp SaIs.cpp Manacher.cpp visual\ManacherSession.cpp visual\SuffixArraySession.cpp visual\SessionText.cpp -o "visual\.build\algoritmos.exe"
if errorlevel 1 goto failed
echo Abre http://127.0.0.1:8080 en tu navegador.
python visual\server.py --exe "visual\.build\algoritmos.exe"
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
