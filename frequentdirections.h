//
// Created by semeg on 06/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_FREQUENTDIRECTIONS_H
#define FREQUENT_DIRECTIONS_FREQUENTDIRECTIONS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "Eigen/Dense"
#include <string>
#include "csv.h"
#include "opencsv.h"

class FrequentDirections {

public:

    template <int l>
    static Eigen::MatrixXd frequentDirections (const Eigen::MatrixXd& A, const int d, const std::string& nomeFile) {

        int cols = A.cols();
        int rows = A.rows();

        std::cout << "righe: " << l << std::endl;
        std::cout << "colonne: " << cols << std::endl;

        // Inizializzazione matrice B a zero
        Eigen::MatrixXd B = initB(l, cols);

        bool check = false;

        int countIExceedsLHalf = 0;

        for(int i = 0; i < rows; i++) {

            if(i >= l)
                check = true;

            // Conta quante volte i supera l/2
            if (i % (l / 2) == 0 && i >= l)
                countIExceedsLHalf++;

            std::cout << "count: " <<countIExceedsLHalf << std::endl;

            std::cout << "\n\n\nRIGA " << i+1 << "\n" << std::endl;

            Eigen::RowVectorXd Ai = A.row(i);
            B = append<l>(Ai, i, B, check, countIExceedsLHalf);


        }

        // std::cout << "Matrice B finale: \n" << B << std::endl;

        std::cout << "Riduzione completata." << std::endl;

        return B;

    }


    template<int l>
    static Eigen::MatrixXd frequentDirectionStreaming(const std::string& nomeFile, int cols) {

        // Inizializzazione matrice B a zero
        Eigen::MatrixXd B = initB(l, cols);

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

        std::string line;

        int i = 0;
        //nextZeroRow
        int index = 0;
        int countIExceedsLHalf = 0;
        bool check = false;

        // Leggi una riga alla volta
        while (std::getline(file, line)) {
            // Usa uno stringstream per analizzare la riga CSV
            std::istringstream iss(line);
            std::string value;

            // Costruisci una RowVectorXd per rappresentare la riga
            Eigen::RowVectorXd row;

            while (std::getline(iss, value, ',')) {
                // Converti il valore da stringa a double e aggiungilo alla riga
                row.conservativeResize(row.size() + 1);
                row(row.size() - 1) = std::stod(value);
            }

            if(i >= l)
                check = true;

            // Conta quante volte i supera l/2
            if (i % (l / 2) == 0 && i >= l)
                countIExceedsLHalf++;

            // Passa la riga alla funzione
            B = append<l>(row, i, B, check, countIExceedsLHalf);

            i++;

        }

        return B;

    }


    static Eigen::MatrixXd initB(int l, int m) {

        Eigen::MatrixXd B = Eigen::MatrixXd::Zero(l, m);

        //std::cout << "Matrice B: \n" << B << "\n*********" << std::endl;

        return B;

    }

    template <int l>
    static Eigen::MatrixXd append(Eigen::RowVectorXd& Ai, int row, Eigen::MatrixXd B, const bool check, int count) {


        if(!check) {
            B.row(row) = Ai;
            //std::cout << "\nriga " << i << " aggiornata.\n" << std::endl;
            std::cout << "Aggiunta riga " << row + 1 << " di A alla matrice B" << std::endl;
        }
        else {

            int index = (row-(count*(l/2)))-1;
            std::cout << index << std::endl;
            if(B.row(index).isZero()) {
                B.row(index) = Ai;
                //std::cout << "\nriga " << i << " aggiornata.\n" << std::endl;
                std::cout << "Aggiunta riga " << row + 1 << " di A alla matrice B (riga " << index+1 << ")" << std::endl;
            }

            else {
                B = rotateB<l>(row, B);
                B.row(index) = Ai;
            }


        }

        return B;

    }



    template<int l>
    static Eigen::MatrixXd rotateB(int i, Eigen::MatrixXd B) {

        std::cout << "Eseguo rotazione della matrice B... (riga " << i << ")\n" << std::endl;

        //provare sia JacobiSVD che BCDSVD della libreria Eigen per calcolare l'SVD

        std::cout << "Calcolo SVD...\n" << std::endl;

        //JacobiSVD
        //Eigen::JacobiSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinV);

        //BCDSVD
        Eigen::BDCSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinV);

        // Ottenere le matrici U, S e V
        // Eigen::MatrixXd U = svd.matrixU();
        Eigen::DiagonalMatrix<double, l> S = svd.singularValues().asDiagonal();
        Eigen::MatrixXd V = svd.matrixV();

        //std::cout << "Matrice U:\n" << U << "\n\n";
        //std::cout << "Matrice S:\n" << S << "\n\n";
        //std::cout << "Matrice V:\n" << V << "\n\n";

        //if(S(l-1, l-1) != 0) {

        //std::cout << "sigma_l =/= 0: calcolo S'... \n" << std::endl;

        std:: cout << "Calcolo S'...\n" << std::endl;

