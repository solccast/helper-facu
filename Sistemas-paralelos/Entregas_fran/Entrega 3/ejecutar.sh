#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=8
#SBATCH -o ./output.txt
#SBATCH -e ./errors.txt
mpirun ecuacion_MPI $1 128
