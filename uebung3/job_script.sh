#! /bin/bash
#SBATCH -A froehlich
#SBATCH -N 4
#SBATCH -n 4
srun ./timescript.sh > ./timescript.out
