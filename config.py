import subprocess,shlex

#Comments
#=======================
#next step is to cater the arguments for a particular program. And add the timeout if it doesnt respond
#after that i will be catering the arguments such as mpiversion, e-g 'ibarrier 2 mpiversion=3.0 timeLimit=30'
#=======================

#"Starting tests..."
mpiexec = "/home/umar/mpich-install/bin/mpiexec" + " -n "
testlistPath = "/home/umar/Downloads/mpich-3.2/test/mpi/coll/"
testFile = open('testlist')

testlist = testFile.read().strip().split('\n') #this will later be read from a file

for testProg in testlist:
	for i in range (2,5):
		command = mpiexec + str(i)+" "+ testlistPath + testProg
		args = shlex.split(command)
		#print args
		p = subprocess.Popen(args, stdout = subprocess.PIPE)
		out, err = p.communicate()
		print out,i

#"Ending the tests..."
