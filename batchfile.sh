#!/bin/bash
#
#SBATCH --job-name=mpihello
#SBATCH --output=helloResult.txt
#SBATCH -N20
#SBATCH --ntasks-per-node=24
#SBATCH --time=00:10:00

python config.py

