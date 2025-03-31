#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./output_hibrido.txt
#SBATCH -e ./errors_hibrido.txt
mpirun --bind-to none a.out $1 128 $2
