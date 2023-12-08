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
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinU | Eigen::ComputeThinV);

        //BCDSVD
        //Eigen::BDCSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinU | Eigen::ComputeThinV);

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

};


#endif //FREQUENT_DIRECTIONS_FREQUENTDIRECTIONS_H

