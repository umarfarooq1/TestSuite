#!/bin/bash
#
#SBATCH --job-name=TestSuite
#SBATCH --output=helloResult(1).txt
#SBATCH -N20
#SBATCH --ntasks-per-node=24
#SBATCH --time=00:10:00

python config.py

