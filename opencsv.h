//
// Created by semeg on 05/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_OPENCSV_H
#define FREQUENT_DIRECTIONS_OPENCSV_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "Eigen/Dense"
#include <string>


class opencsv {

public:

  static std::vector<double> convertiStringaInDouble(const std::string &str) {

        try {

            double result = std::stod(str);

            return {result, 0};

        } catch (const std::invalid_argument &e) {
            std::cerr << "Errore nella conversione della stringa in double: " << str << ", " << e.what() << std::endl;
            return {0.0, 1};
        } catch (const std::out_of_range &e) {
            std::cerr << "Overflow durante la conversione della stringa in double: " << str << ", " << e.what()
                      << std::endl;
            return {0.0, 1};
        }
    }


  static  Eigen::MatrixXd leggiCSVEigen(const std::string &nomeFile) {
        std::ifstream file(nomeFile, std::ios::binary);


        // Array per i primi tre byte del file
        char bom[4] = {0};

        // Legge i primi tre byte
        file.read(bom, 4);

        file.seekg(0, std::ios::beg);  // Posiziona il cursore all'inizio del file

        //BOM per codifica UTF-8
        if (bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) &&
            bom[2] == static_cast<char>(0xBF)) {
            std::cout << "Il file ha il BOM per la codifica UTF-8" << std::endl;
            file.ignore(3); // Ignora i primi 3 byte
        }
            //BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE)) ||
                 (bom[0] == static_cast<char>(0xFE) && bom[1] == static_cast<char>(0xFF))) {
            std::cout << "Il file ha il BOM per la codifica UTF-16" << std::endl;
            file.ignore(2); // Ignora i primi 2 byte
        }
            // BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE) &&
                  bom[2] == static_cast<char>(0x00) && bom[3] == static_cast<char>(0x00)) ||
                 (bom[0] == static_cast<char>(0x00) && bom[1] == static_cast<char>(0x00) &&
                  bom[2] == static_cast<char>(0xFE) && bom[3] == static_cast<char>(0xFF))) {
            std::cout << "Il file ha il BOM per la codifica UTF-32" << std::endl;
            file.ignore(4); // Ignora i primi 4 byte
        } else
            std::cout << "Il file non presenta BOM" << std::endl;


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

                if (canc && j == 1) {
                    continue;
                }

                std::vector<double> convert = convertiStringaInDouble(dato);

                if (convert[1] == 1 && i == 0) {
                    canc = true;
                    break;
                }
                if (convert[1] != 1)
                    rigaDati.push_back(convert[0]);

            }

            if (!canc || i != 0)
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


  static void scriviMatriceSuCSV(const Eigen::MatrixXd &matrice, const std::string &nomeFile) {

        std::ofstream file(nomeFile);

        if (!file.is_open()) {
            std::cerr << "Errore nell'apertura del file " << nomeFile << std::endl;
            return;
        }

        int righe = matrice.rows();
        int colonne = matrice.cols();

        std::cout << "n. righe: " << righe << "\nn. colonne: " << colonne << std::endl;

        // Scrive ogni elemento della matrice nel file CSV
        for (int i = 0; i < righe; i++) {
            for (int j = 0; j < colonne; j++) {
                file << matrice(i, j);

                // Aggiunge una virgola dopo ogni elemento, tranne per l'ultimo elemento della riga
                if (j < colonne - 1) {
                    file << ",";
                }
            }

            // Aggiunge un ritorno a capo alla fine di ogni riga, tranne per l'ultima riga
            if (i < righe - 1) {
                file << "\n";
            }
        }

        file.close();

    }


};

#endif //FREQUENT_DIRECTIONS_OPENCSV_H
