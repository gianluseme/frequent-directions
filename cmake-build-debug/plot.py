import pandas as pd
import matplotlib.pyplot as plt


file_names = ['matrice.csv', 'mediumMatrix.csv', 'largeMatrix.csv']
svd = ['gesdd', 'gesvd']

for svd_value in svd:
    for file_name in file_names:

        # Leggi i dati dal file CSV
        file_path = 'results/'+svd_value+'/list/results_list_'+file_name
        data = pd.read_csv(file_path, header=None)

        # Estrai le colonne interessate
        l_values = data.iloc[:, 0]  # Prima colonna (l)
        tempo_di_esecuzione = data.iloc[:, 1]  # Seconda colonna (tempo di esecuzione)
        accuracies = data.iloc[:, 3] # Quarta colonna (accuratezza)

        # Imposta le dimensioni della figura
        plt.figure(figsize=(16, 8))  # Imposta le dimensioni desiderate (larghezza x altezza)


        # Crea un grafico
        plt.plot(l_values, tempo_di_esecuzione, linestyle='-', linewidth=3.0, alpha=0.7)

        # Aggiungi etichette e titoli
        plt.xlabel('l')
        plt.ylabel('Tempo di esecuzione (ms)')
        plt.title('Grafico '+file_name+' (l e tempo di esecuzione)')

        # Imposta valori personalizzati sull'asse x
        custom_ticks = [10, 500, 1000, 1500, 2000, 3000, 4000, 5000, 7500, 10000]  # Modifica questa lista con i tuoi valori desiderati
        plt.xticks(ticks=custom_ticks)
        # plt.yticks(tempo_di_esecuzione)


    # Salva il grafico in un file
        plt.savefig('results/'+svd_value+'/grafico_tempoDiEsecuzione_'+file_name+'.png')

        plt.figure(figsize=(16, 8))  # Imposta le dimensioni desiderate (larghezza x altezza)


        plt.plot(l_values, accuracies, linestyle='-', linewidth=3.0, alpha=0.7)

        plt.xlabel('l')
        plt.ylabel('Accuratezza')
        plt.title('Grafico '+file_name+' (l e accuracy)')

        custom_ticks = [10, 500, 1000, 1500, 2000, 3000, 4000, 5000, 7500, 10000]  # Modifica questa lista con i tuoi valori desiderati
        plt.xticks(ticks=custom_ticks)


        plt.savefig('results/'+svd_value+'/grafico_accuracy_'+file_name+'.png')


