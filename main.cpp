
#define BLAZE_BLAS_MODE 1
#define BLAZE_BLAS_IS_64BIT 1
#define BLAZE_USE_BLAS_MATRIX_MATRIX_MULTIPLICATION 1
#define BLAZE_USE_BLAS_MATRIX_VECTOR_MULTIPLICATION 1
#define BLAZE_BLAS_INCLUDE_FILE "/usr/include/x86_64-linux-gnu/cblas.h"

#include <iostream>
#include <blaze/Math.h>
#include <chrono>
#include "opencsv.h"
#include "frequent_directions.h"
#include <cxxopts.hpp>
#include <filesystem>
#include <cstdlib>


using blaze::DynamicMatrix;


int main(int argc, char* argv[]) {

    // Argomenti: file di input, l (numero di righe della matrice ridotta),
    // parametro per scegliere tra la funzione gesvd e gesdd (default gesdd), modalità di funzionamento (solo riduzione/calcolo accuratezza, default solo riduzione)
    // parametro l_truesize per scegliere se usare il numero di righe l oppure se lavorare con 2l, acctest per effettuare test di accuratezza, bound per calcolare il bound teorico

    // Registra il tempo di inizio dell'esecuzione del programma
    auto start_time = std::chrono::high_resolution_clock::now();

    // numero di righe della matrice sketch
    int l;
    std::string nomeFileCSV;

    // variabile per determinare la funzione per il calcolo SVD (true: gesvd; false: gesdd)
    bool svd;
    std::string svd_value;

    /* variabile per stabilire se il programma deve limitarsi a ridurre la matrice (true) o a calcolarne al contempo l'accuratezza
     e salvare in un file csv il bound, l'accuratezza e il tempo di esecuzione, dato il numero di righe l (false) */
    bool mode;

    // se questa variabile è impostata su true (e se mode è false) l'esecuzione dell'algoritmo frequent directions viene ripetuta per 5 volte
    bool bench = false;

    // se è true, questa variabile abilita la modalità di calcolo del bound del programma (dato l e la matrice di input)
    bool bound = false;

    // variabile che abilita il calcolo dell'accuratezza
    bool accur = false;

    // variabile per determinare il funzionamento in modalità l o 2l (default 2l)
    bool l_size = false;

    // tempo di esecuzione di Frequent Directions
    int timeFd;

    cxxopts::Options options("Frequent Directions", "Descrizione del programma");

    options.add_options()
            ("h,help", "Mostra l'aiuto")
            ("i,input", "File di input (.csv)", cxxopts::value<std::string>())
            ("l", "N. righe della matrice ridotta", cxxopts::value<int>())
            ("svd", "Scelta tra funzioni gesvd e gesdd per il calcolo SVD nell'algoritmo", cxxopts::value<std::string>()->default_value("gesdd"))
            ("mode", "Modalità (solo riduzione/accuracy test)", cxxopts::value<std::string>()->default_value("ronly"))
            ("bench", "Abilita la modalità di benchmark (esegue più volte l'algoritmo)")
            ("bound", "Calcola il bound dell'algoritmo (dato l)")
            ("acctest", "Esegui il test di accuratezza (dato l)")
            ("l_truesize", "Scegli se eseguire l'algoritmo usando il numero originale di righe per la matrice sketch");

    try {

        // parsing degli argomenti da linea di comando

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
        if(l <= 0) {
            std::cerr << "Valore non valido per l." << std::endl;
            return 1;
        }

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

    // variabile di conversione a string di l
    std::string lString = std::to_string(l);

    if(bound) {  // modalità di calcolo del bound

        // memorizza la matrice leggendo il file csv definito in input tramite apposita funzione
        DynamicMatrix<double> matrice = opencsv::leggiCSV(nomeFileCSV);

        // stampa del bound calcolato
        std::cout << "Bound: " << frequent_directions::boundCalculation(matrice, l) << std::endl;

        // Registra il tempo di fine dell'esecuzione del programma
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        // Stampa il tempo di esecuzione
        std::cout << "Tempo di esecuzione: " << time << " millisecondi" << std::endl;

    } else

        if(accur) {  // modalità di stampa dell'accuratezza

            std::string l_value;

            // l_value è pari a 2 se l'algoritmo sta usando il doppio delle righe per la matrice sketch, altrimenti è 1

            if(!l_size)
                l_value = "2";
            else
                l_value = "1";

            // viene eseguito uno script python che calcola l'accuratezza e la stampa in output
            std::string command = "python3 accuracy_calc.py " + lString + " 0 "+ svd_value + " "  + nomeFileCSV + " " + l_value + " 2";
            system(command.c_str());

            // Registra il tempo di fine dell'esecuzione completa del programma
            auto end_time = std::chrono::high_resolution_clock::now();

            // Calcola la durata in millisecondi
            auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            // Stampa il tempo di esecuzione
            std::cout << "Tempo di esecuzione: " << time << " millisecondi" << std::endl;

        } else

    if(mode) {       // modalità ronly (effettua solo la riduzione della matrice)

        // variabile path per l'eventuale creazione della directory dei risultati
        std::filesystem::path filePath;

        DynamicMatrix<double> matriceRidotta;

        // in base alla modalità di esecuzione (2l o l), viene definito il percorso in cui verrà salvata la matrice sketch
        if(!l_size)
            filePath = "./results/"+svd_value+"/sketch_l" + lString + "_" + nomeFileCSV; //2l
        else filePath = "./results/"+svd_value+"/sketch_1_l" + lString + "_" + nomeFileCSV; //l

        // Verifica e crea le directory se non esistono
        if (!std::filesystem::exists(filePath.parent_path())) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        auto start_timeFd = std::chrono::high_resolution_clock::now();

        // chiamata alla funzione frequent directions con 2l o l per il numero di righe della matrice sketch
        if(!l_size)
            matriceRidotta = frequent_directions::frequentDirections(2 * l, nomeFileCSV, svd, l_size);
        else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

        // Registra il tempo di fine dell'esecuzione di frequent directions
        auto end_timeFd = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi del tempo di esecuzione di frequent directions
        timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

        // scrittura della matrice sketch su un file csv alla directory definita da filePath
        opencsv::scriviMatriceSuCSV(matriceRidotta, filePath);

        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi del tempo di esecuzione totale del programma
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione totale
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;

    } else {   // modalità calcolo di accuratezza

        DynamicMatrix<double> matriceRidotta;

        if(bench) {   // ripete l'esecuzione dell'algoritmo 5 volte per ottenere una stima più accurata sul tempo di esecuzione

            // std::vector per salvare i tempi di esecuzione dell'algoritmo
            std::vector<int> timesFd;

            for (int i = 0; i < 5; i++) {

                // Registra il tempo di inizio dell'esecuzione di frequent directions
                auto start_timeFd = std::chrono::high_resolution_clock::now();

                // chiamata alla funzione frequent directions con 2l o l per il numero di righe della matrice sketch
                if(!l_size)
                    matriceRidotta = frequent_directions::frequentDirections(2*l, nomeFileCSV, svd, l_size);
                else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

                // Registra il tempo di fine dell'esecuzione di frequent directions
                auto end_timeFd = std::chrono::high_resolution_clock::now();

                // Calcola la durata in millisecondi
                timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

                // salvataggio del tempo di esecuzione in timesFd
                timesFd.push_back(timeFd);

            }

            int sum = 0;
            for(const auto& element : timesFd)
                sum += element;

            // salvataggio della media dei valori di timesFd in timeFd
            timeFd = timesFd.empty() ? 0 : sum / timesFd.size();

        } else {

            auto start_timeFd = std::chrono::high_resolution_clock::now();

            // chiamata alla funzione frequent directions con 2l o l per il numero di righe della matrice sketch
            if(!l_size)
                matriceRidotta = frequent_directions::frequentDirections(2*l, nomeFileCSV, svd, l_size);
            else matriceRidotta = frequent_directions::frequentDirections(l, nomeFileCSV, svd, l_size);

            // Registra il tempo di fine dell'esecuzione di frequent directions
            auto end_timeFd = std::chrono::high_resolution_clock::now();

            // Calcola la durata in millisecondi
            timeFd = std::chrono::duration_cast<std::chrono::milliseconds>(end_timeFd - start_timeFd).count();

        }

        // variabile path per l'eventuale creazione della directory dei risultati
        std::filesystem::path filePath;

        // in base alla modalità di esecuzione (2l o l), viene definito il percorso in cui verrà salvata la matrice sketch
        if(!l_size)
            filePath = "./results/"+svd_value+"/sketch_l" + lString + "_" + nomeFileCSV;
        else filePath = "./results/"+svd_value+"/sketch_1_l" + lString + "_" + nomeFileCSV;

        // Verifica e crea le directory se non esistono
        if (!std::filesystem::exists(filePath.parent_path())) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        // scrittura della matrice sketch su un file csv alla directory definita da filePath
        opencsv::scriviMatriceSuCSV(matriceRidotta, filePath);

        std::string l_value;

        // l_value è pari a 2 se l'algoritmo sta usando il doppio delle righe per la matrice sketch, altrimenti è 1

        // in base alla modalità di esecuzione (2l o l), viene definito il percorso in cui verrà salvata la matrice sketch
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

        /* esecuzione di uno script python che salva i risultati dell'esecuzione dell'algoritmo (dato l, il file csv di input e la modalità di esecuzione l/2l):
            - accuratezza, che viene calcolata all'interno dell'algoritmo;
            - bound
            - tempo di esecuzione
         */

        std::string command = "python3 accuracy_calc.py " + lString + " " + std::to_string(timeFd) + " " + svd_value + " " + nomeFileCSV + " " + l_value + " 1";

        system(command.c_str());

        // Registra il tempo di fine dell'esecuzione completa del programma
        auto end_time = std::chrono::high_resolution_clock::now();

        // Calcola la durata in millisecondi
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Tempo di esecuzione di Frequent Directions: " << timeFd << " millisecondi" << std::endl;

        // Stampa il tempo di esecuzione
        std::cout << "Tempo totale di esecuzione: " << duration.count() << " millisecondi" << std::endl;


    }


    return 0;
}
