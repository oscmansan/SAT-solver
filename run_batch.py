import subprocess
import time
import os

for file in sorted(os.listdir('random3SAT/')):
    if file.startswith('vars-200'):
	f = open('random3SAT/' + file)
	p = subprocess.Popen(['./oscarSAT'], stdin=f, stdout=subprocess.PIPE)
	print file
	print '========================'
	start = time.time()
	output = p.communicate()[0]
	end = time.time()
	for line in output.splitlines():
	    print(line)
	print 'Time: ' + str(end-start) + '\n'