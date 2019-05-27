from django.contrib import admin
from import_export.admin import ImportExportModelAdmin
from .models import Detector, LocationTransfer, Irradiation, Annealing


class DetectorModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for detectors
		---> adds search bar and its fields
	'''
	search_fields = ('id',)


class LocationTransferModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for location transfer
		---> adds the read-only field source location 
		---> adds search bar and its fields
	'''
	fields 			= ['detector_id__id', 
						'transfer_datetime__iexact', 
						'source_location',
						'destination_location',
						'internal_or_external__iexact',
						'responsible_party',
						'comment',
						]
	readonly_fields = ['source_location']
	search_fields 	= ('detector_id__id', 
						'transfer_datetime__iexact',
						'source_location',
						'destination_location',
						'internal_or_external',
						'responsible_party',
						)


class AnnealingModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for annealings 
		---> adds search bar and its fields
	'''
	search_fields = ('detector_id__id',
						'annealing_datetime__iexact',
						'temperature__startswith',
						'time__startswith',
						)


class IrradiationModelAdmin(ImportExportModelAdmin):
	'''
		custom admin model for irradiations
		---> adds search bar and its fields
	'''
	search_fields = ('detector_id__id',
						'location',
						'irradiation_particle',
						'irradiation_datetime__iexact',
						)


# register your models here
# admin.site.register(ModelName, ModelAdminName[optional])
admin.site.register(Detector, DetectorModelAdmin)
admin.site.register(LocationTransfer, LocationTransferModelAdmin)
admin.site.register(Annealing, AnnealingModelAdmin)
admin.site.register(Irradiation, IrradiationModelAdmin)

