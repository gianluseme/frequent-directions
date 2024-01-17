//
// Created by semeg on 12/12/2023.
//

#ifndef FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
#define FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <blaze/Blaze.h>


using blaze::DynamicMatrix;
using blaze::DynamicVector;
using blaze::gesvd;
using blaze::gesdd;


class frequent_directions {

public:

    // funzione che implementa l'algoritmo Frequent Directions
    static DynamicMatrix<double> frequentDirections(int l, const std::string& nomeFile, const bool svd, const bool l_value) {

        // apertura del file specificato dalla variabile 'nomeFile' in modalità binaria
        std::ifstream file(nomeFile, std::ios::binary);

        // Array per i primi tre byte del file
        char bom[4] = {0};

        // Legge i primi tre byte
        file.read(bom, 4);

        file.seekg(0, std::ios::beg);  // Posiziona il cursore all'inizio del file

        // BOM per codifica UTF-8
        if (bom[0] == static_cast<char>(0xEF) && bom[1] == static_cast<char>(0xBB) &&
            bom[2] == static_cast<char>(0xBF)) {
            file.ignore(3); // Ignora i primi 3 byte
        }

        if (!file.is_open()) {
            std::cerr << "Errore nell'apertura del file " << nomeFile << ": " << strerror(errno) << std::endl;
            return {};
        }

        // variabile per immagazzinare le righe lette dal file di input
        std::string line;
        std::getline(file, line);

        // input string stream per effettuare il processing sulla riga
        std::istringstream iss(line);

        // variabile per immagazzinare ogni campo nella riga
        std::string field;

        // conteggio delle colonne della matrice di input
        int columnCount = 0;

        // aumenta il conteggio delle colonne per ogni campo rilevato all'interno della prima riga
        while (std::getline(iss, field, ',')) {
            ++columnCount;
        }

        // verifica che il valore vero di l sia <= del numero di colonne della matrice di input
        // se l_value è true, l è rimasto invariato (sto usando il vero valore di l)
        // se l_value è false, l è doppio rispetto al suo valore originale
        if(l_value) { // l
            if (l > columnCount) {
                std::cerr << "l non deve essere più grande del numero di colonne della matrice di input (n. colonne: " << columnCount << ")." << std::endl;
                exit(1);
            }
        } else if(l/2 > columnCount) {  // 2*l
            std::cerr << "l non deve essere più grande del numero di colonne della matrice di input(n. colonne: " << columnCount << ")." << std::endl;
            exit(1);
        }

        // dichiarazione della matrice sketch B inizializzata a zero
        // l righe (2*l se l'algoritmo viene eseguito nella modalità con doppio valore di l), numero di colonne pari a quello della matrice originale
        DynamicMatrix<double> B(l, columnCount, 0.0);

        // dichiarazione del vettore riga che verrà estratto iterativamente dalla matrice di input
        DynamicVector<double> row(columnCount);

        //tiene traccia dell'indice della riga nulla successiva di B
        int nextZeroRow = 0;

        // Leggi una riga alla volta
        while (std::getline(file, line)) {

            // Usa uno stringstream per analizzare la riga CSV
            std::istringstream iss(line);
            std::string value;

            // estrazione della riga corrente dalla matrice di input e assegnazione a row
            for (int j = 0; j < columnCount && std::getline(iss, value, ','); ++j) {
                // Converti il valore da stringa a double e aggiungilo alla riga
                row[j] = std::stod(value);
            }

            // Passa la riga alla funzione appendRow
            appendRow(l, row, B, nextZeroRow, svd);

        }

        // se l'algoritmo è stato eseguito senza l'opzione --l_truesize,
        // è stato usato un numero doppio di righe per la matrice sketch, quindi viene dimezzato il numero di righe prima di effettuare il return
        if(!l_value)
            B = submatrix(B,0UL, 0UL, l/2, B.columns());  //restituisco le prime l righe della matrice

        return B;

    }


    static void appendRow(int l, const DynamicVector<double> &Ai, DynamicMatrix<double> &B, int& nextZeroRow, const bool svd) {

        // se non ci sono più righe vuote, effettua l'operazione di rotazione della matrice con rotateB
        if(nextZeroRow >= l)
            rotateB(l, B, nextZeroRow, svd);

        // aggiungi una nuova riga della matrice di input
        for (size_t j = 0; j < B.columns(); ++j) {
            B(nextZeroRow, j) = Ai[j];
        }

        // aumenta di 1 l'indice della riga nulla successiva
        nextZeroRow += 1;

    }


    static void rotateB(int l, DynamicMatrix<double> &B, int& nextZeroRow, const bool svd) {

        // Allocazione per i valori singolari
        DynamicVector<double> S;

        // Allocazione per la matrice V (trasposta)
        DynamicMatrix<double> V;

        /* In base alla modalità di funzionamento del programma,
         * viene utilizzata la funzione gesvd gesdd per la
         * decomposizione in valori singolari della matrice B
         */

        if(svd)
            gesvd(B, S, V, 'N', 'S');
        else {
            // Allocazione per la matrice U (non utilizzata nel resto dell'algoritmo ma necessaria per la gesdd)
            DynamicMatrix<double> U;
            gesdd(B, U, S, V, 'S');
        }

        int halfl = (l/2) - 1;

        // calcolo di delta (il quadrato dell'l/2-esimo elemento del vettore dei valori singolari S)
        double delta = blaze::pow(S[halfl], 2);

        // quest'operazione riduce i primi l/2 valori singolari, annullando i successivi
        S = blaze::sqrt(blaze::max(S * S - delta, 0.0));

        // matrice  diagonale contentente i valori singolari in ordine decrescente
        DynamicMatrix<double> diagS(S.size(), S.size(), 0.0);
        blaze::diagonal(diagS) = S;

        /*  la rotazione di B viene ultimata con il prodotto diagS*V
            le prime l/2 righe del prodotto diagS*V vengono inserite nelle prime l/2 righe di B
            le righe successive sono impostate a zero
            quest'operazione è equivalente a calcolare B = diagS*V (con diagS avente numero di righe pari a quello di B),
            ma è più efficiente, in quanto coinvolge nel prodotto una matrice diagonale
         */
        submatrix(B, 0UL, 0UL, l / 2, B.columns()) = submatrix(diagS*V, 0UL, 0UL, l / 2, (diagS*V).columns());
        submatrix(B, l / 2, 0UL, B.rows() - l / 2, B.columns()) = 0.0;

        // dopo il prodotto B ha le ultime l/2 righe nulle
        nextZeroRow = halfl;

    }


    // funzione per il calcolo del limite teorico sull'accuratezza della matrice di sketch
    static double boundCalculation(const DynamicMatrix<double>& A, int l) {

        return 2*(blaze::sqrNorm(A))/l;

    }

};


#endif //FREQUENT_DIRECTIONS_FREQUENT_DIRECTIONS_H
