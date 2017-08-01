from math import ceil

f = open('combinations.txt','r')
data = f.read().split('\n')
f.close()
mystr= '''#!/bin/bash
#
#SBATCH --job-name=MatrixMultiplication
#SBATCH --output=Result_%s_n%s.txt
#SBATCH --exclusive
#SBATCH --mem=0
#SBATCH -N%s
#SBATCH --ntasks-per-node=24
#SBATCH --time=04:00:00

mpiexec -l -n %s hostname
mpiexec -n %s ./mm %s'''

for i in data:
	try:
		n = int(i.split(' ')[0])
		matrix_size = int(i.split(' ')[1])
		nodes = int(ceil(float(n)/float(24)))
		if (nodes > 20):
			nodes = 20
		fileData = mystr % (matrix_size,n,nodes,n,n,matrix_size)
#	print fileData
		filename = "batchfile%s.sh" %(str(matrix_size))
		x = open(filename,'w')
		x.write(fileData)
		x.close()
	except:
		pass
