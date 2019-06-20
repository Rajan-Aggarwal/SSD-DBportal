import subprocess

# root -b -q -l "GetNerOfMeasurement.C(\"3D-7781-DET-3-Pad_u_25x50\",\"cv\")" | tail -n 1

WORKING_DIR = '/home/raaggarw/ssd-dbportal/root-scripts'
COMMAND 	= ['root', '-b', '-l', '-q', 
	'./GetNerOfMeasurement.C(\"3D-7781-DET-3-Pad_u_25x50\",\"cv\")'
	,]

try:
	output = subprocess.check_output(COMMAND, cwd=WORKING_DIR)
except Exception as e:
	output = str(e.output)

print(output)
