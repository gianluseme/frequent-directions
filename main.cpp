#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Dense"
#include "opencsv.h"
#include "frequentdirections.h"
#include <chrono>


using namespace Eigen;
using namespace std;




int main() {

    // Registra il tempo di inizio
    auto start = std::chrono::high_resolution_clock::now();

    const int l = 100;

    std::string nomeFileCSV = "matricecsv";

    Eigen::MatrixXd matrice = opencsv::leggiCSVEigen(nomeFileCSV+".csv");

    // Genera una matrice casuale n x m
    //Eigen::MatrixXd matrice = Eigen::MatrixXd::Random(1000,500);

    //std::cout <<"Matrice letta:\n" << matrice << std::endl;

    std::cout <<"Matrice letta con successo." << std::endl;


    //std::cout <<"Prima riga della matrice: " << std::endl;

    for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;

    //opencsv::scriviMatriceSuCSV(matrice, "matricecsv.csv");

    Eigen::MatrixXd matriceRidotta = FrequentDirections::frequentDirections(l, matrice);

    opencsv::scriviMatriceSuCSV(matriceRidotta, nomeFileCSV+"Ridotto.csv");

    // Registra il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();

    // Calcola il tempo trascorso
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Stampa il tempo trascorso
    std::cout << "Tempo di esecuzione: " << duration.count() << " microsecondi" << std::endl;

    return 0;
}
