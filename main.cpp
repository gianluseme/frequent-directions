#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Dense"
#include "opencsv.h"
#include "frequentdirections.h"
#include <chrono>


using namespace Eigen;
using namespace std;




int main(int argc, char* argv[]) {

    /*if (argc != 3) {
        std::cerr << "Utilizzo: " << argv[0] << " <dimensione_riduzione> <nome_file_input>" << std::endl;
        return 1; // Ritorna 1 per indicare errore nei parametri
    }*/

    // Ottieni la dimensione di riduzione (l)
    //const int l = std::stoi(argv[1]);

    // Ottieni i nomi dei file
    //std::string nomeFileCSV = argv[2];

    // Registra il tempo di inizio
    auto start = std::chrono::high_resolution_clock::now();

    const int l = 80;

    //n. colonne
    constexpr int d = 80;

    std::string nomeFileCSV = "matricecsv";

    Eigen::MatrixXd matrice = opencsv::leggiCSVEigen(nomeFileCSV+".csv");

    // Genera una matrice casuale n x m
    //Eigen::MatrixXd matrice = Eigen::MatrixXd::Random(1000,80);

    //std::cout <<"Matrice letta:\n" << matrice << std::endl;

    std::cout <<"Matrice letta con successo." << std::endl;


    //std::cout <<"Prima riga della matrice: " << std::endl;

    for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;

    //opencsv::scriviMatriceSuCSV(matrice, "matricecsv.csv");

    Eigen::MatrixXd matriceRidotta = FrequentDirections::frequentDirections<l>(matrice, d,nomeFileCSV+".csv");

    opencsv::scriviMatriceSuCSV(matriceRidotta, nomeFileCSV+"Ridotto.csv");

    // Registra il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();

    // Calcola il tempo trascorso
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Stampa il tempo trascorso
    std::cout << "Tempo di esecuzione: " << duration.count() << " microsecondi" << std::endl;

    return 0;
}
