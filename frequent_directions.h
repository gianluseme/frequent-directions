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
#include <blaze/Math.h>
#include "opencsv.h"


using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::blas_int_t;
using blaze::gesvd;
using blaze::StaticMatrix;
using blaze::columnMajor;


class frequent_directions {

public:

    template<int l>
    static DynamicMatrix<double> frequentDirectionsStream(const std::string& nomeFile, int cols) {

        DynamicMatrix<double> B(2*l, cols, 0.0);
        DynamicVector<double> row(cols);

        std::ifstream file(nomeFile, std::ios::binary);
/*
        // Array per i primi tre byte del file
        char bom[4] = {0};

        // Legge i primi tre byte
        file.read(bom, 4);

        file.seekg(0, std::ios::beg);  // Posiziona il cursore all'inizio del file

        //BOM per codifica UTF-8
        if (bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) &&
            bom[2] == static_cast<char>(0xBF)) {
            //std::cout << "Il file ha il BOM per la codifica UTF-8" << std::endl;
            file.ignore(3); // Ignora i primi 3 byte
        }
            //BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE)) ||
                 (bom[0] == static_cast<char>(0xFE) && bom[1] == static_cast<char>(0xFF))) {
            //std::cout << "Il file ha il BOM per la codifica UTF-16" << std::endl;
            file.ignore(2); // Ignora i primi 2 byte
        }
            // BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE) &&
                  bom[2] == static_cast<char>(0x00) && bom[3] == static_cast<char>(0x00)) ||
                 (bom[0] == static_cast<char>(0x00) && bom[1] == static_cast<char>(0x00) &&
                  bom[2] == static_cast<char>(0xFE) && bom[3] == static_cast<char>(0xFF))) {
            //std::cout << "Il file ha il BOM per la codifica UTF-32" << std::endl;
            file.ignore(4); // Ignora i primi 4 byte
        } else
            //std::cout << "Il file non presenta BOM" << std::endl;
*/

        if (!file.is_open()) {
            std::cerr << "Errore nell'apertura del file " << nomeFile << ": " << strerror(errno) << std::endl;
            return {};
        }

        std::string line;

        //tiene traccia della riga della matrice A
        int i = 0;
        //tiene traccia dell'indice della riga nulla successiva di B
        int nextZeroRow = 0;

        // Leggi una riga alla volta
        while (std::getline(file, line)) {
            // Usa uno stringstream per analizzare la riga CSV
            std::istringstream iss(line);
            std::string value;


            for (int j = 0; j < cols && std::getline(iss, value, ','); ++j) {
                // Converti il valore da stringa a double e aggiungilo alla riga
                row[j] = std::stod(value);
            }

            // Passa la riga alla funzione
            append<2*l>(row, i, B, nextZeroRow);

            i++;

        }

        return blaze::submatrix(B,0UL, 0UL, l, B.columns());

    }


    template<int l>
    static void append(DynamicVector<double> &Ai, int row, DynamicMatrix<double> &B, int& nextZeroRow) {

        if(nextZeroRow >= l)
            rotateB<l>(B, nextZeroRow);

        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }


    template<int l>
    static void rotateB(DynamicMatrix<double> &B, int& nextZeroRow) {

        // Allocazione per i valori singolari
        DynamicVector<double> S;
        //S.resize(B.columns());

        // Allocazione per la matrice V
        DynamicMatrix<double> V;
        //V.resize(B.columns(), B.columns());

        DynamicMatrix<double> U;

        svd(B, U, S, V);

        // Ottenere puntatori ai dati della matrice Blaze
        /*double* Adata = B.data();
        std::ptrdiff_t lda = std::max<blaze::blas_int_t>(1, B.rows()); // Leading dimension

        // Lavoro ausiliario per la funzione gesvd
        double work_query;
        blas_int_t lwork = -1;
        blas_int_t info;

        gesvd('N', 'S', B.rows(), B.columns(), Adata, lda, S.data(), nullptr, 1, V.data(), B.columns(), &work_query, lwork, &info);

        lwork = static_cast<blaze::blas_int_t>(work_query);
        std::vector<double> work(lwork);

        gesvd('N', 'S', B.rows(), B.columns(), Adata, lda, S.data(), nullptr, 1, V.data(), B.columns(), work.data(), lwork, &info);
*/

        int halfl = (l/2) - 1;

        if(S.size() >= l/2) {

            double delta = blaze::pow(S[halfl], 2);
            S = blaze::sqrt(blaze::max(S * S - delta, 0.0));
            DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
            blaze::diagonal(diagS) = S;
            auto tempMatrix = diagS * V;

            submatrix(B, 0UL, 0UL, l / 2, B.columns()) = submatrix(tempMatrix, 0UL, 0UL, l / 2, tempMatrix.columns());
            submatrix(B, l / 2, 0UL, B.rows() - l / 2, B.columns()) = submatrix(B, l / 2, 0UL, B.rows() - l / 2, B.columns()) * 0.0;
            nextZeroRow = l / 2;

        } else {

            DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
            blaze::diagonal(diagS) = S;
            submatrix(B, 0UL, 0UL, S.size(), B.columns()) = diagS * V;
            submatrix(B, S.size(), 0UL, B.rows() - S.size(), B.columns()) = submatrix(B, S.size(), 0UL, B.rows() - S.size(), B.columns()) * 0.0;

            nextZeroRow = S.size();

        }

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
