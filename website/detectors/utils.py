from django_tables2 import SingleTableView
from django_tables2 import RequestConfig
import subprocess
import getpass

# root utils

ROOT_WORK_DIR = '/home/{}/ssd-dbportal/root-scripts'.format(getpass.getuser())

def get_ner_of_meas(detector_id, meastype):
	'''
		::param detector_id is the id of the detector

		::param type is the type of measurement

		To return the number of 'type' readings 
		done on detector_id
	'''

	command = ['./GetNerOfMeasurement', detector_id, meastype]
	try:
		output 	= subprocess.check_output(command, cwd=ROOT_WORK_DIR)
		ner 	= output.decode('utf-8')
	except:
		ner 	= 0
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
		output 	= subprocess.check_output(command, cwd=ROOT_WORK_DIR)
		# get it in a list of strings format
		datetime_list = output.decode('utf-8').split('\n')[1:-1]
	except:
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
	subprocess.run(command, cwd=ROOT_WORK_DIR)




# THIS CODE HAS NOT BEEN UTILIZED; USE IT IN CASE PAGINATION FAILS #
####################################################################
####################################################################
####################################################################

class PagedFilteredTableView(SingleTableView):
	'''
		In-case default pagination given by django_tables2.SingleTableMixin 
		(inherited in the generic-view classes) doesn't work
	'''
	filter_class 		= None
	formhelper_class 	= None
	context_filter_name = 'filter'

	def get_queryset(self, **kwargs):
		'''
			override the get_queryset to allow pagination
		'''
		qs 							= super(PagedFilteredTableView, self).get_queryset()
		self.filter 				= self.filter_class(self.request.GET, queryset=qs)
		self.filter.form.helper 	= self.formhelper_class()
		return self.filter.qs

	def get_context_data(self, **kwargs):
		'''
			override the get_context_data to allow pagination
		'''
		context = super(PagedFilteredTableView, self).get_context_data()
		context[self.context_filter_name] = self.filter
		return context