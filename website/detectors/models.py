from django.db import models
from django.shortcuts import get_object_or_404
from datetime import date, datetime

# by default each field is mandatory
# says blank=True, otherwise

class Detector(models.Model):
	'''
		refers to the main table
	'''
	# detector properties
	id 				= models.CharField('Detector name or ID', max_length=256, primary_key=True) # sensor name 
	producer 		= models.CharField(max_length=256)
	project 		= models.CharField(max_length=256)
	bulk_type 		= models.CharField(max_length=256)
	type 			= models.CharField(max_length=256)
	run_number 		= models.CharField(max_length=256)
	wafer_number 	= models.CharField(max_length=256)
	trec_id 		= models.CharField('TREC ID', max_length=256)

	# detector characteristics
	area 					= models.FloatField('Area (in cm sq)', blank=True, null=True)
	thickness 				= models.FloatField('Thickness (in micrometer)', blank=True, null=True)
	support_wafer_thickness = models.FloatField('Support wafer thickness (in micrometer)', blank=True, null=True)
	resistivity 			= models.FloatField('Resistivity (in ohm-cm)',blank=True, null=True)

	# detector status
	dead_or_alive = models.CharField(max_length=1, choices= [
											('D', 'Dead'), ('A', 'Alive')])
	# local information 
	ssd_responsible 	= models.CharField('Person responsible for the detector',max_length=256)
	arrival_date 		= models.DateField(default=date.today)
	current_location 	= models.CharField(max_length=256)

	# description
	comment 			= models.TextField(max_length=1024, blank=True)

	def __str__(self):
		return self.id

class LocationTransfer(models.Model):
	'''
		table storing a detector's location history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	transfer_datetime 		= models.DateTimeField('Date and time', default=datetime.now)
	source_location 		= models.CharField('From', max_length=256, editable=False)	
	# non-editable since source location can only be the current location of the detector
	destination_location 	= models.CharField('To', max_length=256)
	internal_or_external 	= models.CharField(max_length=10, choices = [
												('External', 'External'),
												('Internal', 'Internal')])
	responsible_party 		= models.CharField('Person undertaking the transfer', max_length=256, blank=True)
	comment		 			= models.TextField(max_length=1024, blank=True)

	def __str__(self):
		return "Transfer of {} to {}".format(self.detector_id, self.destination_location)

	def save(self, *args, **kwargs):
		'''
			overriding the save method:
			--> makes the source location same as the current_location of the pertinent detector
			--> call the save method
			--> changes the current_location of detector, once a location transfer is done
		'''
		self.source_location 	= Detector.objects.get(pk=self.detector_id).current_location			# make the source same as current location
		location_transfer 		= super(LocationTransfer, self).save(*args, **kwargs)
		Detector.objects.filter(pk=self.detector_id).update(current_location=self.destination_location) 
		# update the detector's current location to destination location
		return location_transfer


class Annealing(models.Model):
	'''
		table storing a detector's annealing history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	annealing_datetime 		= models.DateTimeField('Date and time', default=datetime.now)
	temperature 			= models.FloatField('Temp (in Celcius)')
	time 					= models.FloatField('Time (in minutes)')

	def __str__(self):
		return "Annealing of {} on {}".format(self.detector_id, self.annealing_datetime)

class Irradiation(models.Model):
	'''
		table storing a detector's irradiation history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	location 				= models.CharField(max_length=256)
	irradiation_particle 	= models.CharField('Particle used for radiation', max_length=256)
	fluence_or_dose 		= models.DecimalField(max_digits=20, decimal_places=10)
	energy_magnitude 		= models.FloatField('Energy')
	energy_unit 			= models.CharField('Unit of energy', max_length=10)
	irradiation_datetime 	= models.DateTimeField('Date and time', default=datetime.now)

	def __str__(self):
		return "Irradiation of {} on {} with {}".format(self.detector_id, 
													self.irradiation_datetime,
													self.irradiation_particle)