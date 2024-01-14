#!/bin/bash

# Aggiorna l'elenco dei pacchetti
sudo apt update

# Installa LAPACK
sudo apt-get install liblapack-dev -y

# Installa OpenBLAS
sudo apt-get install libopenblas-dev -y

# Installa Blaze (C++ Template Library for High-Performance Linear Algebra)
# Puoi scaricare Blaze dal repository GitHub e compilare il codice sorgente
git clone https://github.com/parsa/blaze.git
cd blaze
cmake -DCMAKE_INSTALL_PREFIX=/usr/local/
sudo make install
cd ..

# Installa cxxopts (Lightweight C++ command line option parser)
git clone https://github.com/jarro2783/cxxopts.git
cd cxxopts
sudo cp  include/cxxopts.hpp /usr/local/include
cd ..

echo "Installazione completata."