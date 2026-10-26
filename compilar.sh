#!/bin/sh
# Compila el procesador de PdL-JS del grupo 21 (macOS / Linux)
cd "$(dirname "$0")" || exit 1
c++ -std=c++17 -O2 -I TS_CPP -I TS_CPP/Include -o analizador \
    analizador.cpp TS_CPP/TSManager.cpp TS_CPP/Include/*.cpp \
  && echo "Compilado: ./analizador" || echo "Error de compilacion"
