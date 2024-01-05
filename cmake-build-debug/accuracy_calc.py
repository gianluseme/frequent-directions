import sys
import pandas as pd
import numpy as np
import os

# Assicurati di avere almeno 2 argomenti (nome dello script + valore di l)
if len(sys.argv) < 5:
    print("Usage: python script.py <l_value> <timeFd_value> <svd> <fileName>")
    sys.exit(1)

# Ottieni il valore di l dalla riga di comando
l_value = int(sys.argv[1])

timeFd_value = float(sys.argv[2])

svd = str(sys.argv[3])

nomeFileCSV = str(sys.argv[4])

filePath = './results/'+svd+'/list/results_' + nomeFileCSV

df = pd.read_csv(nomeFileCSV, header=None)
A = df.values

# Verifica se il file esiste o è vuoto
if os.path.exists(filePath) and os.stat(filePath).st_size > 0:
    # Leggi il file CSV esistente
    df_result = pd.read_csv(filePath)
else:
    # Crea un DataFrame vuoto con le colonne etichettate
    df_result = pd.DataFrame(columns=['l', 'timeFd', 'bound', 'accuracy'])

# Calcola la norma di Frobenius
norm_frobenius = np.linalg.norm(A, ord='fro')


file_path = './results/'+svd+'/sketch_l'+ str(l_value) + "_" + nomeFileCSV

df1 = pd.read_csv(file_path, header=None)
B = df1.values

# Calcola la norma di Frobenius
norm_frobenius_A = np.linalg.norm(A, ord='fro')

# Calcola l'accuracy
diff = A.T @ A - B.T @ B
accuracy = np.linalg.norm(diff, ord=2)

# Cerca una riga con lo stesso valore di 'l'
existing_row = df_result[df_result['l'] == l_value]

if not existing_row.empty:
    # Se trovi una riga con lo stesso valore di 'l', sostituisci i dati
    df_result.loc[existing_row.index, ['timeFd', 'bound', 'accuracy']] = [timeFd_value, 2 * (norm_frobenius_A ** 2) / l_value, accuracy]
else:
    # Aggiungi le nuove informazioni al DataFrame
    new_row = pd.DataFrame({'l': [l_value], 'timeFd': [timeFd_value], 'bound': [2 * (norm_frobenius_B ** 2) / l_value], 'accuracy': [accuracy]})
    df_result = pd.concat([df_result, new_row], ignore_index=True)

# Ordina il DataFrame in base alla colonna 'l'
df_result = df_result.sort_values(by='l')

# Formatta il DataFrame per scrivere solo le ultime tre cifre decimali
df_result['accuracy'] = df_result['accuracy'].apply(lambda x: round(x, 3))
df_result['bound'] = df_result['bound'].apply(lambda x: round(x, 3))

# Salva il DataFrame come file CSV
df_result.to_csv(filePath, index=False, header=True)
