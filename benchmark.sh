#!/bin/bash

# Verifica il numero corretto di argomenti dalla riga di comando
if [ "$#" -lt 1 ] || [ "$#" -gt 3 ]; then
    echo "Usage: $0 [<file1> [<file2> [<file3>]]]"
    exit 1
fi

percorso_programma_cpp="./frequent_directions"

# Lista dei file dai parametri della linea di comando
file_paths=("$@")

# Funzione per ottenere dinamicamente la lista l_values in base al numero di colonne del file CSV
get_l_values() {
    num_columns=$1

    # Implementa la logica per ottenere la lista l_values in base al numero di colonne
    if [ "$num_columns" -le 100 ]; then
        base_list=$(seq 10 10 $((2 * num_columns)))
    elif [ "$num_columns" -le 1000 ]; then
        base_list=$(seq 10 10 100; seq 200 100 $((2 * num_columns)))
    elif [ "$num_columns" -le 2000 ]; then
        base_list=$(seq 10 10 100; seq 200 100 1000; seq 1100 100 2000; seq 2200 200 $((2 * num_columns)))
    else
        base_list=$(seq 10 10 100; seq 200 100 1000; seq 1100 100 2000; seq 2200 200 $((2 * num_columns)))
    fi

    # Aggiungi l'eventuale resto al valore massimo di num_columns
    if [ "$((num_columns % 10))" -gt 0 ]; then
        base_list+=" $num_columns"
    fi

    echo "$base_list"
}

# Funzione per leggere il numero di colonne di un file csv
get_num_columns() {
    file_path=$1
    # Utilizza cat per ottenere la prima riga del file e wc per contare gli elementi
        num_columns=$(cat "$file_path" | head -n 1 | tr -s ',' | awk -F',' '{print NF}')
        echo "$num_columns"
}

# Funzione per eseguire il programma C++
esegui_programma() {
    file_input=$1
    l=$2
    svd_option=$3
    ./frequent_directions -i "$file_input" -l "$l" --svd "$svd_option" --mode acc --bench

}

# Funzione per eseguire il programma per tutti i file, i valori di l e modalità di esecuzione
esegui_programma_per_tutti_i_file() {
    start_time=$(date +%s)  # Registra il tempo di inizio
    for file_input in "${file_paths[@]}"; do
        for svd_value in "gesvd" "gesdd"; do
            if [ -n "$file_input" ]; then
                num_columns=$(get_num_columns "$file_input")
                l_values=($(get_l_values "$num_columns"))
                for l_value in "${l_values[@]}"; do
                    echo "Esecuzione per l = $l_value su $file_input, SVD: ($svd_value)..."
                    esegui_programma "$file_input" "$l_value" "$svd_value"
                done
            fi
        done

        # Esecuzione dello script python per generare i grafici per il file considerato
        python3 plot.py "$file_input"
    done
    end_time=$(date +%s)  # Registra il tempo di fine
    elapsed_time=$((end_time - start_time))
    echo "Tempo totale di esecuzione: $elapsed_time secondi"
}

esegui_programma_per_tutti_i_file

echo "Esecuzioni completate."
