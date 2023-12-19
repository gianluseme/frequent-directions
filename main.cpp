
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
#include <filesystem>



using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::rowMajor;
using blaze::columnVector;
using blaze::gesvd;
using blaze::StaticMatrix;
using blaze::gesdd;

int main(int argc, char* argv[]) {

    // Argomenti: file di input, l (numero di righe della matrice ridotta),
    // parametro per scegliere tra la funzione gesvd e gesdd (default gesvd), modalità di funzionamento (solo riduzione/calcolo accuratezza, default solo riduzione)

    // Registra il tempo di inizio
    auto start_time = std::chrono::high_resolution_clock::now();

    int l;
    std::string nomeFileCSV;
    bool svd;
    bool mode;
    bool bench = false;

    int timeFd;

    cxxopts::Options options("Frequent Directions", "Descrizione del programma");

    options.add_options()
            ("h,help", "Mostra l'aiuto")
            ("i,input", "File di input (.csv)", cxxopts::value<std::string>())
            ("l", "N. righe della matrice ridotta", cxxopts::value<int>())
            ("svd", "Scelta tra gesvd e gesdd", cxxopts::value<std::string>()->default_value("gesvd"))
            ("mode", "Modalità (solo riduzione/accuracy test)", cxxopts::value<std::string>()->default_value("ronly"))
            ("bench", "Abilia la modalità di benchmark");

    try {

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        if (!result.count("input") || !result.count("l")) {
            std::cerr << "Errore: Le opzioni --input e -l sono obbligatorie." << std::endl;
            return 1;
        }

        nomeFileCSV = result["input"].as<std::string>();
        l = result["l"].as<int>();

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

        if(result.count("bench"))
            bench = true;


    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Errore nell'analisi degli argomenti: " << e.what() << std::endl;
        return 1;
    }

    std::ostringstream convertitore;
    convertitore << l;
    std::string lString = convertitore.str();


    if(mode) {

        auto start_timeFd = std::chrono::high_resolution_clock::now();

        DynamicMatrix<double> matriceRidotta = frequent_directions::frequentDirectionsStream(l, nomeFileCSV, svd);

        auto end_timeFd = std::chrono::high_resolution_clock::now();

        timeFd = std::chrono::duration_cast<std::chrono::milliseconds >(end_timeFd - start_timeFd).count();

        if(svd)
            opencsv::scriviMatriceSuCSV(matriceRidotta, "./results/gesvd/sketch_l" + lString + "_" + nomeFileCSV);
        else
            opencsv::scriviMatriceSuCSV(matriceRidotta, "./results/gesdd/sketch_l" + lString + "_" + nomeFileCSV);

        // Registra il tempo di fine
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;

    } else {

        DynamicMatrix<double> matrice = opencsv::leggiCSV(nomeFileCSV);
        DynamicMatrix<double> matriceRidotta;

        if(bench) {

            std::vector<int> timesFd;

            for(int i = 0; i < 10; i++) {

                auto start_timeFd = std::chrono::high_resolution_clock::now();

                matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, matrice, svd);

                auto end_timeFd = std::chrono::high_resolution_clock::now();

                timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

                timesFd.push_back(timeFd);

            }

            double sum = 0.0;
            for(const auto& element : timesFd)
                sum += element;

            timeFd = timesFd.empty() ? 0.0 : sum / timesFd.size();

        } else {

            auto start_timeFd = std::chrono::high_resolution_clock::now();

            matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, matrice, svd);

            auto end_timeFd = std::chrono::high_resolution_clock::now();

            timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

        }


        if(svd)
            opencsv::scriviMatriceSuCSV(matriceRidotta, "./results/gesvd/sketch_l" + lString + "_" + nomeFileCSV);
        else
            opencsv::scriviMatriceSuCSV(matriceRidotta, "./results/gesdd/sketch_l" + lString + "_" + nomeFileCSV);

        const double accuracy = frequent_directions::accuracyTest(matrice, matriceRidotta);

        // Registra il tempo di fine
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;

        if(svd) {

            std::filesystem::path filePath = "./results/gesvd/list/results_list_" + nomeFileCSV;

            // Verifica e crea le directory se non esistono
            if (!std::filesystem::exists(filePath.parent_path())) {
                std::filesystem::create_directories(filePath.parent_path());
            }

            opencsv::appendCSV(l, timeFd, duration.count(), accuracy,filePath);

        } else {

            std::filesystem::path filePath = "./results/gesdd/list/results_list_" + nomeFileCSV;

            // Verifica e crea le directory se non esistono
            if (!std::filesystem::exists(filePath.parent_path())) {
                std::filesystem::create_directories(filePath.parent_path());
            }

            opencsv::appendCSV(l, timeFd, duration.count(), accuracy,filePath);

        }



    }


    return 0;
}
