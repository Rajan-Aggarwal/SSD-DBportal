from django_tables2 import SingleTableView
from django_tables2 import RequestConfig
import subprocess
import getpass

# root utils

def get_root_work_dir():
	'''
		determine which directory to use
		acc. to whether this is dev env
		or deployment env
	'''

	if getpass.getuser() == 'root':
		root_work_dir = '/home/ubuntu/ssd-dbportal/root-scripts'
	else:
		root_work_dir = '/home/{}/ssd-dbportal/root-scripts'.format(getpass.getuser())
	return root_work_dir


def get_ner_of_meas(detector_id, meastype):
	'''
		::param detector_id is the id of the detector

		::param type is the type of measurement

		To return the number of 'type' readings 
		done on detector_id
	'''

	command = ['./GetNerOfMeasurement', detector_id, meastype]
	try:
		output 	= subprocess.check_output(command, cwd=get_root_work_dir())
		ner 	= output.decode('utf-8')
	except Exception as e:
		print(e) 
		ner 	= '0 '
	return ner[:-1]


def get_list_of_datetimes(detector_id, meastype):
	'''
		::param detector_id is the id of the detector
		
		::param meastype is the type of measurement

		To return a list of datetimes of measurement of type
		meastype on detector_id
	'''

	command = ['./GetListOfDates', detector_id, meastype]
	try:
		output 	= subprocess.check_output(command, cwd=get_root_work_dir())
		# get it in a list of strings format
		datetime_list = output.decode('utf-8').split('\n')[:-1]
	except Exception as e:
		print(e)
		datetime_list = None
	
	return datetime_list


def create_measurement_pdf(detector_id, meastype, datetime):
	'''
		::param detector_id is the id of the detector

		::param meastype is the type of measurement

		::param datetime is the date and time of the measurement

		To generate a pdf of root measurements and store it in 
		~/ssddb-portal/tmp/pdfs
	'''
	command = ['./GetPlotCVIV', detector_id, meastype, datetime]
	try:
		subprocess.run(command, cwd=get_root_work_dir())
	except Exception as e:
		print(e)


def get_ner_of_meas_tct(detector_id, meastype):
	'''
		::param detector_id is the id of the detector

		::param type is the type of tct measurement

		To return the number of 'type' tct readings 
		done on detector_id
	'''

	command = ['./GetNerOfMeasurement_TCT', detector_id, meastype]
	try:
		output 	= subprocess.check_output(command, cwd=get_root_work_dir())
		ner 	= output.decode('utf-8')
	except Exception as e:
		print(e)
		ner 	= '0 '
	return ner[:-1]


def get_list_of_files_tct(detector_id, meastype):
	'''
		::param detector_id is the id of the detector
		
		::param meastype is the type of measurement

		To return a list of files of measurements of tct type
		meastype on detector_id
	'''

	command = ['./GetListOfFiles_TCT', detector_id, meastype]
	try:
		output 		= subprocess.check_output(command, cwd=get_root_work_dir())
		# get it in a list of strings format
		file_list 	= output.decode('utf-8').split('\n')[:-1]
	except Exception as e:
		print(e)
		file_list	= None

	return file_list



