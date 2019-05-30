import django_filters
from .models import Detector, LocationTransfer, Annealing, Irradiation

class DetectorFilter(django_filters.FilterSet):

	'''
		filter for the detectors (used in the search bar)
	'''

	id 					= django_filters.CharFilter(label='Detector ID:', lookup_expr='icontains')
	trec_id 			= django_filters.CharFilter(label='TREC ID:', lookup_expr='icontains')
	producer 			= django_filters.CharFilter(label='Producer:', lookup_expr='icontains')
	project 			= django_filters.CharFilter(label='Project:', lookup_expr='icontains')
	run_number 			= django_filters.CharFilter(label='Run number:', lookup_expr='icontains')
	current_location 	= django_filters.CharFilter(label='Current location:', lookup_expr='icontains')
	ssd_responsible 	= django_filters.CharFilter(label='Responsible person:', lookup_expr='icontains')
	dead_or_alive 		= django_filters.CharFilter(label='Dead or Alive:', lookup_expr='iexact')
	arrival_date 		= django_filters.DateFilter(label='Arrival date:', lookup_expr='iexact')

	class Meta:
		model 	= Detector
		fields 	= ['id', 'trec_id', 'producer', 'project',
				'run_number', 'current_location','ssd_responsible',
				'dead_or_alive', ]


class LocationTransferFilter(django_filters.FilterSet):

	'''
		filter for the location transfer history (used in the search bar)
	'''

	transfer_datetime 		= django_filters.DateTimeFilter(label='Date and time:', lookup_expr='icontains')
	source_location 		= django_filters.CharFilter(label='Initial location:', lookup_expr='icontains')
	destination_location	= django_filters.CharFilter(label='Destination location:', lookup_expr='icontains')
	internal_or_external	= django_filters.CharFilter(label='Internal or External:', lookup_expr='iexact')
	responsible_party 		= django_filters.CharFilter(label='Person responsible:', lookup_expr='icontains')

	class Meta:
		model 	= LocationTransfer
		fields 	= ['transfer_datetime', 'source_location', 'destination_location',
					'internal_or_external', 'responsible_party', ]


class AnnealingFilter(django_filters.FilterSet):

	'''
		filter for the annealing history (used in the seach bar)
	'''

	annealing_datetime	= django_filters.DateTimeFilter(label='Date and time:', lookup_expr='icontains')
	temperature			= django_filters.NumberFilter(label='Temperature:', lookup_expr='icontains')
	time 				= django_filters.NumberFilter(label='Time:', lookup_expr='icontains')

	class Meta:
		model 	= Annealing
		fields 	= ['annealing_datetime', 'time', 'temperature', ]


class IrradiationFilter(django_filters.FilterSet):

	'''
		filter for the irradiation history (used in the search bar)
	'''

	location 				= django_filters.CharFilter(label='Location:', lookup_expr='icontains')
	irradiation_particle 	= django_filters.CharFilter(label='Particle:', lookup_expr='icontains')
	fluence_or_dose 		= django_filters.NumberFilter(label='Fluence/Dose:', lookup_expr='iexact')
	irradiation_datetime 	= django_filters.DateTimeFilter(label='Date and time:', lookup_expr='iexact')
	energy_magnitude 		= django_filters.NumberFilter(label='Energy magnitude:', lookup_expr='iexact')
	hardness_factor 		= django_filters.NumberFilter(label='Hardness factor:', lookup_expr='iexact')

	class Meta:
		model 	= Irradiation
		fields	= ['location', 'irradiation_particle', 'fluence_or_dose', 'irradiation_datetime',
					'energy_magnitude', 'hardness_factor', ]