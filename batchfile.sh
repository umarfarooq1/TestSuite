#!/bin/bash
#
#SBATCH --job-name=MatrixMultiplication
#SBATCH --output=Result_2048_n4.txt
#SBATCH --exclusive
#SBATCH --mem=0
#SBATCH -N1
#SBATCH --ntasks-per-node=24
#SBATCH --time=04:00:00

mpiexec -l -n 1 hostname
mpiexec -n 1 ./testing 2048
