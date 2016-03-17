import subprocess
import time
import os

def run(cmd,file):
    f = open('random3SAT/' + file)
    p = subprocess.Popen(cmd, stdin=f, stdout=subprocess.PIPE)
    start = time.time()
    output = p.communicate()[0]
    end = time.time()
    elapsed = end-start;
    for line in output.splitlines():
	print(line)
    return elapsed

for file in sorted(os.listdir('random3SAT/')):
    if file.startswith('vars-300'):
	print file
	print '========================'
	
	print 'Running oscarSAT...'
	oscarSAT_time = run(['./oscarSAT'],file)
	print '***'
	print 'Running picosat...'
	picosat_time = run(['picosat','-n'],file)
	print '***'
	
	print 'oscarSAT time: ' + str(round(oscarSAT_time,3))
	print 'picosat time: ' + str(round(picosat_time,3))
	print 'Comparison: ' + str(round(oscarSAT_time/picosat_time,2)) + '\n'