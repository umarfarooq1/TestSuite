import subprocess,shlex,os,sys
from math import ceil
from os import listdir
from os.path import isfile, join

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

timeLimit = 14400
os.environ['MPIEXEC_TIMEOUT'] = str(timeLimit)
sbatch_launcher = "/usr/bin/sbatch"
mypath = os.getcwd()
my_batch = [f for f in listdir(mypath) if isfile(join(mypath, f))]#["batchfile16384.sh","batchfile2048.sh","batchfile32768.sh","batchfile4096.sh","batchfile8192.sh"]
my_batch_files = [f for f in my_batch if ".sh" in f]
command = sbatch_launcher + " "
for i in my_batch_files:
	x = command + i
	#print x
	args = shlex.split(x)
	#print args
	p = subprocess.Popen(args, stdout = subprocess.PIPE)
	out, err = p.communicate()
	print out,err
