//
// Created by semeg on 12/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_OPENCSV_H
#define FREQUENT_DIRECTIONS_OPENCSV_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <blaze/Math.h>


using blaze::DynamicMatrix;

class opencsv {

public:

    // funzione che converte una stringa in un valore double
    static std::vector<double> convertiStringaInDouble(const std::string &str) {

        try {

            // conversione in double della stringa
            double result = std::stod(str);

            // restituisce un vettore contenente il risultato della conversione e uno 0 a indicare che la conversione è avvenuta con successo
            return {result, 0};

            // se si presentano errori nella conversione, la funzione restituisce come risultato 0.0 e 1, a indicare che la conversione non è avvenuta con successo
        } catch (const std::invalid_argument &e) {
            std::cerr << "Errore nella conversione della stringa in double: " << str << ", " << e.what() << std::endl;
            return {0.0, 1};
        } catch (const std::out_of_range &e) {
            std::cerr << "Overflow durante la conversione della stringa in double: " << str << ", " << e.what()
                      << std::endl;
            return {0.0, 1};
        }
    }

    // funzione che legge una matrice in formato csv e la alloca in una matrice dinamica della libreria blaze
    static DynamicMatrix<double> leggiCSV(const std::string& nomeFile) {

        // apertura del file in modalità binaria
        std::ifstream file(nomeFile, std::ios::binary);

        // Array per i primi tre byte del file
        char bom[4] = {0};

        // lettura i primi tre byte
        file.read(bom, 4);

        file.seekg(0, std::ios::beg);  // Posiziona il cursore all'inizio del file

        //BOM per codifica UTF-8
        if (bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) &&
            bom[2] == static_cast<char>(0xBF)) {
            file.ignore(3); // Ignora i primi 3 byte se vengono rilevati i Byte Order Mark per la codifica UTF-8
        }

        // verifica se il file è aperto correttamente
        if (!file.is_open()) {
            std::cerr << "Errore nell'apertura del file " << nomeFile << ": " << strerror(errno) << std::endl;
            return {};
        }

        // vettore bidimensionale per memorizzare i dati letti dal file csv
        std::vector<std::vector<double>> dati;

        // variabile per memorizzare ciascuna riga letta dal file
        std::string riga;

        // contatore per il numero totale di righe lette
        int i = 0;

        // flag per gestire l'eliminazione di prima riga e colonna nel caso di conversione (eliminazione di eventuali label)
        bool canc = false;

        // ciclo per leggere ogni riga dal file
        while (std::getline(file, riga)) {

            // contatore per il numero totale di colonne lette all'interno della riga corrente
            int j = 0;

            // vettore per memorizzare i valori convertiti della riga corrente
            std::vector<double> rigaDati;

            // string stream per analizzare la riga
            std::stringstream ss(riga);
            std::string dato;

            // loop per analizzare ogni campo separato da virgola nella riga
            while (std::getline(ss, dato, ',')) {

                j++;

                // se il flag di cancellazione è attivo e si sta analizzando la prima riga, salta il ciclo corrente
                if (canc && j == 1) {
                    continue;
                }

                // vettore risultato della funzione convertiStringaInDouble; contiene il valore della conversione e
                // 0 o 1 a indicare il successo dell'operazione
                std::vector<double> convert = convertiStringaInDouble(dato);

                // se si verifica un errore di conversione nella prima riga, attiva il flag di cancellazione
                if (convert[1] == 1 && i == 0) {
                    canc = true;
                    break;
                }

                // se la conversione ha avuto successo, aggiungi il valore convertito al vettore rigaDati
                if (convert[1] != 1)
                    rigaDati.push_back(convert[0]);

            }

            // se il flag di cancellazione non è attivo o se si tratta di una riga successiva alla prima,
            // aggiungi la rigaDati al vettore dei dati
            if (!canc || i != 0)
                dati.push_back(rigaDati);

            i++;
        }

        file.close();

        // numero totale di righe e colonne lette
        int righe = dati.size();
        int colonne = righe > 0 ? dati[0].size() : 0;

        // alloca una matrice dinamica blaze con le dimensioni adeguate
        blaze::DynamicMatrix<double> matrice(righe, colonne);

        // popola la matrice blaze con i dati letti dal file CSV
        for (size_t i = 0; i < righe; ++i) {
            for (size_t j = 0; j < colonne; ++j) {
                matrice(i, j) = dati[i][j];
            }
        }


        return matrice;

    }

    static void scriviMatriceSuCSV(const DynamicMatrix<double> &matrice, const std::string &nomeFile) {

        // apertura del file in modalità scrittura
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
