import django_tables2 as tables 
from .models import Detector

class DetectorTable(tables.Table):
	class Meta:
		model = Detector
		attrs = {'class': 'paleblue', 'width': '70%'}
		template_name = 'django_tables2/bootstrap.html'
