from django.db import models
from django.shortcuts import get_object_or_404
from datetime import date

# by default each field is mandatory
# says blank=True, otherwise

class Detector(models.Model):
	'''
		refers to the main table
	'''
	id 						= models.CharField('Detector name or ID', max_length=256, primary_key=True) # sensor name 
	producer 				= models.CharField(max_length=256)
	project 				= models.CharField(max_length=256)
	bulk_type 				= models.CharField(max_length=256)
	type 					= models.CharField('Sensor type', max_length=256) # refers to the sensor type
	run_number 				= models.CharField(max_length=256)
	wafer_number 			= models.CharField(max_length=256)
	trec_id 				= models.CharField('TREC ID', max_length=256, blank=True, null=True)
	area 					= models.FloatField('Area (in cm sq)', blank=True, null=True)
	thickness 				= models.FloatField('Thickness (in micrometer)', blank=True, null=True)
	support_wafer_thickness = models.FloatField('Support wafer thickness (in micrometer)', blank=True, null=True)
	resistivity 			= models.FloatField('Resistivity (in ohm-cm)',blank=True, null=True)
	dead_or_alive 			= models.CharField('Dead/Alive', max_length=1, choices= [
																			('D', 'Dead'), 
																			('A', 'Alive'),
																			])
	ssd_responsible 		= models.CharField('Person responsible for the detector',max_length=256)
	arrival_date 			= models.DateField(default=date.today)
	current_location 		= models.CharField(max_length=256)
	comment 				= models.TextField(max_length=1024, blank=True)

	def __str__(self):
		return self.id

	class Meta:
		ordering = ['-arrival_date']


class LocationTransfer(models.Model):
	'''
		table storing a detector's location history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	transfer_date  			= models.DateField('Date', default=date.today)
	# non-editable field source_location (auto-fill as current location of the detector)
	source_location 		= models.CharField('From', default='The current location of this detector (auto-fill)', 
												max_length=256, 
												editable=False, 
												)	
	destination_location 	= models.CharField('To', max_length=256)
	internal_or_external 	= models.CharField('Internal/External', 
												max_length=10, 
												choices = [
													('External', 'External'),
													('Internal', 'Internal'),
													]
												)
	responsible_party 		= models.CharField('Person undertaking the transfer', max_length=256, blank=True)
	comment		 			= models.TextField(max_length=1024, blank=True)

	def __str__(self):
		return "Transfer of {} from {} to {} on {}".format(self.detector_id, 
														self.source_location, 
														self.destination_location, 
														self.transfer_date)

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
		return location_transfer

	class Meta:
		ordering = ['-transfer_date']


class Annealing(models.Model):
	'''
		table storing a detector's annealing history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	annealing_date 			= models.DateTimeField('Date', default=date.today)
	temperature 			= models.FloatField('Temp (in Celcius)')
	time 					= models.FloatField('Time (in minutes)')

	def __str__(self):
		return "Annealing of {} on {} at {} Celcius for {} minutes".format(self.detector_id, 
														self.annealing_date,
														self.temperature,
														self.time)

	class Meta:
		ordering = ['-annealing_date']


class Irradiation(models.Model):
	'''
		table storing a detector's irradiation history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	location 				= models.CharField(max_length=256)
	irradiation_particle 	= models.CharField('Particle used for radiation', max_length=256)
	fluence_or_dose 		= models.DecimalField('Fluence/Dose', max_digits=20, decimal_places=10)
	energy_magnitude 		= models.FloatField('Energy')
	energy_unit 			= models.CharField('Unit of energy', max_length=10)
	hardness_factor			= models.FloatField(blank=True, null=True)
	irradiation_date 		= models.DateField('Date', default=date.today)

	def __str__(self):
		return "Irradiation of {} on {} with {}".format(self.detector_id, 
													self.irradiation_date,
													self.irradiation_particle)

	class Meta:
		ordering = ['-irradiation_date']