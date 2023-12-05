#include <iostream>
#include <fstream>
#include <sstream>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;

class FrequentDirections {
public:
    FrequentDirections(int n, int k) : n(n), k(k), U(MatrixXd::Random(n, k)) {}

    // Aggiunge una colonna alla matrice
    void addColumn(VectorXd& a) {
        MatrixXd Q = U.transpose() * a;

        for (int j = 0; j < k; ++j) {
            U.col(j) -= (Q(j) / (1 + Q(j) * Q(j))) * a;
        }

        // Normalizza le colonne di U
        for (int j = 0; j < k; ++j) {
            U.col(j).normalize();
        }
    }

    // Restituisce la matrice basata su Frequent Directions
    MatrixXd getMatrix() const {
        return U * U.transpose();
    }

    // Restituisce la matrice U
    MatrixXd getU() const {
        return U;
    }

private:
    int n;  // Dimensione della matrice
    int k;  // Parametro k, rango basso desiderato
    MatrixXd U;  // Matrice delle direzioni frequenti
};

MatrixXd readMatrixFromCSV(const string& filename, int rows, int cols) {
    MatrixXd matrix(rows, cols);

    ifstream file(filename);
    if (file.is_open()) {
        string line;
        for (int i = 0; i < rows; ++i) {
            if (getline(file, line)) {
                stringstream ss(line);
                string cell;
                for (int j = 0; j < cols; ++j) {
                    if (getline(ss, cell, ',')) {
                        matrix(i, j) = stod(cell);
                    } else {
                        cerr << "Errore durante la lettura del file CSV: troppo pochi elementi sulla riga " << i + 1 << endl;
                        return {};
                    }
                }
            } else {
                cerr << "Errore durante la lettura del file CSV: troppo poche righe" << endl;
                return {};
            }
        }
        file.close();
    } else {
        cerr << "Impossibile aprire il file: " << filename << endl;
        return {};
    }

    return matrix;
}

int main1() {
    // Esempio di utilizzo
    int n = 5;  // Dimensione della matrice
    int k = 2;  // Rango basso desiderato

    // Creazione di un oggetto FrequentDirections
    FrequentDirections fd(n, k);

    // Lettura della matrice da un file CSV
    MatrixXd inputMatrix = readMatrixFromCSV("traffic.csv", n, n);

    // Aggiunta delle colonne dalla matrice di input
    for (int i = 0; i < n; ++i) {
        VectorXd column = inputMatrix.col(i);
        fd.addColumn(column);

        // Stampa la matrice U dopo l'aggiunta di ogni colonna
        cout << "Matrice U dopo aggiunta colonna " << i + 1 << ":\n" << fd.getU() << endl;
    }

    // Ottenere la matrice basata su Frequent Directions
    MatrixXd resultMatrix = fd.getMatrix();

    // Stampare la matrice risultante
    cout << "Matrice risultante:\n" << resultMatrix << endl;

    return 0;
}
