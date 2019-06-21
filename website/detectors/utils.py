from django_tables2 import SingleTableView
from django_tables2 import RequestConfig
import subprocess


# root utils

ROOT_WORK_DIR = '/home/raaggarw/ssd-dbportal/root-scripts'

def get_ner_of_meas(detector_id, meastype):
	'''
		::param detector_id is the id of the detector

		::param type is the type of measurement

		To return the number of 'type' readings 
		done on detector_id
	'''

	command = ['root', '-b', '-l', '-q',
				'GetNerOfMeasurement.C(\"{}\",\"{}\")'.format(
														detector_id,
														meastype)]
	output 	= subprocess.check_output(command, cwd=ROOT_WORK_DIR)

	# output is of the form b'\n<ner>\n'
	ner 	= output.decode('utf-8')[1] 
	return ner


def get_list_of_datetimes(detector_id, meastype):
	'''
		::param detector_id is the id of the detector
		
		::param type is the type of measurement

		To return a list of datetimes of measurement of type
		meastype on detector_id
	'''

	command = ['root', '-b', '-l', '-q',
				'GetListOfDates.C(\"{}\",\"{}\")'.format(
													detector_id,
													meastype)]
	output 	= subprocess.check_output(command, cwd=ROOT_WORK_DIR)

	# get it in list of strings format
	datetime_list = output.decode('utf-8').split('\n')[1:-1]
	return datetime_list



############################
###NOT UTILIZED AS OF YET###
############################

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