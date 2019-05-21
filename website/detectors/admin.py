from django.contrib import admin
from .models import Detector, LocationTransfer, Irradiation, Annealing
# Register your models here.

admin.site.register(Detector)
admin.site.register(LocationTransfer)
admin.site.register(Irradiation)
admin.site.register(Annealing)
