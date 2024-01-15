//
// Created by semeg on 12/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_OPENCSV_H
#define FREQUENT_DIRECTIONS_OPENCSV_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <regex>
#include <fstream>
#include <sstream>
#include <blaze/Math.h>
#include <regex>


using blaze::DynamicMatrix;

class opencsv {

public:

    static std::vector<double> convertiStringaInDouble(const std::string &str) {

        try {

            double result = std::stod(str);

            return {result, 0};

        } catch (const std::invalid_argument &e) {
            //std::cerr << "Errore nella conversione della stringa in double: " << str << ", " << e.what() << std::endl;
            return {0.0, 1};
        } catch (const std::out_of_range &e) {
            //std::cerr << "Overflow durante la conversione della stringa in double: " << str << ", " << e.what()
                      //<< std::endl;
            return {0.0, 1};
        }
    }

    static DynamicMatrix<double> leggiCSV(const std::string& nomeFile) {

        std::ifstream file(nomeFile, std::ios::binary);

        // Array per i primi tre byte del file
        char bom[4] = {0};

        // Legge i primi tre byte
        file.read(bom, 4);

        file.seekg(0, std::ios::beg);  // Posiziona il cursore all'inizio del file

        //BOM per codifica UTF-8
        if (bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) &&
            bom[2] == static_cast<char>(0xBF)) {
            file.ignore(3); // Ignora i primi 3 byte
        }

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

        blaze::DynamicMatrix<double> matrice(righe, colonne);

        for (size_t i = 0; i < righe; ++i) {
            for (size_t j = 0; j < colonne; ++j) {
                matrice(i, j) = dati[i][j];
            }
        }


        return matrice;

    }

    static void scriviMatriceSuCSV(const DynamicMatrix<double> &matrice, const std::string &nomeFile) {
        std::ofstream file(nomeFile);

        if (!file.is_open()) {
            std::cerr << "Errore nell'apertura del file " << nomeFile << std::endl;
            return;
        }

        int righe = matrice.rows();
        int colonne = matrice.columns();

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
