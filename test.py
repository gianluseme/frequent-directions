import subprocess
import time
import sys
import pandas as pd

if len(sys.argv) < 2:
    print("Usage: python test.py <l_size> [<file1> [<file2> [<file3>]]]")
    sys.exit(1)

l_size = int(sys.argv[1])

# Percorso del tuo programma C++
percorso_programma_cpp = "./frequent_directions"

# Lista dei file dai parametri della linea di comando
file_paths = sys.argv[2:]

# Funzione per ottenere dinamicamente la lista l_values in base al numero di colonne del file CSV
def get_l_values(num_columns):
    if num_columns <= 100:
        base_list = [i for i in range(10, num_columns + 1, 10)]
        remainder = num_columns % 10
        if remainder > 0:
            base_list.append(num_columns)
        return base_list
    elif num_columns <= 1000:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, num_columns + 1, 100)]
        remainder = num_columns % 100
        if remainder > 0:
            base_list.append(num_columns)
        return base_list
    elif num_columns <= 2000:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, 1001, 100)] + [i for i in range(1200, num_columns + 1, 200)]
        remainder = num_columns % 100
        if remainder > 0:
            base_list.append(num_columns)
        return base_list
    else:
        base_list = [i for i in range(10, 101, 10)] + [i for i in range(100, 1001, 100)] + [i for i in range(1200, 2001, 200)] + [i for i in range(2200, num_columns + 1, 200)]
        remainder = num_columns % 100
        if remainder > 0:
            base_list.append(num_columns)
        return base_list

# Funzione per eseguire il programma C++
def esegui_programma(file_input, l, svd_option):
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

# Funzione per eseguire il programma per tutti i file e valori di l
def esegui_programma_per_tutti_i_file():
    start_time = time.time()  # Registra il tempo di inizio
    for svd_value in ["gesvd", "gesdd"]:
        for file_input in file_paths:
            if file_input:
                try:
                    df = pd.read_csv(file_input)
                    num_columns = len(df.columns)
                    l_values = get_l_values(num_columns)
                    df = None  # Elimina il DataFrame per liberare memoria
                    for l_value in l_values:
                        print(f"Esecuzione per l = {l_value} su {file_input}...")
                        esegui_programma(file_input, l_value, svd_value)
                except Exception as e:
                    print(f"Errore nella lettura del file {file_input}: {e}")
                    sys.exit(1)

    end_time = time.time()  # Registra il tempo di fine
    elapsed_time = end_time - start_time
    print(f"Tempo totale di esecuzione: {elapsed_time:.2f} secondi")

# Esegui il programma per tutti i file e valori di l
esegui_programma_per_tutti_i_file()

print("Esecuzioni completate.")
