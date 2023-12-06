//
// Created by semeg on 05/12/2023.
//

#include "opencsv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include "Eigen/Dense"
#include <string>




int main1() {

    Eigen::MatrixXd matrice = opencsv::leggiCSVEigen("trafficNew.csv");

    //std::cout <<"Matrice letta:\n" << matrice << std::endl;

    std::cout <<"Prima riga della matrice: " << std::endl;

    for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;

    // Scrive la matrice su un nuovo file CSV
    //scriviMatriceSuCSV(matrice, "trafficNew.csv");

    //std::cout << "Matrice scritta su trafficNew.csv\n";

    return 0;

}
