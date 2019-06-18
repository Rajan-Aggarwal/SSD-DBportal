from django.db import models
from django.shortcuts import get_object_or_404
from datetime import date

# create your models/database-tables here

class Detector(models.Model):
	'''
		Detector(id, producer, project, bulk_type, type, run_number, wafer_number, trec_id, area, thickness, 
		support_wafer_thickness, resistivity, dead_or_alive, ssd_responsible, arrival_date, current_location,
		comment).

		It is the main table storing all the detectors and its information. Each such detector object/row maybe
		linked with serveral other rows in LocationTransfer, Annealing or Irradiation table with a foreign-key.

		Each field is mandatory by default. Make blank=True and null=True (attributes of the field) to change that. 
		To add or update a field, refer to https://docs.djangoproject.com/en/2.2/topics/db/models/#fields.
	'''
	id 						= models.CharField('Detector name or ID', max_length=256, primary_key=True) # sensor name 
	producer 				= models.CharField(max_length=256)
	project 				= models.CharField(max_length=256)
	bulk_type 				= models.CharField(max_length=256)
	type 					= models.CharField('Sensor type', max_length=256) # refers to the sensor type
	run_number 				= models.CharField(max_length=256, blank=True, null=True)
	wafer_number 			= models.CharField(max_length=256)
	trec_id 				= models.CharField('TREC ID', max_length=256, blank=True, null=True)
	area 					= models.FloatField('Area (in cm sq)', blank=True, null=True)
	thickness 				= models.FloatField('Thickness (in micrometer)', blank=True, null=True)
	support_wafer_thickness = models.FloatField('Support wafer thickness (in micrometer)', blank=True, null=True)
	resistivity 			= models.FloatField('Resistivity (in ohm-cm)', blank=True, null=True)
	dead_or_alive 			= models.CharField('Dead/Alive', max_length=10, choices= [
																			('Dead', 'Dead'), 
																			('Alive', 'Alive'),
																			('Unknown', 'Unknown'),
																			])
	ssd_responsible 		= models.CharField('Person responsible for the detector', max_length=256)
	arrival_date 			= models.DateField(default=date.today)
	current_location 		= models.CharField(max_length=256)
	comment 				= models.TextField(max_length=1024, blank=True, null=True)

	def __str__(self):
		return self.id

	class Meta:
		ordering = ['-arrival_date']


class LocationTransfer(models.Model):
	'''
		LocationTransfer(detector_id, transfer_date, source_location, destination_location, internal_or_external,
		responsible_party, comment)

		It is the table presenting the location tranfer history of detectors. Hence, each row/object is 
		linked with the Detector table using detector_id as a foreign-key. Thus it presents a many-to-one
		relationship. A primary-key for each row is added by default by django-models. 

		Each field is mandatory by default. Make blank=True and null=True (attributes of the field) to change that. 
		To add or update a field, refer to https://docs.djangoproject.com/en/2.2/topics/db/models/#fields.

		The save method of this table is overriden. It upadates the current_location of
		the detector automatically as one saves a location transfer row. Moreover, the source_location field
		is a non-editable field (editable=False), and the source location is always same as the current location
		of the detector before it being updated by the saving of the new row.
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
	responsible_party 		= models.CharField('Person undertaking the transfer', max_length=256, blank=True, null=True)
	comment		 			= models.TextField(max_length=1024, blank=True)

	def __str__(self):
		return "Transfer of {} from {} to {} on {}".format(self.detector_id, 
														self.source_location, 
														self.destination_location, 
														self.transfer_date.strftime('%d/%m/%Y'))

	def save(self, *args, **kwargs):
		'''
			overriding the save method:
			--> makes the source location same as the current_location of the pertinent detector
			--> call the save method
			--> changes the current_location of detector, once a location transfer is done
		'''
		self.source_location 	= Detector.objects.get(pk=self.detector_id).current_location
		location_transfer 		= super(LocationTransfer, self).save(*args, **kwargs)
		Detector.objects.filter(pk=self.detector_id).update(current_location=self.destination_location) 
		return location_transfer

	class Meta:
		ordering = ['-transfer_date']


class Annealing(models.Model):
	'''
		Annealing(detector_id, annealing_date, temperature, time)

		It is the table presenting the annealing history of detectors. Hence, each row/object is linked 
		with the Detector table using the detector_id as a foreign key. Thus it presents a many-to-one 
		relationship. A primary-key for each row is added by default by django-models. 

		Each field is mandatory by default. Make blank=True and null=True (attributes of the field) to change that. 
		To add or update a field, refer to https://docs.djangoproject.com/en/2.2/topics/db/models/#fields.
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	annealing_date 			= models.DateField('Date', default=date.today)
	temperature 			= models.FloatField('Temp (in Celcius)')
	time 					= models.FloatField('Time (in minutes)')

	def __str__(self):
		return "Annealing of {} on {} at {} Celcius for {} minutes".format(self.detector_id, 
														self.annealing_date.strftime('%d/%m/%Y'),
														self.temperature,
														self.time)

	class Meta:
		ordering = ['-annealing_date']


class Irradiation(models.Model):
	'''
		Irradiation(detector_id, location, irradiation_particle, fluence_or_does, fd_unit, energy_magnitude,
		energy_unit, hardness factor, irradiation_date)

		It is the table presenting the irradiation history of detectors. Hence, each row/object is linked 
		with the Detector table using the detector_id as a foreign key. Thus it presents a many-to-one 
		relationship. A primary-key for each row is added by default by django-models.

		Each field is mandatory by default. Make blank=True and null=True (attributes of the field) to change that. 
		To add or update a field, refer to https://docs.djangoproject.com/en/2.2/topics/db/models/#fields.
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	location 				= models.CharField(max_length=256)
	irradiation_particle 	= models.CharField('Particle used for irradiation', max_length=256)
	fluence_or_dose 		= models.DecimalField('Fluence/Dose', max_digits=100, decimal_places=50)
	fd_unit 				= models.CharField('Unit of fluence/dose', max_length=10) 
	energy_magnitude 		= models.FloatField('Energy', blank=True, null=True)
	energy_unit 			= models.CharField('Unit of energy', max_length=10, blank=True, null=True)
	hardness_factor			= models.FloatField(blank=True, null=True)
	irradiation_date 		= models.DateField('Date', default=date.today)

	def __str__(self):
		return "Irradiation of {} on {} with {}".format(self.detector_id, 
													self.irradiation_date.strftime('%d/%m/%Y'),
													self.irradiation_particle)

	class Meta:
		ordering = ['-irradiation_date']
