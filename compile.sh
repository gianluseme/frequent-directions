#!/bin/bash


# Creazione della directory di compilazione
mkdir -p build
cd build

# Configurazione del progetto con CMake
cmake ..

# Compilazione del progetto
make

# Copia dei file nella directory build
cp ../accuracy_calc.py .
cp ../test.py .
cp ../smallMatrix.csv .
cp ../plot.py .

# Torna alla directory originale
cd ..
