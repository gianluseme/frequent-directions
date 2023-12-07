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

class FrequentDirections {

public:

    static Eigen::MatrixXd frequentDirections (const int l, const Eigen::MatrixXd& A) {

        int cols = A.cols();
        int rows = A.rows();

        std::cout << "righe: " << l << std::endl;
        std::cout << "colonne: " << cols << std::endl;

        // Inizializzazione matrice B a zero
        Eigen::MatrixXd B = initB(l, cols);

        for(int i = 0; i < rows; i++) {

            Eigen::RowVectorXd Ai = A.row(i);
            B = append(Ai, i, l, i, B);

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

    static Eigen::MatrixXd append(Eigen::RowVectorXd& Ai, int row, int l, int iteration, Eigen::MatrixXd B) {

        for(int i = row; i < l; i++) {
            //std::cout << "\nriga " << i << " di A: " << Ai << std::endl;
            if((B.row(i).array() == 0).all()) {
                B.row(i) = Ai;
                //std::cout << "\nriga " << i << " aggiornata.\n" << std::endl;
                return B;
            }
        }

        B = rotateB(iteration, l, B);

        return B;

    }

    static Eigen::MatrixXd rotateB(int i, int l, Eigen::MatrixXd B) {

        //std::cout << "Eseguo rotazione della matrice B... (riga " << i+1 << ")\n" << std::endl;

        //provare sia JacobiSVD che BCDSVD della libreria Eigen per calcolare l'SVD

        //JacobiSVD
        //Eigen::JacobiSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinU | Eigen::ComputeThinV);

        //BCDSVD
        Eigen::BDCSVD<Eigen::MatrixXd> svd(B, Eigen::ComputeThinU | Eigen::ComputeThinV);

        // Ottenere le matrici U, S e V
        // Eigen::MatrixXd U = svd.matrixU();
        Eigen::MatrixXd S = svd.singularValues().asDiagonal();
        Eigen::MatrixXd V = svd.matrixV();

        //std::cout << "Matrice U:\n" << U << "\n\n";
        //std::cout << "Matrice S:\n" << S << "\n\n";
        //std::cout << "Matrice V:\n" << V << "\n\n";

        if(S(l-1, l-1) != 0) {

            int halfl = (l/2) - 1;
            double delta = std::pow(S(halfl, halfl), 2);

            //std::cout << "delta = " << delta << "\n" << std::endl;

            for(int i = 0; i < l; i++) {
                double deltaPrimo = std::sqrt(std::pow((S(i, i)), 2) - delta);
                if(deltaPrimo > 0) {
                    S(i, i) = deltaPrimo;
                } else {
                    S(i, i) = 0;
                }

            }

            //std::cout << "\n S':\n" << S << std::endl; // S'


        }

        B = S * V.transpose();

        //std::cout << "\n Matrice B aggiornata:\n" << B << std::endl;

        return B;

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENTDIRECTIONS_H

