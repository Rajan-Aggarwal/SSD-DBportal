import subprocess
import getpass
import os

ROOT_WORK_DIR = '/home/ubuntu/ssd-dbportal/root-scripts'


def get_ner_of_meas(detector_id, meastype):
  '''
    ::param detector_id is the id of the detector

    ::param type is the type of measurement

    To return the number of 'type' readings 
    done on detector_id
  '''

  command = ['sudo', 'ROOTSYS=/usr/local', 'LD_LIBRARY_PATH=/usr/local/lib/root', './GetNerOfMeasurement', detector_id, meastype]
  try:
    output=subprocess.check_output(command, cwd=ROOT_WORK_DIR)
  # output is of the form b'\n<ner>\n'
  except Exception as e:
    print(e)
  ner     = output.decode('utf-8')
  return ner

def get_list_of_datetimes(detector_id, meastype):
  '''
    ::param detector_id is the id of the detector
    
    ::param meastype is the type of measurement

    To return a list of datetimes of measurement of type
    meastype on detector_id
  '''

  command = ['./GetListOfDates', detector_id, meastype]

  try:
    output  = subprocess.check_output(command, cwd=ROOT_WORK_DIR)

    # get it in a list of strings format
    datetime_list = output.decode('utf-8').split('\n')[:-1]
  except:
    datetime_list = []
  return datetime_list

print(get_list_of_datetimes('LGADs_CNM_11748_W5_DB17','iv'))
