import subprocess,shlex,os,sys
#need to figure out this case, p2pt testlist
#scancel 2 xfail=ticket2266 xfail=ticket2270
passed = 0
failed = 0
timedout = 0
other = 0
mpiexec = "/home/umar/mpich-install/bin/mpiexec" + " -n "
testlistPaths = ["/home/umar/Downloads/mpich-3.2/test/mpi/coll/","/home/umar/Downloads/mpich-3.2/test/mpi/pt2pt/"]
timeLimit = 5
os.environ['MPIEXEC_TIMEOUT'] = str(timeLimit) #need to clarify about this
if len(sys.argv) == 1:
	for testlistPath in testlistPaths:
		x = testlistPath.split('/')		
		testFile = open(x[len(x)-2])
		testlist = testFile.read().strip().split('\n') 
		testFile.close()
		results = open('results-'+x[len(x)-2],'a+')
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
					passed+=1
				elif err is not None:
					dat = testProg + "-n "+str(i)+"...error\n Error: "+err+"\n"
					results.write(dat)
					failed+=1
				elif 'timed out' in out.lower():
					dat = testProg + "-n "+str(i)+"...timed out\n" 
					results.write(dat)
					timedout+=1
				else:
					#print out,err
		 			dat = testProg + "-n "+str(i)+"...\nout:" + out + "\n"
					results.write(dat)
					other+=1
		results.close()
		print "passed: ",passed,"failed: ",failed,"timedout: ",timedout,"other: ",other
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
