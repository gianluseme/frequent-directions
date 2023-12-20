//
// Created by semeg on 12/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_OPENCSV_H
#define FREQUENT_DIRECTIONS_OPENCSV_H

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <sstream>
#include <blaze/Math.h>
#include <algorithm>

using blaze::DynamicMatrix;

class opencsv {

    struct CSVRow {
        int l;
        int timeFd;
        int timeTotal;
        double accuracy;

        // Operatori per confronto e ordinamento
        bool operator<(const CSVRow& other) const {
            return l < other.l;
        }

        // Operatore di uguaglianza per cercare una riga con lo stesso valore di l
        bool operator==(int value) const {
            return l == value;
        }
    };

    struct CSVString {
        int l;
        std::string timeFd;
        std::string timeTotal;
        std::string accuracy;
    };


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

        DynamicMatrix<double> matrice = blaze::generate(righe, colonne, [&dati](size_t i, size_t j) {
            return dati[i][j];
        });

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

    static void appendCSV(int l, int timeFd, int timeTotal, double accuracy, const std::string& nomeFile) {
        // Vettore per contenere le righe del file CSV
        std::vector<CSVRow> rows;

        // Leggi il contenuto attuale del file
        std::ifstream inputFile(nomeFile);

        CSVRow row;

        for (CSVString i; (inputFile >> i.l).ignore(std::numeric_limits<std::streamsize>::max(), ',') &&
                       std::getline(inputFile, i.timeFd, ',') &&
                       std::getline(inputFile, i.timeTotal, ',') &&
                       std::getline(inputFile, i.accuracy); ) {

            // Converti le stringhe lette in interi e double
            row.l = i.l;
            row.timeFd = std::stoi(i.timeFd);
            row.timeTotal = std::stoi(i.timeTotal);
            row.accuracy = std::stod(i.accuracy);

            rows.push_back(row);
        }

        inputFile.close();

        // Cerca una riga con lo stesso valore di l
        auto it = std::find_if(rows.begin(), rows.end(), [l](const CSVRow& existingRow) {
            return existingRow.l == l;
        });

        // Se trovi una riga con lo stesso valore di l, sostituisci i dati
        if (it != rows.end()) {
            it->timeFd = timeFd;
            it->timeTotal = timeTotal;
            it->accuracy = accuracy;
        } else {
            // Altrimenti, aggiungi una nuova riga
            rows.push_back({l, timeFd, timeTotal, accuracy});
        }

        // Ordina le righe in base a l
        std::sort(rows.begin(), rows.end());

        // Apri il file in modalità out
        std::ofstream outputFile(nomeFile, std::ios::out);

        // Scrivi le righe ordinate nel file
        for (const auto& updatedRow : rows) {
            outputFile << updatedRow.l << ',' << updatedRow.timeFd << ',' << updatedRow.timeTotal << ',' << updatedRow.accuracy << '\n';
        }

        outputFile.close();
    }


};


#endif //FREQUENT_DIRECTIONS_OPENCSV_H
