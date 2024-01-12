
#define BLAZE_BLAS_MODE 1
#define BLAZE_BLAS_IS_64BIT 1
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
#include <cstdlib>



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
    std::string svd_value;
    bool mode;
    bool bench = false;
    bool bound = false;
    bool accur = false;
    bool l_size = false; // usa 2l di default

    int timeFd;

    cxxopts::Options options("Frequent Directions", "Descrizione del programma");

    options.add_options()
            ("h,help", "Mostra l'aiuto")
            ("i,input", "File di input (.csv)", cxxopts::value<std::string>())
            ("l", "N. righe della matrice ridotta", cxxopts::value<int>())
            ("svd", "Scelta tra gesvd e gesdd", cxxopts::value<std::string>()->default_value("gesvd"))
            ("mode", "Modalità (solo riduzione/accuracy test)", cxxopts::value<std::string>()->default_value("ronly"))
            ("bench", "Abilia la modalità di benchmark")
            ("bound", "Calcola il bound dell'algoritmo (dato l)")
            ("acctest", "Esegui il test di accuratezza (dato l)")
            ("l_truesize", "Scegli se eseguire l'algoritmo usando 2l o l");

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

        if(result["svd"].as<std::string>() == "gesvd") {
            svd = true;
            svd_value = "gesvd";
        }
        else if(result["svd"].as<std::string>() == "gesdd") {
                svd = false;
                svd_value = "gesdd";
            }
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

        if(result.count("l_truesize"))
            l_size = true;

        if(result.count("bench"))
            bench = true;

        if(result.count("bound"))
            bound = true;

        if(result.count("acctest"))
            accur = true;


    } catch (const cxxopts::exceptions::parsing& e) {
        std::cerr << "Errore nell'analisi degli argomenti: " << e.what() << std::endl;
        return 1;
    }

    std::ostringstream convertitore;
    convertitore << l;
    std::string lString = convertitore.str();


    if(bound) {

        auto start_time = std::chrono::high_resolution_clock::now();

        DynamicMatrix<double> matrice = opencsv::leggiCSV(nomeFileCSV);

        std::cout << "Bound: " << frequent_directions::boundCalculation(matrice, l) << std::endl;

        auto end_time = std::chrono::high_resolution_clock::now();

        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        std::cout << "Tempo di esecuzione: " << time << " millisecondi" << std::endl;

    } else

        if(accur) {

            auto start_time = std::chrono::high_resolution_clock::now();

            std::string l_value;

            if(!l_size)
                l_value = "2";
            else
                l_value = "1";

            std::string command = "python3 accuracy_calc.py " + lString + " 0 "+ svd_value + " "  + nomeFileCSV + " " + l_value + " 2";
            system(command.c_str());

            auto end_time = std::chrono::high_resolution_clock::now();
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            std::cout << "Tempo di esecuzione: " << time << " millisecondi" << std::endl;

        } else

    if(mode) {       // modalità ronly (effettua solo la riduzione della matrice)

        std::filesystem::path filePath;

        DynamicMatrix<double> matriceRidotta;

        if(!l_size)
            filePath = "./results/"+svd_value+"/sketch_l" + lString + "_" + nomeFileCSV;
        else filePath = "./results/"+svd_value+"/sketch_1_l" + lString + "_" + nomeFileCSV;

        // Verifica e crea le directory se non esistono
        if (!std::filesystem::exists(filePath.parent_path())) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        auto start_timeFd = std::chrono::high_resolution_clock::now();

        if(!l_size)
            matriceRidotta = frequent_directions::frequentDirections(2 * l, nomeFileCSV, svd, l_size);
        else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

        auto end_timeFd = std::chrono::high_resolution_clock::now();

        timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

        opencsv::scriviMatriceSuCSV(matriceRidotta, filePath);

        // Registra il tempo di fine
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;

    } else { //modalità calcolo di accuratezza

        DynamicMatrix<double> matriceRidotta;

        if(bench) {   // ripete l'esecuzione dell'algoritmo 5 volte per ottenere una stima più accurata sul tempo di esecuzione

            std::vector<int> timesFd;

            for (int i = 0; i < 5; i++) {

                auto start_timeFd = std::chrono::high_resolution_clock::now();

                if(!l_size)
                    matriceRidotta = frequent_directions::frequentDirections(2*l, nomeFileCSV, svd, l_size);
                else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

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

            if(!l_size)
                matriceRidotta = frequent_directions::frequentDirections(2*l, nomeFileCSV, svd, l_size);
            else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

            auto end_timeFd = std::chrono::high_resolution_clock::now();

            timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

        }

        std::filesystem::path filePath;

        if(!l_size)
            filePath = "./results/"+svd_value+"/sketch_l" + lString + "_" + nomeFileCSV;
        else filePath = "./results/"+svd_value+"/sketch_1_l" + lString + "_" + nomeFileCSV;

        // Verifica e crea le directory se non esistono
        if (!std::filesystem::exists(filePath.parent_path())) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        opencsv::scriviMatriceSuCSV(matriceRidotta, filePath);


        //const double accuracy = frequent_directions::accuracyTest(matrice, matriceRidotta);

        std::string l_value;

        if(!l_size) {
            filePath = "./results/"+svd_value+"/list/results_" + nomeFileCSV;
            l_value = "2";
        }
        else {
            filePath = "./results/"+svd_value+"/list/results_1_" + nomeFileCSV;
            l_value = "1";
        }

        // Verifica e crea le directory se non esistono
        if (!std::filesystem::exists(filePath.parent_path())) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        //opencsv::appendCSV(l, timeFd, filePath);

        std::string command = "python3 accuracy_calc.py " + lString + " " + std::to_string(timeFd) + " " + svd_value + " " + nomeFileCSV + " " + l_value + " 1";

        system(command.c_str());

        // Registra il tempo di fine
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;


    }


    return 0;
}
