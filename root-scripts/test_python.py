import subprocess

# root -b -q -l "GetNerOfMeasurement.C(\"3D-7781-DET-3-Pad_u_25x50\",\"cv\")" | tail -n 1

WORKING_DIR = '/home/raaggarw/ssd-dbportal/root-scripts'
COMMAND 	= ['root', '-b', '-l', '-q', 
	'./GetListOfDates.C(\"3D-7781-DET-3-Pad_u_25x50\",\"iv\")'
	,]

output = subprocess.check_output(COMMAND, cwd=WORKING_DIR)
print(output.decode('utf-8').split('\n')[1:-1])