        int halfl = (l/2) - 1;
        double delta = std::pow(S.diagonal()(halfl), 2);

        //std::cout << "delta = " << delta << "\n" << std::endl;

        S.diagonal() = (S.diagonal().array().square() - delta).cwiseMax(0).sqrt();

        //std::cout << "\n S':\n" << S << std::endl; // S'


        //} else
        //std::cout << "sigma_l = 0 \n" << std::endl;

        std::cout <<"Aggiornamento B...\n" << std::endl;

        B = S * V.transpose();

        std::cout <<"Matrice B aggiornata." << std::endl;

        return B;

    }

    /*
    template <int l>
    static Eigen::MatrixXd frequentDirections (const Eigen::MatrixXd& A, const int d, const std::string& nomeFile) {

        int cols = A.cols();
        int rows = A.rows();

        std::cout << "righe: " << l << std::endl;
        std::cout << "colonne: " << cols << std::endl;

        // Inizializzazione matrice B a zero
        Eigen::MatrixXd B = initB(2*l, cols);

        bool check = false;

        int countIExceedsLHalf = 0;

        // per tenere traccia della riga nulla successiva
        int index = 0;

        for(int i = 0; i < rows; i++) {

            if(i >= 2*l)
                check = true;

            // Conta quante volte i diventa un multiplo di l/2
            if (i % (l) == 0 && i >= 2*l)
                countIExceedsLHalf++;

            std::cout << "count: " <<countIExceedsLHalf << std::endl;

            std::cout << "\n\n\nRIGA " << i+1 << "\n" << std::endl;

            Eigen::RowVectorXd Ai = A.row(i);
            //B = append<2*l>(Ai, i, B, check, countIExceedsLHalf);
            B = append<2*l>(Ai, i, B, index);


        }

        // std::cout << "Matrice B finale: \n" << B << std::endl;

        std::cout << "Riduzione completata." << std::endl;

        return B.topRows(l);

    }

    static Eigen::MatrixXd initB(int l, int m) {

        Eigen::MatrixXd B = Eigen::MatrixXd::Zero(l, m);

        //std::cout << "Matrice B: \n" << B << "\n*********" << std::endl;

        return B;

    }


    template <int l>
    static Eigen::MatrixXd append(Eigen::RowVectorXd& Ai, int row, Eigen::MatrixXd B, int& index) {

        if(index >= l)
            B = rotateB<l>(row, B, index);

        B.row(index) = Ai;
        std::cout << "Aggiunta riga " << row + 1 << " di A alla matrice B (riga " << index+1 << ")" << std::endl;
        index += 1;

        return B;

    }



    template<int l>
    static Eigen::MatrixXd rotateB(int i, Eigen::MatrixXd B, int& index) {

        std::cout << "Eseguo rotazione della matrice B... (riga " << i << ")\n" << std::endl;

        //provare sia JacobiSVD che BCDSVD della libreria Eigen per calcolare l'SVD

        std::cout << "Calcolo SVD...\n" << std::endl;

        //JacobiSVD
        //Eigen::JacobiSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinV);

        //BDCSVD
        Eigen::BDCSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinV);

        // Ottenere le matrici U, S e V
        // Eigen::MatrixXd U = svd.matrixU();
        Eigen::VectorXd S = svd.singularValues();
        Eigen::MatrixXd V = svd.matrixV();

        //std::cout << "Matrice U:\n" << U << "\n\n";
        //std::cout << "Matrice S:\n" << S << "\n\n";
        //std::cout << "Matrice V:\n" << V << "\n\n";

        int halfl = (l/2) - 1;

        //std::cout << "sigma_l =/= 0: calcolo S'... \n" << std::endl;

        //std:: cout << "Calcolo S'...\n" << std::endl;

        std::cout <<"Aggiornamento B...\n" << std::endl;

        if(S.size() >= l/2) {

            double delta = std::pow(S(halfl), 2);
            S = (S.array().square() - delta).cwiseMax(0).sqrt();
            Eigen::MatrixXd tempMatrix = S.asDiagonal() * V.transpose();
            B.topRows(l/2) = tempMatrix.topRows(l/2);
            B.bottomRows(B.rows() - l/2).setZero();

            index = l/2;


        } else {

            std::cout << "caso 2" << std::endl;
            B.topRows(S.size()) = S.asDiagonal() * V.transpose();
            B.bottomRows(B.rows() - S.size()).setZero();

            index = S.size();

        }


        //B = S.asDiagonal() * V.transpose();

        std::cout <<"Matrice B aggiornata." << std::endl;

        return B;

    }

    static void accuracyTest(Eigen::MatrixXd A, Eigen:: MatrixXd B) {

        Eigen::MatrixXd matrixDifference = (A.transpose() * A) - (B.transpose() * B);

        double score = matrixDifference.lpNorm<1>();

        std::cout <<"Accuracy: " << score << std::endl;

    }*/


};


#endif //FREQUENT_DIRECTIONS_FREQUENTDIRECTIONS_H

