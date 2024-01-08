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

    // funzione per il vero valore di l
    static DynamicMatrix<double> frequentDirections1(int l, const std::string& nomeFIle, bool svd) {

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

        DynamicMatrix<double> B(l, columnCount, 0.0);
        DynamicVector<double> row(columnCount);

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
            append1(l, row, B, nextZeroRow, svd);

        }

        return B;

    }


    //funzione per il doppio valore di l
    static DynamicMatrix<double> frequentDirectionsStream(int l, const std::string& nomeFile, bool svd) {

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
        std::getline(file, line);

        std::istringstream iss(line);
        std::string field;

        int columnCount = 0;

        while (std::getline(iss, field, ',')) {
            ++columnCount;
        }

        DynamicMatrix<double> B(2*l, columnCount, 0.0);
        DynamicVector<double> row(columnCount);

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
            append(2 * l, row, B, nextZeroRow, svd);

        }

        B = submatrix(B,0UL, 0UL, l, B.columns());
        return B;

    }


    static void append(int l, DynamicVector<double> &Ai, DynamicMatrix<double> &B, int& nextZeroRow, bool svd) {

        if(nextZeroRow >= l)
            rotateB(l, B, nextZeroRow, svd);

        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }


    static void append1(int l, DynamicVector<double> &Ai, DynamicMatrix<double> &B, int& nextZeroRow, bool svd) {

        if(nextZeroRow >= l)
            rotateB1(l, B, nextZeroRow, svd);

        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        nextZeroRow += 1;

    }

    // per vero valore di l
    static void rotateB1(int l, DynamicMatrix<double> &B, int& nextZeroRow, bool svd) {

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

        if(S.size() >= l) {

            double delta = blaze::pow(S[halfl], 2);
            S = blaze::sqrt(blaze::max(S * S - delta, 0.0));
            DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
            blaze::diagonal(diagS) = S;

            B = diagS * V;

            nextZeroRow = halfl;

        } else {

            DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
            blaze::diagonal(diagS) = S;

            submatrix(B, 0UL, 0UL, S.size(), B.columns()) = diagS * V;
            submatrix(B, S.size(), 0UL, B.rows() - S.size(), B.columns()) = submatrix(B, S.size(), 0UL, B.rows() - S.size(), B.columns()) * 0.0;

            nextZeroRow = S.size();

        }


    }


    // per 2*l
    static void rotateB(int l, DynamicMatrix<double> &B, int& nextZeroRow, bool svd) {

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

    static int findFirstZeroRow(const DynamicMatrix<double>& B) {
        for (size_t i = 0; i < B.rows(); ++i) {
            // Verifica se la riga è completamente composta da zeri
            if (blaze::isZero(row(B, i))) {
                return static_cast<int>(i);  // Restituisce l'indice della prima riga con zeri
            }
        }
        return -1;  // Se nessuna riga è composta solo da zeri
    }


    static double accuracyTest(DynamicMatrix<double> A, DynamicMatrix<double> B) {

        DynamicMatrix<double> diff = (trans(A) * A) - (trans(B) * B);
        const double accuracy = blaze::l2Norm(diff);
        std::cout << "Accuracy: " << accuracy << std::endl;
        return accuracy;

    }

    static double boundCalculation(DynamicMatrix<double> A, int l) {

        double norm = blaze::l2Norm(A);
        return 2*(norm * norm)/l;

    }

    static void boundCalculationIter(DynamicMatrix<double> A) {

        double norm = blaze::sqrNorm(A); //norma di frobenius al quadrato

        std::vector<int> ls = {10, 20, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 400, 500, 750, 1000, 2000, 3000, 4000, 5000, 7500, 10000};

        for(int i = 0; i < ls.size(); i++) {
            double bound = 2*(norm)/ls[i];
            std::cout << bound << std::endl;
        }

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
