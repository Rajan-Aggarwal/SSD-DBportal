import django_tables2 as tables 
from django_tables2.utils import A
from .models import Detector

class DetectorTable(tables.Table):
	class Meta:
		model 		= Detector
		attrs 		= {'class': 'table-striped table-bordered'}
		empty_text 	= 'There are no detectors matching the search criteria'
