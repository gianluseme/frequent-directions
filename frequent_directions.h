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


    static DynamicMatrix<double> frequentDirections(int l, const std::string& nomeFile, DynamicMatrix<double> A, bool svd) {

        int cols = A.columns();
        int rows = A.rows();

        DynamicMatrix<double> B(2*l, cols, 0.0);

        int nextZeroRow = 0;

        for(int i = 0; i < rows; i++) {
            auto vect = blaze::row(A, i);

            blaze::DynamicVector<double> Ai(cols);

            for(size_t j = 0; j < cols; ++j)
                Ai[j] = vect[j];

            // Passa la riga alla funzione
            if(svd)
                appendGESVD(2*l, Ai, i, B, nextZeroRow);
            else
                appendGESDD(2*l, Ai, i, B, nextZeroRow);

        }

        B = submatrix(B,0UL, 0UL, l, B.columns());
        return B;

    }


    static DynamicMatrix<double> frequentDirectionsStream(int l, const std::string& nomeFile, int cols, bool svd) {

        DynamicMatrix<double> B(2*l, cols, 0.0);
        DynamicVector<double> row(cols);

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
            //BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE)) ||
                 (bom[0] == static_cast<char>(0xFE) && bom[1] == static_cast<char>(0xFF))) {
            file.ignore(2); // Ignora i primi 2 byte
        }
            // BOM per codifica UTF-16 (little-endian o big-endian)
        else if ((bom[0] == static_cast<char>(0xFF) && bom[1] == static_cast<char>(0xFE) &&
                  bom[2] == static_cast<char>(0x00) && bom[3] == static_cast<char>(0x00)) ||
                 (bom[0] == static_cast<char>(0x00) && bom[1] == static_cast<char>(0x00) &&
                  bom[2] == static_cast<char>(0xFE) && bom[3] == static_cast<char>(0xFF))) {
            file.ignore(4); // Ignora i primi 4 byte
        } else


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
            if(svd)
                appendGESVD(2*l, row, i, B, nextZeroRow);
            else
                appendGESDD(2*l, row, i, B, nextZeroRow);

            i++;

        }

        B = submatrix(B,0UL, 0UL, l, B.columns());
        //return blaze::submatrix(B,0UL, 0UL, l, B.columns());
        return B;

    }


    static void appendGESVD(int l, DynamicVector<double> &Ai, int row, DynamicMatrix<double> &B, int& nextZeroRow) {

        if(nextZeroRow >= l)
            rotateBGESVD(l, B, nextZeroRow);

        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }

    static void appendGESDD(int l, DynamicVector<double> &Ai, int row, DynamicMatrix<double> &B, int& nextZeroRow) {

        if(nextZeroRow >= l)
            rotateBGESDD(l, B, nextZeroRow);

        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }


    static void rotateBGESVD(int l, DynamicMatrix<double> &B, int& nextZeroRow) {

        // Allocazione per i valori singolari
        DynamicVector<double> S;

        // Allocazione per la matrice V
        DynamicMatrix<double> V;

        gesvd(B, S, V, 'N', 'S');

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

    static void rotateBGESDD(int l, DynamicMatrix<double> &B, int& nextZeroRow) {

        // Allocazione per i valori singolari
        DynamicVector<double> S;

        // Allocazione per la matrice V
        DynamicMatrix<double> V;
        DynamicMatrix<double> U;

        gesdd(B, U, S, V, 'S');

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

    static void accuracyTest(DynamicMatrix<double> A, DynamicMatrix<double> B) {

        DynamicMatrix<double> diff = (trans(A) * A) - (trans(B) * B);
        std::cout << "Hello" << std::endl;
        const double l1norm = blaze::l1Norm(diff);
        std::cout << "Accuracy: " << l1norm << std::endl;

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
