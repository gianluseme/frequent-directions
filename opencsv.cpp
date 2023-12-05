//
// Created by semeg on 05/12/2023.
//

#include "opencsv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "Eigen/Dense"
#include <string>

std::vector<double> convertiStringaInDouble(const std::string& str) {

    try {

        double result = std::stod(str);

        return {result, 0};

    } catch (const std::invalid_argument& e) {
        std::cerr << "Errore nella conversione della stringa in double: " << str << ", " << e.what() << std::endl;
        return {0.0, 1};
    } catch (const std::out_of_range& e) {
        std::cerr << "Overflow durante la conversione della stringa in double: " << str << ", " << e.what() << std::endl;
        return {0.0, 1};
    }
}


Eigen::MatrixXd leggiCSVEigen(const std::string& nomeFile) {
    std::ifstream file(nomeFile, std::ios::binary);
    file.ignore(3); // Ignora i primi 3 byte (BOM potrebbe essere di 3 byte)

    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file " << nomeFile << ": " << strerror(errno) << std::endl;
        return {};
    }

    std::vector<std::vector<double>> dati;
    std::string riga;

    int i = 0;

    bool canc = false;

    while (std::getline(file, riga)) {

        int j = 0;

        std::vector<double> rigaDati;
        std::stringstream ss(riga);
        std::string dato;

        while (std::getline(ss, dato, ',')) {

            j++;

            if(canc && j == 1) {
                continue;
            }

            std::vector<double> convert = convertiStringaInDouble(dato);

            if(convert[1] == 1 && i == 0) {
                canc = true;
                break;
            }
            if(convert[1] != 1)
                rigaDati.push_back(convert[0]);

        }

        if(!canc || i != 0)
            dati.push_back(rigaDati);

        i++;
    }

    file.close();

    int righe = dati.size();
    int colonne = righe > 0 ? dati[0].size() : 0;

    Eigen::MatrixXd matrice(righe, colonne);

    for (int i = 0; i < righe; i++) {
        for (int j = 0; j < colonne; j++) {
            matrice(i, j) = dati[i][j];
        }
    }

    return matrice;
}


int main() {

    Eigen::MatrixXd matrice = leggiCSVEigen("traffic.csv");

    //std::cout <<"Matrice letta:\n" << matrice << std::endl;

    std::cout <<"Prima riga della matrice: " << std::endl;

    for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;

    return 0;

}
