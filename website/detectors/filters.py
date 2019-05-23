import django_filters
from .model import Detector

class DetectorFilter(django_filters.FilterSet):
	'''
		filter for the detectors (used in the search bar)
	'''
	class Meta:
		model 	= Detector
		fields 	= {'id', 'producer'}
