#!/bin/bash


# Funzione per controllare l'esistenza di un comando
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

# Verifica l'installazione di liblapack-dev
if ! command_exists dpkg; then
  echo "Il comando dpkg non è disponibile. Assicurati di essere su un sistema con dpkg installato."
  exit 1
fi

# Aggiornamento dell'elenco dei pacchetti
sudo apt update

# Installazione di LAPACK (se non è già installata)
if dpkg -l | grep -q liblapack-dev; then
  echo "liblapack-dev è già installato."
else
  sudo apt-get install liblapack-dev -y
fi

# Installazione di OpenBLAS (se non è già installata)
if dpkg -l | grep -q libopenblas-dev; then
  echo "libopenblas-dev è già installato."
else
  sudo apt-get install libopenblas-dev -y
fi

# Installazione di Blaze (se non è già installata)
if [ -f "/usr/local/include/blaze/Blaze.h" ]; then
    echo "Blaze è già installato."
else
  git clone https://github.com/parsa/blaze.git
  cd blaze
  cmake -DCMAKE_INSTALL_PREFIX=/usr/local/
  sudo make install
  cd ..
fi

# Installazione di cxxopts (se non è già installata)
if [ -f "/usr/local/include/cxxopts.hpp" ] || [ -f "/usr/include/cxxopts.hpp" ]; then
    echo "La libreria cxxopts è già installata."
else
  git clone https://github.com/jarro2783/cxxopts.git
  cd cxxopts
  sudo cp  include/cxxopts.hpp /usr/local/include
  cd ..
fi

echo "Installazione completata."
