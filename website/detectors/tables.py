import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector, LocationTransfer, Annealing, Irradiation

EMPTY_TEXT = 'There is no entry matching the given criteria'

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
		return "More"

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
		ordering	= ['-transfer_datetime']


class AnnealingTable(tables.Table):

	'''
		the table displaying all the annealing history for a given detector
	'''

	class Meta:
		model  		= Annealing
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT
		exclude 	= ('id', 'detector_id',)
		ordering 	= ['-annealing_datetime']


class IrradiationTable(tables.Table):

	'''
		the table displaying all the irradiation history for a given detector
	'''

	class Meta:
		model 		= Irradiation
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT
		exclude 	= ('id', 'detector_id',)
		ordering 	= ['-irradiation_datetime']

		