
#define BLAZE_BLAS_MODE 1
#define BLAZE_BLAS_IS_64BIT 1
#define BLAZE_BLAS_IS_PARALLEL 1
#define BLAZE_USE_BLAS_MATRIX_MATRIX_MULTIPLICATION 1
#define BLAZE_USE_BLAS_MATRIX_VECTOR_MULTIPLICATION 1
#define BLAZE_BLAS_INCLUDE_FILE "/usr/include/x86_64-linux-gnu/cblas.h"
//#define BLAZE_BLAS_INCLUDE_FILE "/usr/include/mkl/mkl_cblas.h"

#include <iostream>
#include <blaze/Math.h>
#include <random>
#include <chrono>
#include "opencsv.h"
#include "frequent_directions.h"
#include <cxxopts.hpp>


using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::rowMajor;
using blaze::columnVector;
using blaze::gesvd;
using blaze::StaticMatrix;
using blaze::gesdd;

int main(int argc, char* argv[]) {

    // Argomenti: file di input, l (numero di righe della matrice ridotta), d(n. colonne),
    // parametro per scegliere tra la funzione gesvd e gesdd (default gesvd), modalità di funzionamento (solo riduzione/calcolo accuratezza, default solo riduzione)

    // Registra il tempo di inizio
    auto start_time = std::chrono::high_resolution_clock::now();

    int l;
    int d;
    std::string nomeFileCSV;
    bool svd;
    bool mode;

    cxxopts::Options options("Frequent Directions", "Descrizione del programma");

    options.add_options()
            ("h,help", "Mostra l'aiuto")
            ("i,input", "File di input (.csv)", cxxopts::value<std::string>())
            ("l", "N. righe della matrice ridotta", cxxopts::value<int>())
            ("d", "N. colonne della matrice ridotta", cxxopts::value<int>())
            ("svd", "Scelta tra gesvd e gesdd", cxxopts::value<std::string>()->default_value("gesvd"))
            ("mode", "Modalità (solo riduzione/accuracy test)", cxxopts::value<std::string>()->default_value("ronly"));

    try {

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (!result.count("input") || !result.count("l") || !result.count("d")) {
            std::cerr << "Errore: Le opzioni input, l e d sono obbligatorie." << std::endl;
            return 1;
        }

        nomeFileCSV = result["input"].as<std::string>();
        l = result["l"].as<int>();
        d = result["d"].as<int>();

        if(result["svd"].as<std::string>() == "gesvd")
            svd = true;
        else if(result["svd"].as<std::string>() == "gesdd")
            svd = false;
        else {
            std::cout << "Argomento non valido per --svd" << std::endl;
            return 1;
        }

        if(result["mode"].as<std::string>() == "ronly")
            mode = true;
        else if(result["mode"].as<std::string>() == "acc")
            mode = false;
        else {
            std::cout << "Argomento non valido per --mode" << std::endl;
            return 1;
        }


    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Errore nell'analisi degli argomenti: " << e.what() << std::endl;
        return 1;
    }


    //constexpr int l = 200;
    //int d = 862;

    //std::string nomeFileCSV = "trafficNew";

    if(mode) {

        DynamicMatrix<double> matriceRidotta = frequent_directions::frequentDirectionsStream(l, nomeFileCSV, d, svd);
        opencsv::scriviMatriceSuCSV(matriceRidotta, "sketch_" + nomeFileCSV);

    } else {

        DynamicMatrix<double> matrice = opencsv::leggiCSV(nomeFileCSV);
        DynamicMatrix<double> matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, matrice, svd);
        opencsv::scriviMatriceSuCSV(matriceRidotta, "sketch_" + nomeFileCSV);
        frequent_directions::accuracyTest(matrice, matriceRidotta);

    }

    //DynamicMatrix<double> matrice = opencsv::leggiCSV(nomeFileCSV+".csv");

    //DynamicMatrix<double> matriceRidotta = frequent_directions::frequentDirections<l>(nomeFileCSV+, matrice, svd);
    //opencsv::scriviMatriceSuCSV(matriceRidotta, nomeFileCSV+"Ridotto.csv");

    //frequent_directions::accuracyTest(matrice, matriceRidotta);

    // Registra il tempo di fine
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calcola la durata in millisecondi
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Stampa il tempo di esecuzione
    std::cout << "Tempo di esecuzione: " << duration.count() << " milliseconds" << std::endl;

    return 0;
}
