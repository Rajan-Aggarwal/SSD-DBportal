import django_filters
from .models import Detector, LocationTransfer, Annealing, Irradiation

class DetectorFilter(django_filters.FilterSet):
	'''
		filter for the detectors (used in the search bar)
	'''
	class Meta:
		model 	= Detector
		fields 	= {
			'id': ['icontains'],
			'trec_id': ['icontains'],
			'producer': ['icontains'],
			'project': ['icontains'],
			'run_number': ['icontains'],
			'current_location': ['icontains'],
			'ssd_responsible': ['icontains']
		}


class LocationTransferFilter(django_filters.FilterSet):
	'''
		filter for the location transfer history (used in the search bar)
	'''
	class Meta:
		model 	= LocationTransfer
		fields 	= {
			'destination_location': ['icontains'],
			'responsible_party': ['icontains'],
			'transfer_datetime': ['icontains'],
			'internal_or_external': ['iexact']
		}


class AnnealingFilter(django_filters.FilterSet):
	'''
		filter for the annealing history (used in the seach bar)
	'''
	class Meta:
		model 	= Annealing
		fields 	= {
			'annealing_datetime': ['icontains'],
			'temperature': ['iexact'],
			'time': ['iexact'],
		} 


class IrradiationFilter(django_filters.FilterSet):
	'''
		filter for the irradiation history (used in the search bar)
	'''
	class Meta:
		model 	= Irradiation
		fields	= {
			'irradiation_particle': ['iexact'],
			'location': ['icontains'],
			'fluence_or_dose': ['icontains'],
			'irradiation_datetime': ['icontains']
		}