//
// Created by semeg on 12/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
#define FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <string>
#include <cstring>
#include <blaze/Blaze.h>
#include "opencsv.h"


using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::blas_int_t;
using blaze::gesvd;
using blaze::StaticMatrix;
using blaze::columnMajor;
using blaze::gesdd;


class frequent_directions {

public:

    // funzione per il vero valore di l
    static DynamicMatrix<double> frequentDirections(int l, const std::string& nomeFIle, const bool svd, const bool l_value) {

        std::ifstream file(nomeFIle, std::ios::binary);

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
            std::cerr << "Errore nell'apertura del file " << nomeFIle << ": " << strerror(errno) << std::endl;
            return {};
        }

        std::string line;
        std::getline(file, line);

        std::istringstream iss(line);
        std::string field;

        int columnCount = 0;

        while (std::getline(iss, field, ',')) {
            ++columnCount;
        }

        // verifica che il valore vero di l sia <= del numero di colonne della matrice di input
        // se l_value è true, l è rimasto invariato (sto usando il vero valore di l)
        // se l_value è false, l è doppio rispetto al suo valore originale
        if(l_value) { // l
            if (l > columnCount) {
                std::cerr << "l non deve essere più grande del numero di colonne della matrice di input (n. colonne: " << columnCount << ")." << std::endl;
                exit(1);
            }
        } else if(l/2 > columnCount) {  // 2*l
            std::cerr << "l non deve essere più grande del numero di colonne della matrice di input(n. colonne: " << columnCount << ")." << std::endl;
            exit(1);
        }

        DynamicMatrix<double> B(l, columnCount, 0.0);

        DynamicVector<double> row(columnCount);
        row.reserve(columnCount);

        //tiene traccia dell'indice della riga nulla successiva di B
        int nextZeroRow = 0;

        // Leggi una riga alla volta
        while (std::getline(file, line)) {
            // Usa uno stringstream per analizzare la riga CSV
            std::istringstream iss(line);
            std::string value;

            for (int j = 0; j < columnCount && std::getline(iss, value, ','); ++j) {
                // Converti il valore da stringa a double e aggiungilo alla riga
                row[j] = std::stod(value);
            }

            // Passa la riga alla funzione
            appendRow(l, row, B, nextZeroRow, svd);

        }

        if(!l_value)
            B = submatrix(B,0UL, 0UL, l/2, B.columns());  //restituisco le prime l righe della matrice

        return B;

    }


    static void appendRow(int l, const DynamicVector<double> &Ai, DynamicMatrix<double> &B, int& nextZeroRow, const bool svd) {

        // se non ci sono più righe vuote, ruota la matrice
        if(nextZeroRow >= l)
            rotateB(l, B, nextZeroRow, svd);

        // aggiungi una nuova riga della matrice di input
        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }


    static void rotateB(int l, DynamicMatrix<double> &B, int& nextZeroRow, const bool svd) {

        // Allocazione per i valori singolari
        DynamicVector<double> S;

        // Allocazione per la matrice V
        DynamicMatrix<double> V;

        if(svd)
            gesvd(B, S, V, 'N', 'S');
        else {
            DynamicMatrix<double> U;
            gesdd(B, U, S, V, 'S');
        }

        int halfl = (l/2) - 1;

        double delta = blaze::pow(S[halfl], 2);
        S = blaze::sqrt(blaze::max(S * S - delta, 0.0));
        DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
        blaze::diagonal(diagS) = S;

        submatrix(B, 0UL, 0UL, l / 2, B.columns()) = submatrix(diagS*V, 0UL, 0UL, l / 2, (diagS*V).columns());
        submatrix(B, l / 2, 0UL, B.rows() - l / 2, B.columns()) = 0.0;
        nextZeroRow = halfl;

    }


    static double boundCalculation(const DynamicMatrix<double>& A, int l) {

        double norm = blaze::l2Norm(A);
        return 2*(norm * norm)/l;

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
