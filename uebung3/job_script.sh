#!/bin/bash
#SBATCH -N 4 --ntasks-per-node=4
#SBATCH -n 16
#SBATCH -o timescript.out

srun ./timescript.sh

echo fertig > job_script.out
