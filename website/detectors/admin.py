from django.contrib import admin
from import_export.admin import ImportExportModelAdmin
from .models import Detector, LocationTransfer, Irradiation, Annealing
from .forms import DetectorForm

# create your custom admin models here

class DetectorModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for detectors
		---> adds search bar and its fields
		---> adds export and import options
		---> reorder the object list
		---> allow 'save as new' functionality to duplicate a record
	'''
	form 			= DetectorForm
	search_fields 	= ('id',)
	save_as 		= True		


class LocationTransferModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for location transfer
		---> adds the read-only field source location 
		---> adds search bar and its fields
		---> adds export and import options
		---> allow 'save as new' functionality to duplicate a record
	'''

	fields 			= ['detector_id', 
						'transfer_date', 
						'source_location',
						'destination_location',
						'internal_or_external',
						'responsible_party',
						'comment',
						]
	readonly_fields = ['source_location']
	search_fields 	= ('detector_id__id', 
						'transfer_date',
						'source_location',
						'destination_location',
						'internal_or_external',
						'responsible_party',
						)
	save_as 		= True


class AnnealingModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for annealings 
		---> adds search bar and its fields
		---> adds export and import options
		---> allow 'save as new' functionality to duplicate a record
	'''
	search_fields 	= ('detector_id__id',
						'annealing_date',
						'temperature',
						'time',
						)
	save_as 		= True


class IrradiationModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for irradiations
		---> adds search bar and its fields
		---> adds export and import options
		---> allow 'save as new' functionality to duplicate a record
	'''
	search_fields 	= ('detector_id__id',
						'location',
						'irradiation_particle',
						'irradiation_date',
						)
	save_as 		= True


# register your models here
# admin.site.register(ModelName, ModelAdminName[optional])
admin.site.register(Detector, DetectorModelAdmin)
admin.site.register(LocationTransfer, LocationTransferModelAdmin)
admin.site.register(Annealing, AnnealingModelAdmin)
admin.site.register(Irradiation, IrradiationModelAdmin)

