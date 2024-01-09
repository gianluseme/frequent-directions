import subprocess
import time
import sys

if len(sys.argv) < 1:
    print("Usage: python test.py <l_size>")
    sys.exit(1)

l_size = int(sys.argv[1])

# Percorso del tuo programma C++
percorso_programma_cpp = "./frequent_directions"

# Definisci i file di diverse dimensioni
file_di_piccole_dimensioni = "smallMatrix1.csv"
file_di_medie_dimensioni = "mediumMatrix1.csv"
file_di_grandi_dimensioni = "largeMatrix1.csv"

# Definisci i valori di l da considerare
l_values_3 = [10, 20, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 400, 500, 750, 1000, 2000]

l_values_1 = [10, 20, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200]

l_values_2 = [10, 20, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 400, 500, 750, 1000]

# Funzione per eseguire il programma C++


def esegui_programma(file_input, l, svd_option):
    if l_size == 1:
        comando = [
            percorso_programma_cpp,
            "--input", file_input,
            "-l", str(l),
            "--svd", svd_option,
            "--mode", "acc",
            "--bench",
            "--l_truesize"
        ]
    if l_size == 2:
        comando = [
            percorso_programma_cpp,
            "--input", file_input,
            "-l", str(l),
            "--svd", svd_option,
            "--mode", "acc",
            "--bench"
        ]
    subprocess.run(comando, check=True)

# Funzione per eseguire il programma per tutti i file e valori di l
def esegui_programma_per_tutti_i_file():
    start_time = time.time()  # Registra il tempo di inizio
    for svd_value in ["gesvd", "gesdd"]:
        for file_input in [file_di_piccole_dimensioni, file_di_medie_dimensioni, file_di_grandi_dimensioni]:
            if file_input == 'smallMatrix1.csv':
                for l_value in l_values_1:
                    print(f"Esecuzione per l = {l_value} su {file_input}...")
                    esegui_programma(file_input, l_value, svd_value)

            if file_input == 'mediumMatrix1.csv':
                for l_value in l_values_1:
                    print(f"Esecuzione per l = {l_value} su {file_input}...")
                    esegui_programma(file_input, l_value, svd_value)

            if file_input == 'largeMatrix1.csv':
                for l_value in l_values_3:
                    print(f"Esecuzione per l = {l_value} su {file_input}...")
                    esegui_programma(file_input, l_value, svd_value)

    end_time = time.time()  # Registra il tempo di fine
    elapsed_time = end_time - start_time
    print(f"Tempo totale di esecuzione: {elapsed_time:.2f} secondi")

# Esegui il programma per tutti i file e valori di l
esegui_programma_per_tutti_i_file()



print("Esecuzioni completate.")
