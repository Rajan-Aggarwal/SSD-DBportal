import subprocess
import getpass
import os

ROOT_WORK_DIR = '/home/{}/ssd-dbportal/root-scripts'.format(getpass.getuser())


def get_ner_of_meas(detector_id, meastype):
  '''
    ::param detector_id is the id of the detector

    ::param type is the type of measurement

    To return the number of 'type' readings 
    done on detector_id
  '''

  command = ['./GetNerOfMeasurement', detector_id, meastype]
  output  = subprocess.check_output(command, cwd=ROOT_WORK_DIR)
  # output is of the form b'\n<ner>\n'
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
    datetime_list = output.decode('utf-8').split('\n')[1:-1]
  except:
    datetime_list = []
  return datetime_list

print(get_list_of_datetimes('gfdsgdgfsfg','cv'))
