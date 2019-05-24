import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector, LocationTransfer

EMPTY_TEXT = 'There is no entry matching the given criteria'

class DetectorTable(tables.Table):

	more = tables.LinkColumn('more', args=[A('pk')], orderable=False, empty_values=())

	def render_more(self):
		return "More"

	class Meta:
		model 		= Detector
		attrs 		= {'class': 'paleblue'}
		empty_text 	= EMPTY_TEXT


class LocationTransferTable(tables.Table):

	class Meta:
		model 		= LocationTransfer
		attrs		= {'class': 'paleblue'}
		empty_text	= EMPTY_TEXT