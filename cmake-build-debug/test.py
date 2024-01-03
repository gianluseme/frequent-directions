import subprocess
import time

# Percorso del tuo programma C++
percorso_programma_cpp = "./frequent_directions"

# Definisci i file di diverse dimensioni
piccolo_file = "smallMatrix.csv"
file_di_piccole_dimensioni = "smallMatrix1.csv"
file_di_medie_dimensioni = "mediumMatrix1.csv"
file_di_grandi_dimensioni = "largeMatrix1.csv"

# Definisci i valori di l da considerare
l_values = [10, 20, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 400, 500, 750, 1000, 2000, 3000, 4000, 5000, 7500, 10000]

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
    subprocess.run(comando, check=True)

# Funzione per eseguire il programma per tutti i file e valori di l
def esegui_programma_per_tutti_i_file():
    start_time = time.time()  # Registra il tempo di inizio
    for svd_value in ["gesvd", "gesdd"]:
        for file_input in [file_di_piccole_dimensioni, file_di_medie_dimensioni, file_di_grandi_dimensioni]:
            for l_value in l_values:
                print(f"Esecuzione per l = {l_value} su {file_input}...")
                esegui_programma(file_input, l_value, svd_value)
    end_time = time.time()  # Registra il tempo di fine
    elapsed_time = end_time - start_time
    print(f"Tempo totale di esecuzione: {elapsed_time:.2f} secondi")

# Esegui il programma per tutti i file e valori di l
esegui_programma_per_tutti_i_file()



print("Esecuzioni completate.")
