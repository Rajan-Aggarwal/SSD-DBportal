import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector, LocationTransfer, Annealing, Irradiation

EMPTY_TEXT = 'There is no entry matching the given criteria'	# message shown when any table is empty

class DetectorTable(tables.Table):

	'''
		the table to display all the detectors in the home page
	'''

	# add a hyper-link to more.html enlisting other tables
	more = tables.LinkColumn('more', verbose_name='More information', 
								args=[A('pk')], 
								orderable=False, 
								empty_values=(),
								) 

	def render_more(self):
		'''
			function to set what the link column displays as the
			link for the hyper-link
		'''
		return 'More'

	class Meta:
		model 		= Detector
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT
		ordering	= ['-arrival_date']


class LocationTransferTable(tables.Table):

	'''
		the table displaying all the location transfers for a given detector
	'''

	class Meta:
		model 		= LocationTransfer
		attrs		= {'class': 'paleblue'}
		empty_text	= EMPTY_TEXT
		exclude		= ('id', 'detector_id',)
		ordering	= ['-transfer_date']


class AnnealingTable(tables.Table):

	'''
		the table displaying all the annealing history for a given detector
	'''

	class Meta:
		model  		= Annealing
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT
		exclude 	= ('id', 'detector_id',)
		ordering 	= ['-annealing_date']


class IrradiationTable(tables.Table):

	'''
		the table displaying all the irradiation history for a given detector
	'''

	def render_fluence_or_dose(self, value):
		'''
			::param value is the value this
			func will get from the model field
			fluence_or_dose.

			changes the display of decimal field 
			to scientific notation
		'''
		return '{:.2E}'.format(value)

	class Meta:
		model 		= Irradiation
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT
		exclude 	= ('id', 'detector_id',)
		ordering 	= ['-irradiation_date']

		