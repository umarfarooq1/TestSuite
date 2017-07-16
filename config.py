import subprocess,shlex,os,sys
#need to figure out this case, p2pt testlist
#scancel 2 xfail=ticket2266 xfail=ticket2270
passed = 0
failed = 0
timedout = 0
other = 0
mpilauncher = "/export/installs/mpi/mpich-3.2/bin/mpiexec"
testlistPath = "/home/18100155/helloworld/"#"/export/installs/mpi/mpich-3.2-testsuite/coll/"
testlistPaths = ["/export/installs/mpi/mpich-3.2-testsuite/coll/"]
timeLimit = 5
os.environ['MPIEXEC_TIMEOUT'] = str(timeLimit) #need to clarify about this

def executeProg(n,testlistPath,testProg,otherArgs):
	#add space character before other args, also deal with -n outside this, in the if condition
	command = mpilauncher + " -n "+ str(n)+" "+otherArgs+" "+ testlistPath + testProg
	print command
	args = shlex.split(command)
	p = subprocess.Popen(args, stdout = subprocess.PIPE)
	out, err = p.communicate()
	return out,err

def update(err,out,testProg,n,results):
	if err is None and "no errors" in out.lower():
		dat = testProg + "-n "+str(n)+"...ok\n"
		results.write(dat)
		global passed
		passed+=1
	elif err is not None:
		dat = testProg + "-n "+str(n)+"...error\n Error: "+err+"\n"
		results.write(dat)
		global failed
		failed+=1
	elif 'timed out' in out.lower():
		dat = testProg + "-n "+str(n)+"...timed out\n" 
		results.write(dat)
		global timedout
		timedout+=1
	else:
		#print out,err
		dat = testProg + "-n "+str(n)+"...\nout:" + out + "\n"
		results.write(dat)
		global other
		other+=1
if len(sys.argv) == 1:
	for testlistPath in testlistPaths:
		x = testlistPath.split('/')
		testFile = open(x[len(x)-2])
		testlist = testFile.read().strip().split('\n')
		testFile.close()
		results = open('results-'+x[len(x)-2],'a+')
		for testProg in testlist:
			#print testProg
			if not testProg.startswith('#') and not testProg[0].isdigit() and not (testProg[0] == '-'): #to ignore comments and commented tests
				for i in range (2,5):
					out,err = executeProg(i,testlistPath,testProg,'') #the last argument is empty as discussed for new flags before the program.
					update(err,out,testProg,i,results)					
			elif testProg[0].isdigit():
				#print "not working with range data"
				i = int(testProg[0])
				testProg = testProg[2:]
				out,err = executeProg(i,testlistPath,testProg,'')
				update(err,out,testProg,i,results)
			else:
				sysargv = testProg.split(' ')
				testProg = sysargv[-1]
				sysargv = sysargv[:-1]
				args = dict()
				argIter = iter(sysargv)
				for i in argIter:
					args[i] = next(argIter)
				argString = ""
		                for k in args:
                		        if not(k == '-n'):
                                		argString = argString+str(k)+" "+str(args[k])+" "
                			argString = argString.strip()
		                out,err = executeProg(int(args['-n']),testlistPath,testProg,argString)
                		print out,err
				update(err,out,testProg,i,results)
		results.close()
		print "passed: ",passed,"failed: ",failed,"timedout: ",timedout,"other: ",other
elif sys.argv[1] == '-p':
	testProg = sys.argv[-1]
	sys.argv = sys.argv[2:-1]
	if len(sys.argv) > 0:
		args = dict()
		argIter = iter(sys.argv)
		for i in argIter:
			args[i] = next(argIter)
		argString = ""
		for k in args:
			if not(k == '-n'):
				argString = argString+str(k)+" "+str(args[k])+" "
		argString = argString.strip()
		#print argString	
		out,err = executeProg(int(args['-n']),testlistPath,testProg,argString)
                print out,err			
	else:
		print "Same old logic"
		for i in range (2,5):
			out,err = executeProg(i,testlistPath,testProg,'')
			print out,err
