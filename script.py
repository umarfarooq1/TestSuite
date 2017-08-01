import subprocess,shlex,os,sys
from os import listdir
from os.path import isfile, join

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
