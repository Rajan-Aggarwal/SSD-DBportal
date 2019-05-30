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
import xlwt
import datetime 

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
	filterset_class		= IrradiationFilter


# Create your function-based views here

@login_required(login_url='login/')
def more(request, detector_id):
	'''
		::param request is the http user request 
		::param detector_id is the url parameter
		view displaying options for each detector entry in the main table
		leads to different tables and data corresponding to the detector entry for which option "More" was selected
	'''
	template_name = 'more.html'
	return render(request, template_name, {'detector_id': detector_id})
	

def logout_view(request):
	'''
		::param request is the http user request
		logout functionality rendering the logout page
	'''
	template_name = 'logout.html'
	logout(request)
	return render(request, template_name)


@login_required(login_url='login/')
def export_detectors(request):
	'''
		::param request is the http user request
		to show the options to export the detector
		table in different formats, to a normal user
	'''
	template_name = 'export/export_detectors.html'
	print("hallelujah")
	return render(request, template_name)


@login_required(login_url='login/')
def export_detectors_csv(request):
	'''
		::param request is the http user request
		export in csv format
	'''
	response	= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="detectors.csv"'

	# write the csv file
	writer 		= csv.writer(response)
	# write the first row
	writer.writerow(['id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment']) 

	# get all the detectors from the model
	detectors 	= Detector.objects.all().values_list('id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment') 
	# write all the remaining rows
	for det in detectors:
		writer.writerow(det) 			

	return response


@login_required(login_url='login/')
def export_detectors_xls(request):
	'''
		::param request is the http user request
		export in xls format
	'''
	response	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="detectors.xls"'

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Detectors')

	# sheet header, first row
	row_num 	= 0

	font_style 				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns 	= ['id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()
	date_style 	= xlwt.XFStyle()
	date_style.num_format_str = 'D-MMM-YY'
	# get all the detectors
	rows 		= Detector.objects.all().values_list('id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment')								
	# write each cell
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			if columns[col_num] == 'arrival_date':
				ws.write(row_num, col_num, row[col_num], date_style)
			else:
				ws.write(row_num, col_num, row[col_num], font_style) 	

	wb.save(response)
	return response


@login_required(login_url='login/')
def export_location_transfers(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		to show the options to export the location 
		history of a single detector in different
		formats, to a normal user
	'''
	template_name = 'export/export_location_transfers.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_location_transfers_csv(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in csv format
	'''
	response 	= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="location_transfers-{}.csv"'.format(detector_id)

	# write the csv file
	writer 		= csv.writer(response)
	# write the first row
	writer.writerow(['transfer_datetime', 'source_location', 'destination_location', 'internal_or_external',
		'responsible_party', 'comment'])

	# get data from the model
	location_transfers 			= LocationTransfer.objects.filter(detector_id=detector_id).values_list('transfer_datetime', 
		'source_location', 'destination_location', 'internal_or_external', 'responsible_party', 'comment')
	# write all remaining rows
	for lt in location_transfers:
		writer.writerow(lt)

	return response


@login_required(login_url='login/')
def export_location_transfers_xls(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in xls format
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="location_transfers-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Location transfers')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['transfer_datetime', 'source_location', 'destination_location', 'internal_or_external',
		'responsible_party', 'comment']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()

	# get data from the model
	rows 		= LocationTransfer.objects.filter(detector_id=detector_id).values_list('transfer_datetime', 'source_location', 
		'destination_location', 'internal_or_external', 'responsible_party', 'comment')
	# write each cell
	rows = [ [ x.strftime("%Y-%m-%d %H:%M") if isinstance(x, datetime.datetime) else x for x in row ] for row in rows ]
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


@login_required(login_url='login/')
def export_annealings(request, detector_id):
	'''
		::param request is the http use request
		::param detector_id the url param (GET)
		to show the options to export the annealing 
		history of a single detector in different
		formats, to a normal user
	'''
	template_name = 'export/export_annealings.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_annealings_csv(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in csv format
	'''
	response 	= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="annealings-{}.csv"'.format(detector_id)

	# write the csv file
	writer 		= csv.writer(response)
	# write the first row
	writer.writerow(['annealing_datetime', 'temperature', 'time'])

	# get data from the model
	annealings 	= Annealing.objects.filter(detector_id=detector_id).values_list('annealing_datetime', 
		'temperature', 'time')
	# write all remaining rows
	for ann in annealings:
		writer.writerow(ann)

	return response


@login_required(login_url='login/')
def export_annealings_xls(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in xls format
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="annealings-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Annealing history')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['annealing_datetime', 'temperature', 'time']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()

	# get data from the model
	rows 		= Annealing.objects.filter(detector_id=detector_id).values_list('annealing_datetime', 
		'temperature', 'time')
	# write each cell
	rows = [ [ x.strftime("%Y-%m-%d %H:%M") if isinstance(x, datetime.datetime) else x for x in row ] for row in rows ]
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


@login_required(login_url='login/')
def export_irradiations(request, detector_id):
	'''
		::param request is the http use request
		::param detector_id the url param (GET)
		to show the options to export the irradiation 
		history of a single detector in different
		formats, to a normal user
	'''
	template_name = 'export/export_irradiations.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_irradiations_csv(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in csv format
	'''
	response 		= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="irradiations-{}.csv"'.format(detector_id)

	# write the csv file
	writer 			= csv.writer(response)
	# write the first row
	writer.writerow(['location', 'irradiation_particle', 'fluence_or_dose', 'energy_magnitude',
		'energy_unit', 'hardness_factor', 'irradiation_datetime'])

	# get data from the model
	irradiations 	= Irradiation.objects.filter(detector_id=detector_id).values_list('location', 'irradiation_particle', 
		'fluence_or_dose', 'energy_magnitude', 'energy_unit', 'hardness_factor', 'irradiation_datetime')
	# write all remaining rows
	for irr in irradiations:
		writer.writerow(irr)

	return response


@login_required(login_url='login/')
def export_irradiations_xls(request, detector_id):
	'''
		::param request is the http user request
		::param detector_id is the url param (GET)
		export in xls format
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="irradiations-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Irradiaton history')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['location', 'irradiation_particle', 'fluence_or_dose', 'energy_magnitude',
		'energy_unit', 'hardness_factor', 'irradiation_datetime']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()

	# get data from the model
	rows 		= Irradiation.objects.filter(detector_id=detector_id).values_list('location', 
		'irradiation_particle', 'fluence_or_dose', 'energy_magnitude','energy_unit', 'hardness_factor', 
		'irradiation_datetime')
	# write each cell
	rows = [ [ x.strftime("%Y-%m-%d %H:%M") if isinstance(x, datetime.datetime) else x for x in row ] for row in rows ]
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


