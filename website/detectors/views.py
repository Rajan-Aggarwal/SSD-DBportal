from django.shortcuts import render, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django_tables2 import RequestConfig
from .models import Detector, LocationTransfer, Annealing, Irradiation
from .tables import DetectorTable, LocationTransferTable, AnnealingTable, IrradiationTable
from .filters import DetectorFilter, LocationTransferFilter, AnnealingFilter, IrradiationFilter
from django.http import HttpResponse
from django.views import View
from django.views.generic.base import TemplateView
from django.views.generic.list import ListView
from django.contrib.auth.mixins import LoginRequiredMixin
from django_filters.views import FilterView
from django_tables2.views import SingleTableMixin
import csv

PER_PAGE_ROWS = 25

# Create your class-based views here.

class DetectorView(LoginRequiredMixin, SingleTableMixin, FilterView):

	'''
		generic view for the detector table
			--> generates a table (use render_table in the template) using django-tables2
			--> creates a small filter for you using django-filters
			--> paginates using django-tables2
	'''

	table_class 		= DetectorTable
	model 				= Detector
	template_name 		= 'home.html'
	filterset_class		= DetectorFilter
	paginate_by 		= 20


class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView):

	'''
		(This is an abstract view class: is not used for any routes)
		generic view for the all the tables in the more information section for each detector
			--> generates a table (use render_table in the template) using django-tables2
			--> creates a small filter using django-filters
			--> paginates using django-tables2
	'''

	paginate_by = 20 # set pagination rows to 20

	def get(self, request, **kwargs):
		'''
			override get in order to get the url parameter i.e detector_id
		'''
		self.detector_id = self.kwargs['detector_id']
		return super(GenericDetectorInfoTableView, self).get(self, request, **kwargs)

	def get_queryset(self):
		'''
			override get_queryset in order to filter the queryset according to the url parameter i.e detector_id
		'''
		queryset = super(GenericDetectorInfoTableView, self).get_queryset()
		queryset = queryset.filter(detector_id=self.detector_id)
		return queryset

	def get_context_data(self, **kwargs):
		'''
			override get_context_data to provide an extra context of the detector's id
		'''
		context = super(GenericDetectorInfoTableView, self).get_context_data(**kwargs)
		context.update({'detector_id': self.detector_id})
		return context


class LocationTransferView(GenericDetectorInfoTableView):

	'''
		generic view for the location transfer table
	'''

	table_class			= LocationTransferTable
	model 				= LocationTransfer
	template_name		= 'location_transfer.html'
	filterset_class		= LocationTransferFilter


class AnnealingView(GenericDetectorInfoTableView):

	'''
		generic view for the annealing table
	'''

	table_class			= AnnealingTable
	model 				= Annealing
	template_name		= 'annealing.html'
	filterset_class		= AnnealingFilter


class IrradiationView(GenericDetectorInfoTableView):

	'''
		generic view for the irradiation table
	'''

	table_class			= IrradiationTable
	model 				= Irradiation
	template_name		= 'irradiation.html'
	filterset_class		= LocationTransferFilter


# Create your function-based views here

@login_required(login_url='login/')
def more(request, detector_id):
	'''
		::param request is the user request 
		::param detector_id is the url parameter
		view displaying options for each detector entry in the main table
		leads to different tables and data corresponding to the detector entry for which option "More" was selected
	'''
	template_name = 'more.html'
	return render(request, template_name, {'detector_id': detector_id})
	

def logout_view(request):
	'''
		::param request is the user request
		logout functionality rendering the logout page
	'''
	template_name = 'logout.html'
	logout(request)
	return render(request, template_name)


@login_required(login_url='login/')
def export_detectors(request):
	'''
		::param request is the user request
		to allow a normal to export the whole 
		table of detectors without being an admin
	'''
	response	= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="detectors.csv'

	# write the csv file
	writer 		= csv.writer(response)
	writer.writerow(['id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment']) # write the first row

	detectors 	= Detector.objects.all().values_list('id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment') 	# get all the detectors
	for det in detectors:
		writer.writerow(det) 			# write all the remaining rows

	return response
