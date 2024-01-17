import sys
import pandas as pd
import numpy as np
import os

# 6 argomenti (nome dello script, valore di l, tempo di esecuzione dell'algoritmo, funzione svd,
# nome del file, dimensione di l (normale o doppia), modalità di funzionamento dello script)
if len(sys.argv) < 6:
    print("Usage: python script.py <l_value> <timeFd_value> <svd> <fileName> <lsize> <accmode>")
    sys.exit(1)

# Ottieni il valore di l dalla riga di comando
l_value = int(sys.argv[1])          # Valore di 'l'

timeFd_value = float(sys.argv[2])   # Tempo impiegato dall'algoritmo frequent directions

svd = str(sys.argv[3])              # Funzione per SVD utilizzata

nomeFileCSV = str(sys.argv[4])      # Nome del file CSV

lsize = str(sys.argv[5])            # Dimensione di 'l' (1 o 2x)

accmode = str(sys.argv[6])          # Modalità di output (1 per scrivere su file, 2 per stampare su terminale)


# costruzione del percorso del file risultante in base a 'lsize' e 'svd'
if lsize == '2':
    filePath = './results/'+svd+'/list/results_' + nomeFileCSV
if lsize == '1':
    filePath = './results/'+svd+'/list/results_1_' + nomeFileCSV

# lettura del file csv originale
df = pd.read_csv(nomeFileCSV, header=None)
A = df.values

# Verifica se il file esiste o è vuoto
if os.path.exists(filePath) and os.stat(filePath).st_size > 0:
    # Leggi il file CSV esistente
    df_result = pd.read_csv(filePath)
else:
    # Crea un DataFrame vuoto con le colonne etichettate
    df_result = pd.DataFrame(columns=['l', 'timeFd', 'bound', 'accuracy'])

# percorso della matrice di sketch selezionata in base al numero di righe l, dalla funzione SVD utilizzata
# e dalla modalità di funzionamento (l o 2xl righe)
if lsize == '2':
    file_path = './results/'+svd+'/sketch_l'+ str(l_value) + "_" + nomeFileCSV
if lsize == '1':
    file_path = './results/'+svd+'/sketch_1_l'+ str(l_value) + "_" + nomeFileCSV

# lettura della matrice sketch
df1 = pd.read_csv(file_path, header=None)
B = df1.values

# calcolo l'accuratezza
diff = A.T @ A - B.T @ B
accuracy = np.linalg.norm(diff, ord=2)

# modalità scrittura su file
if accmode == '1':

    # norma di Frobenius della matrice originale
    norm_frobenius_A = np.linalg.norm(A, ord='fro')

    # Cerca una riga con lo stesso valore di 'l'
    existing_row = df_result[df_result['l'] == l_value]

    if not existing_row.empty:
        # Se trovi una riga con lo stesso valore di 'l', sostituisci i dati
        # il limite teorico sull'accuratezza dato dall'algoritmo è calcolato come 2 * (norm_frobenius_A ** 2) / l_value
        df_result.loc[existing_row.index, ['timeFd', 'bound', 'accuracy']] = [timeFd_value, 2 * (norm_frobenius_A ** 2) / l_value, accuracy]
    else:
        # Aggiungi le nuove informazioni al DataFrame
        # il limite teorico sull'accuratezza dato dall'algoritmo è calcolato come 2 * (norm_frobenius_A ** 2) / l_value
        new_row = pd.DataFrame({'l': [l_value], 'timeFd': [timeFd_value], 'bound': [2 * (norm_frobenius_A ** 2) / l_value], 'accuracy': [accuracy]})
        df_result = pd.concat([df_result, new_row], ignore_index=True)

    # Ordina il DataFrame in base alla colonna 'l'
    df_result = df_result.sort_values(by='l')

    # Formatta il DataFrame per scrivere solo le ultime tre cifre decimali
    df_result['accuracy'] = df_result['accuracy'].apply(lambda x: round(x, 3))
    df_result['bound'] = df_result['bound'].apply(lambda x: round(x, 3))

    # Salva il DataFrame come file CSV
    df_result.to_csv(filePath, index=False, header=True)


# stampa su terminale il risultato dell'accuratezza ottenuto
print('Accuracy: ', accuracy)
