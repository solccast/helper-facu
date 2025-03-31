#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --partition=Blade
#SBATCH -o ./output.txt
#SBATCH -e ./errors.txt
./matrices $1 $2

