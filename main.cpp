#include <iostream>
#include <blaze/Math.h>
#include <random>
#include <chrono>
#include "opencsv.h"
#include "frequent_directions.h"


using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::rowMajor;
using blaze::columnVector;
using blaze::blas_int_t;
using blaze::gesvd;
using blaze::StaticMatrix;

int main() {

    // Registra il tempo di inizio
    auto start_time = std::chrono::high_resolution_clock::now();

    constexpr int l = 200;
    int d = 862;

    std::string nomeFileCSV = "trafficNew";

    DynamicMatrix<double> matriceRidotta = frequent_directions::frequentDirectionsStream<l>(nomeFileCSV+".csv", d);
    opencsv::scriviMatriceSuCSV(matriceRidotta, nomeFileCSV+"Ridotto.csv");

    // Registra il tempo di fine
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calcola la durata in millisecondi
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Stampa il tempo di esecuzione
    std::cout << "Tempo di esecuzione: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
