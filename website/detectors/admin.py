from django.contrib import admin
from .models import Detector, LocationTransfer, Irradiation, Annealing
# Register your models here.

class LocationTransferModelAdmin(admin.ModelAdmin):
	'''
		custom admin model for location transfer
		---> adds the read-only field source location 
	'''
	fields 			= ['detector_id', 
						'transfer_datetime', 
						'source_location',
						'destination_location',
						'internal_or_external',
						'responsible_party',
						'comment',
						]
	readonly_fields = ['source_location']


admin.site.register(Detector)
admin.site.register(LocationTransfer, LocationTransferModelAdmin)
admin.site.register(Irradiation)
admin.site.register(Annealing)
