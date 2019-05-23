import django_filters
from .models import Detector

class DetectorFilter(django_filters.FilterSet):
	'''
		filter for the detectors (used in the search bar)
	'''
	#id = django_filters.CharFilter(lookup_expr=['icontains', 'exact'], label='Detector name')
	class Meta:
		model 	= Detector
		fields 	= {
			'id': ['icontains'],
			'trec_id': ['icontains'],
			'producer': ['icontains'],
			'project': ['icontains'],
			'run_number': ['icontains'],
		}
