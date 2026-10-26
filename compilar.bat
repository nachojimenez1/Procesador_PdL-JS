@echo off
REM Compila el procesador de PdL-JS del grupo 21 (requiere g++ / MinGW)
g++ -std=c++17 -O2 -I TS_CPP -I TS_CPP/Include -o analizador.exe analizador.cpp TS_CPP/TSManager.cpp TS_CPP/Include/AttributeValue.cpp TS_CPP/Include/Entry.cpp TS_CPP/Include/SymbolTable.cpp TS_CPP/Include/TSUtils.cpp
if errorlevel 1 (echo Error de compilacion) else (echo Compilado: analizador.exe)
