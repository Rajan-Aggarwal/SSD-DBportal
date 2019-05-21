from django.db import models
from datetime import date, datetime

# by default each field is mandatory
# says blank=True, otherwise

class Detector(models.Model):
	'''
		refers to the main table
	'''
	# detector properties
	id 				= models.CharField(max_length=256, primary_key=True) # sensor name 
	producer 		= models.CharField(max_length=256)
	project 		= models.CharField(max_length=256)
	bulk_type 		= models.CharField(max_length=256)
	type 			= models.CharField(max_length=256)
	run_number 		= models.CharField(max_length=256)
	wafer_number 	= models.CharField(max_length=256)
	trec_id 		= models.CharField(max_length=256)
	# detector characteristics
	area 					= models.FloatField(blank=True)
	thickness 				= models.FloatField(blank=True)
	support_wafer_thickness = models.FloatField(blank=True)
	resistivity 			= models.FloatField(blank=True)
	# detector status
	dead_or_alive = models.CharField(max_length=1, choices= [
											('D', 'Dead'), ('A', 'Alive')])
	# local information 
	ssd_responsible 	= models.CharField(max_length=256)
	arrival_date 		= models.DateField(default=date.today)
	current_location 	= models.CharField(max_length=256)

	# description
	comment 			= models.TextField(max_length=1024, blank=True)

class LocationTransfer(models.Model):
	'''
		table storing a detector's location history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	transfer_datetime 		= models.DateTimeField(default=datetime.now)
	source_location 		= models.CharField('From', max_length=256)
	destination_location 	= models.CharField('To', max_length=256)
	internal_or_external 	= models.CharField(max_length=10, choices = [
												('External', 'External'),
												('Internal', 'Internal')])
	responsible_party 		= models.CharField(max_length=256, blank=True)
	comment		 			= models.TextField(max_length=1024, blank=True)

class Annealing(models.Model):
	'''
		table storing a detector's annealing history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	annealing_datetime 		= models.DateTimeField(default=datetime.now)
	temperature 			= models.FloatField()
	time 					= models.FloatField()

class Irradiation(models.Model):
	'''
		table storing a detector's irradiation history
	'''
	detector_id 			= models.ForeignKey('Detector', on_delete=models.CASCADE)
	location 				= models.CharField(max_length=256)
	irradiation_particle 	= models.CharField(max_length=256)
	fluence_or_dose 		= models.DecimalField(max_digits=20, decimal_places=10)
	energy_magnitude 		= models.FloatField()
	energy_unit 			= models.CharField(max_length=10)
	irradiation_datetime 	= models.DateTimeField(default=datetime.now)