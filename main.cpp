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

    //if (argc != 3) {
     //   std::cerr << "Utilizzo: " << argv[0] << " <dimensione_riduzione> <nome_file_input>" << std::endl;
     //   return 1; // Ritorna 1 per indicare errore nei parametri
   // }

    // Ottieni la dimensione di riduzione (l)
    //const int l = std::stoi(argv[1]);

    // Ottieni i nomi dei file
    //std::string nomeFileCSV = argv[2];

    // Registra il tempo di inizio
    auto start = std::chrono::high_resolution_clock::now();

    const int l = 100;

    //n. colonne
    const int d = 100;

    std::string nomeFileCSV = "matrice";

    //Eigen::MatrixXd matrice = opencsv::leggiCSVEigen(nomeFileCSV+".csv");

    // Genera una matrice casuale n x m
    //Eigen::MatrixXd matrice = Eigen::MatrixXd::Random(100000,100);

    //std::cout <<"Matrice letta:\n" << matrice << std::endl;

    //std::cout <<"Matrice letta con successo." << std::endl;


    //std::cout <<"Prima riga della matrice: " << std::endl;

    /*for(int j = 0; j < matrice.cols(); j++) {
        std::cout << matrice(0, j) << " ";
    }
    std::cout << std::endl;*/

    //opencsv::scriviMatriceSuCSV(matrice, "matrice.csv");

    //Eigen::MatrixXd matriceRidotta = FrequentDirections::frequentDirections<l>(matrice, d,nomeFileCSV+".csv");
    Eigen::MatrixXd matriceRidotta = FrequentDirections::frequentDirectionStreaming<l>(nomeFileCSV+".csv", d);

    opencsv::scriviMatriceSuCSV(matriceRidotta, nomeFileCSV+"Ridotto.csv");

    //FrequentDirections::accuracyTest(matrice, matriceRidotta);

    // Registra il tempo di fine
    auto end = std::chrono::high_resolution_clock::now();

    // Calcola il tempo trascorso
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Stampa il tempo trascorso
    std::cout << "Tempo di esecuzione: " << duration.count() << " microsecondi" << std::endl;

    return 0;
}
/*
int main() {

    Eigen::MatrixXd matriceA = opencsv::leggiCSVEigen("traffic.csv");

    Eigen::MatrixXd B1 = opencsv::leggiCSVEigen("trafficRidotto.csv");

    Eigen::MatrixXd B2 = opencsv::leggiCSVEigen("sketch.csv");

    FrequentDirections::accuracyTest(matriceA, B1);
    FrequentDirections::accuracyTest(matriceA, B2);

    return 0;

}
*/