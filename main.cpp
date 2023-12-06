#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Dense"
#include "opencsv.h"
#include "frequentdirections.h"

using namespace Eigen;
using namespace std;




int main() {

    int l = 2;

    Eigen::MatrixXd matrice = opencsv::leggiCSVEigen("testcsv.csv");

    std::cout <<"Matrice letta:\n" << matrice << std::endl;

    //std::cout <<"Prima riga della matrice: " << std::endl;

    for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;

    std::cout << FrequentDirections::frequentDirections(l, matrice);

    return 0;
}
