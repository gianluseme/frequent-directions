import subprocess
import csv
import time
import sys


# Verifica il numero corretto di argomenti dalla riga di comando
if len(sys.argv) < 2 or len(sys.argv) > 4:
    print("Usage: python test.py [<file1> [<file2> [<file3>]]]")
    sys.exit(1)

percorso_programma_cpp = "./frequent_directions"

# Lista dei file dai parametri della linea di comando
file_paths = sys.argv[1:]

# Funzione per ottenere dinamicamente la lista l_values in base al numero di colonne del file CSV
def get_l_values(num_columns):
    # divisione l'intervallo dei valori di l in base al numero di colonne
    if num_columns <= 100:
        base_list = [i for i in range(10, num_columns + 1, 10)]
    elif num_columns <= 1000:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, num_columns + 1, 100)]
    elif num_columns <= 2000:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, 1001, 100)] + [i for i in range(1200, num_columns + 1, 200)]
    else:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, 1001, 100)] + [i for i in range(1200, 2001, 200)] + [i for i in range(2200, num_columns + 1, 200)]

    # Aggiungi l'eventuale resto al valore massimo di num_columns
    if (num_columns % 10) > 0:
        base_list.append(num_columns)

    return base_list


# funzione per leggere il numero di colonne di un file csv
def get_num_columns(file_path):
    with open(file_path, 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        first_row = next(reader)
        num_columns = len(first_row)
    return num_columns


# Funzione per eseguire il programma C++
def esegui_programma(file_input, l, svd_option, l_size):
    comando = [
        percorso_programma_cpp,
        "--input", file_input,
        "-l", str(l),
        "--svd", svd_option,
        "--mode", "acc",
        "--bench"
    ]
    if l_size == 1:
        comando.append("--l_truesize")
    subprocess.run(comando, check=True)

# Funzione per eseguire il programma per tutti i file, i valori di l e modalità di esecuzione
def esegui_programma_per_tutti_i_file():
    start_time = time.time()  # Registra il tempo di inizio
    for file_input in file_paths:
        for svd_value in ["gesvd", "gesdd"]:
            if file_input:
                try:
                    num_columns = get_num_columns(file_input)
                    l_values = get_l_values(num_columns)
                    for l_value in l_values:
                        print(f"Esecuzione per l = {l_value} su {file_input}, SVD: ({svd_value})...")
                        esegui_programma(file_input, l_value, svd_value, 0)
                        esegui_programma(file_input, l_value, svd_value, 1)
                except Exception as e:
                    print(f"Errore nella lettura del file {file_input}: {e}")
                    sys.exit(1)

        # esecuzione dello script python per generare i grafici per il file considerato
        subprocess.run(["python3", "plot.py", file_input])

    end_time = time.time()  # Registra il tempo di fine
    elapsed_time = end_time - start_time
    print(f"Tempo totale di esecuzione: {elapsed_time:.2f} secondi")

esegui_programma_per_tutti_i_file()

print("Esecuzioni completate.")
