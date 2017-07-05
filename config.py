import subprocess,shlex,os,sys

mpiexec = "/home/umar/mpich-install/bin/mpiexec" + " -n "
testlistPath = "/home/umar/Downloads/mpich-3.2/test/mpi/coll/"
testFile = open('testlist')
timeLimit = 5
testlist = testFile.read().strip().split('\n') 
testFile.close()
os.environ['MPIEXEC_TIMEOUT'] = str(timeLimit) #need to clarify about this
results = open('results','a+')
if len(sys.argv) == 1:
	for testProg in testlist:
		for i in range (2,5):
			command = mpiexec + str(i)+" "+ testlistPath + testProg
			args = shlex.split(command)
			#print args
			#print testProg,i
			p = subprocess.Popen(args, stdout = subprocess.PIPE)
			out, err = p.communicate()
			#print out, "this"
			if err is None and "no errors" in out.lower():
				dat = testProg + "-n "+str(i)+"...ok\n"
				results.write(dat)
			elif err is not None:
				dat = testProg + "-n "+str(i)+"...error\n Error: "+err+"\n"
				results.write(dat)
			elif 'timed out' in out.lower():
				dat = testProg + "-n "+str(i)+"...timed out\n" 
				results.write(dat)
			else:
				print out,err
	 			dat = testProg + "-n "+str(i)+"...\nout:" + out + "\n"
			results.write(dat)
elif sys.argv[1] == '-p':
	for i in range (2,5):
		command = mpiexec + str(i)+" "+ testlistPath + sys.argv[2]
		if len(sys.argv) > 3:
			for i in range(3,len(sys.argv)):
				command = command+ ' '+sys.argv[i]		
		args = shlex.split(command)		
		#print args
		p = subprocess.Popen(args, stdout = subprocess.PIPE)
		out, err = p.communicate()
		print out,err	


