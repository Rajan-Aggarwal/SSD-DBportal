import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector

class DetectorTable(tables.Table):

	more = tables.LinkColumn('more', args=[A('pk')], orderable=False, empty_values=())

	def render_more(self):
		return "More"

	class Meta:
		model 		= Detector
		attrs 		= {'class': 'paleblue'}
		empty_text 	= 'There are no detectors matching the search criteria'
