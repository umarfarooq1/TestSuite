import subprocess,shlex,os,sys

timeLimit = 14400
os.environ['MPIEXEC_TIMEOUT'] = str(timeLimit)
sbatch_launcher = "/usr/bin/sbatch"

my_batch_files = ["batchfile65536.sh"]#["batchfile16384.sh","batchfile2048.sh","batchfile32768.sh","batchfile4096.sh","batchfile8192.sh"]

command = sbatch_launcher + " "
for i in my_batch_files:
	x = command + i
	#print x
	args = shlex.split(x)
	#print args
	p = subprocess.Popen(args, stdout = subprocess.PIPE)
	out, err = p.communicate()
	print out,err
