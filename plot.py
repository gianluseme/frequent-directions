import os
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
import sys


def plot_results(file_path, svd_options):
    for svd_option in svd_options:

        df_1 = pd.read_csv(f'./results/{svd_option}/list/results_1_{file_path}')
        df_2 = pd.read_csv(f'./results/{svd_option}/list/results_{file_path}')

        # Inizializza i dizionari direttamente dai DataFrame
        time_data = {'l': df_1['l'].tolist(),
                     'timeFd (l)': df_1['timeFd'].tolist(),
                     'timeFd (2l)': df_2['timeFd'].tolist()}

        bound_accuracy_data = {'l': df_1['l'].tolist(),
                               'bound': df_1['bound'].tolist(),
                               'accuracy (l)': df_1['accuracy'].tolist(),
                               'accuracy (2l)': df_2['accuracy'].tolist()}

        # Crea la directory se non esiste
        save_dir = f'./results/{svd_option}/plot/'
        os.makedirs(save_dir, exist_ok=True)

        # Crea i grafici per il tempo di esecuzione
        plt.figure(figsize=(12, 7))
        plt.plot(range(len(time_data['l'])), time_data['timeFd (l)'], label='timeFd (l)', color='darkorange', marker='o')
        plt.plot(range(len(time_data['l'])), time_data['timeFd (2l)'], label='timeFd (2l)', color='royalblue', marker='o')
        plt.title(f'Tempo di Esecuzione - SVD: {svd_option}')
        plt.xlabel('l')
        plt.ylabel('Tempo (ms)')
        plt.ticklabel_format(style='plain', axis='y')  # Disabilita la notazione scientifica per l'asse y

        # Visualizza tutti i valori di l sull'asse x come testo senza tener conto dei reali valori numerici
        plt.xticks(range(len(bound_accuracy_data['l'])), map(str, bound_accuracy_data['l']), rotation=45)

        plt.legend()
        plt.savefig(f'{save_dir}time_execution_{file_path}.png')

        # Crea i grafici per bound e accuracy
        plt.figure(figsize=(12, 7))
        plt.plot(range(len(bound_accuracy_data['l'])), bound_accuracy_data['bound'], label='Bound', color='firebrick', marker='o')
        plt.plot(range(len(bound_accuracy_data['l'])), bound_accuracy_data['accuracy (l)'], label='Accuracy (l)', color='darkorange', marker='o')
        plt.plot(range(len(bound_accuracy_data['l'])), bound_accuracy_data['accuracy (2l)'], label='Accuracy (2l)', color='royalblue', marker='o')
        plt.yscale('log')
        plt.title(f'Bound e Accuracy - SVD: {svd_option}')
        plt.xlabel('l')
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

svd_options = ['gesvd', 'gesdd']

# Chiamata alla funzione per creare i grafici
plot_results(file_name, svd_options)
