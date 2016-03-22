import subprocess as sp
import time
import os

def run(cmd,file):
	print ' '.join(cmd) + ' ' + file

	f = open('random3SAT/' + file)
	p = sp.Popen(cmd, stdin=f, stdout=sp.PIPE)

	tic = time.time()
	output = p.communicate()[0]
	tac = time.time()
	elapsed = tac-tic

	for line in output.splitlines():
		if 'SATISFIABLE' in line:
			sat = line.split()[1]
		if 'decisions' in line:
			dec = int(line.split()[1])
		if 'propagations' in line:
			prop = int(line.split()[1])

	return sat, elapsed, dec, int(prop/elapsed)


buffer = 'file' + '\t' + 'result' + '\t' + \
		 'picosat time' + '\t' + 'picosat dec' + '\t' + 'picosat pps' + '\t' + \
		 'oscarsat time' + '\t' + 'oscarsat dec' + '\t' + 'oscarsat pps' + '\n'
for file in sorted(os.listdir('random3SAT/')):
	if file.startswith('vars-250'):
		buffer += file + '\t'
		sat1, t, dec, pps = run(['picosat','-v'],file)
		buffer += sat1 + '\t' + str(t) + '\t' + str(dec) + '\t' + str(pps) + '\t'
		sat2, t, dec, pps = run(['./oscarSAT'],file)
		buffer += str(t) + '\t' + str(dec) + '\t' + str(pps) + '\n'
		if sat1 != sat2: raise Exception('ERROR: results do not match')

f = open('table.csv', "w")
f.write(buffer)
f.close()