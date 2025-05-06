#!/bin/bash 
#SBATCH -N 1
#SBATCH --exclusive

# Tamaños de matriz válidos 
nxn=(1024 2048 4096)

# Cantidad de hilos 
hilos=(2 4 8)

for n in "${nxn[@]}"; do
    for h in "${hilos[@]}"; do
        output_file="./output/output_${n}_${h}_o3_fix_1.txt"
        error_file="./output/errors_${n}_${h}_o3_fix_1.txt"
        ./ejercicio_pthreads_o3 "$n" "$h" > "$output_file" 2> "$error_file"
    done
done
