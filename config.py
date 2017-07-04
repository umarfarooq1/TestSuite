import subprocess,shlex
#print "Script is starting..."
mpiexec = "/home/umar/mpich-install/bin/mpiexec" + " -n "
testlistPath = "/home/umar/Downloads/mpich-3.2/test/mpi/coll/"
testlist = "allgather2" #this will later be read from a file
for i in range (2,5):
	command = mpiexec + str(i)+" "+ testlistPath + testlist
	args = shlex.split(command)
	#print args
	p = subprocess.Popen(args, stdout = subprocess.PIPE)
	out, err = p.communicate()
	print out,i

#print "Ending the tests..."
