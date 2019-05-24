import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector

class DetectorTable(tables.Table):

	more_info = tables.TemplateColumn('<a href="/home/{{record.id}}">More</a>')

	class Meta:
		model 		= Detector
		attrs 		= {'class': 'paleblue'}
		empty_text 	= 'There are no detectors matching the search criteria'
