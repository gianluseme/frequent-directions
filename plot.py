import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys


def plot_results(file_path):

    df_1 = pd.read_csv(f'./results/gesdd/list/results_{file_path}')
    df_2 = pd.read_csv(f'./results/gesvd/list/results_{file_path}')

    # Trova l'intersezione dei valori di l
    common_l_values = set(df_1['l']).intersection(df_2['l'])

    # Filtra i DataFrame solo con valori di l comuni
    df_1_common = df_1[df_1['l'].isin(common_l_values)]
    df_2_common = df_2[df_2['l'].isin(common_l_values)]

    # Inizializza i dizionari direttamente dai DataFrame filtrati
    time_data = {'l': df_1_common['l'].tolist(),
                 'timeFd (gesdd)': df_1_common['timeFd'].tolist(),
                 'timeFd (gesvd)': df_2_common['timeFd'].tolist()}

    bound_accuracy_data = {'l': df_1_common['l'].tolist(),
                           'bound': df_1_common['bound'].tolist(),
                           'accuracy': df_1_common['accuracy'].tolist()}

    memory_usage_data = {'l': df_1_common['l'].tolist(),
                         'memoryUsage (gesdd)': df_1_common['memoryUsage'].tolist(),
                         'memoryUsage (gesvd)': df_2_common['memoryUsage'].tolist()}

    # Crea la directory se non esiste
    save_dir = f'./results/plot/'
    os.makedirs(save_dir, exist_ok=True)

    # Crea i grafici per l'utilizzo della memoria
    plt.figure(figsize=(12, 7))
    plt.plot(range(len(memory_usage_data['l'])), memory_usage_data['memoryUsage (gesdd)'], label='gesdd', color='darkorange', marker='o')
    plt.plot(range(len(memory_usage_data['l'])), memory_usage_data['memoryUsage (gesvd)'], label='gesvd', color='darkgreen', marker='o')
    plt.title(f'Utilizzo della memoria')
    plt.xlabel('Righe della matrice sketch (l)')
    plt.ylabel('Utilizzo memoria (KB)')
    plt.ticklabel_format(style='plain', axis='y')  # Disabilita la notazione scientifica per l'asse y

    # Visualizza tutti i valori di l sull'asse x come testo senza tener conto dei reali valori numerici
    plt.xticks(range(len(memory_usage_data['l'])), map(str, memory_usage_data['l']), rotation=45)

    plt.legend()
    plt.savefig(f'{save_dir}memory_usage_{file_path}.png')

    # Crea i grafici per il tempo di esecuzione
    plt.figure(figsize=(12, 7))
    plt.plot(range(len(time_data['l'])), time_data['timeFd (gesdd)'], label='gesdd', color='darkorange', marker='o')
    plt.plot(range(len(time_data['l'])), time_data['timeFd (gesvd)'], label='gesvd', color='darkgreen', marker='o')
    plt.title(f'Tempo di Esecuzione')
    plt.xlabel('Righe della matrice sketch (l)')
    plt.ylabel('Tempo di esecuzione (ms)')
    plt.ticklabel_format(style='plain', axis='y')  # Disabilita la notazione scientifica per l'asse y

    # Visualizza tutti i valori di l sull'asse x come testo senza tener conto dei reali valori numerici
    plt.xticks(range(len(time_data['l'])), map(str, time_data['l']), rotation=45)

    plt.legend()
    plt.savefig(f'{save_dir}time_execution_{file_path}.png')

    # Crea i grafici per bound e accuracy
    plt.figure(figsize=(12, 7))
    plt.plot(range(len(bound_accuracy_data['l'])), bound_accuracy_data['bound'], label='Bound', color='firebrick', marker='o')
    plt.plot(range(len(bound_accuracy_data['l'])), bound_accuracy_data['accuracy'], label='Accuracy', color='royalblue', marker='o')
    plt.yscale('log')
    plt.title(f'Bound e Accuracy')
    plt.xlabel('Righe della matrice sketch (l)')
    plt.ylabel('Accuracy (log scale)')

    # evita di usare la notazione scientifica
    plt.gca().yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
    plt.ticklabel_format(style='plain', useOffset=False, axis='y')

    # Visualizza tutti i valori di l sull'asse x come testo senza tener conto dei reali valori numerici
    plt.xticks(range(len(bound_accuracy_data['l'])), map(str, bound_accuracy_data['l']), rotation=45)

    plt.yticks([1000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000], ['1000', '5000', '10000', '20000', '50000', '100000', '200000', '500000', '1000000'])

    plt.legend()
    plt.savefig(f'{save_dir}bound_accuracy_{file_path}.png')


if len(sys.argv) != 2:
    print("Usage: python script.py <file_name.csv>")
    sys.exit(1)

# Ottieni il nome del file da analizzare dall'argomento della linea di comando
file_name = sys.argv[1]

# Chiamata alla funzione per creare i grafici
plot_results(file_name)
