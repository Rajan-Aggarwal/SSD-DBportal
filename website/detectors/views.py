from django.shortcuts import render, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django.core.paginator import Paginator
from django_tables2 import RequestConfig
from .models import Detector, LocationTransfer, Annealing, Irradiation
from .tables import DetectorTable, LocationTransferTable, AnnealingTable, IrradiationTable
from .filters import DetectorFilter, LocationTransferFilter, AnnealingFilter, IrradiationFilter
from .utils import (get_ner_of_meas, get_list_of_datetimes, create_measurement_pdf,
	get_ner_of_meas_tct, get_list_of_files_tct, )
from django.http import HttpResponse, FileResponse, Http404
from django.views import View
from django.views.generic.base import TemplateView
from django.views.generic.list import ListView
from django.contrib.auth.mixins import LoginRequiredMixin
from django_filters.views import FilterView
from django_tables2.views import SingleTableMixin
from datetime import datetime
import csv
import xlwt
from pathlib import Path
import getpass

PER_PAGE_ROWS 		= 25
PER_PAGE_READINGS 	= 50

# Create your class-based views here.

class DetectorView(LoginRequiredMixin, SingleTableMixin, FilterView):

	'''
		class DetectorView(LoginRequieredMixin, SingleTableMixin, FilterView)

		A generic view for the detector table showing the Detector table/model.
		This view inherits 3 classes: (1) LoginRequiredMixin for login restriction, 
		(2) SingleTableMixin to publish the table, and
		(3) FilterView to add filters.

		It generates a table (use render_table in the template) using django-tables2.

			--> creates a small filter using django-filters

			--> paginates using django-tables2
	'''

	table_class 		= DetectorTable
	model 				= Detector
	template_name 		= 'home.html'
	filterset_class		= DetectorFilter
	paginate_by 		= PER_PAGE_ROWS


class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView):

	'''
		class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView)

		IMPORTANT NOTE: This is an abstract view class: is not used for any actual routes

		A generic class-based abstract view for the all the tables in the more information section for each detector.
		This view inherits 3 classes: (1) LoginRequiredMixin for login restriction,
		(2) SingleTableMixin to publish the table, and
		(3) FilterView to add filters.


		It generates a table (use render_table in the template) using django-tables2.

			--> creates a small filter using django-filters

			--> paginates using django-tables2

		It overrides:

			--> get method to get the url param (GET) detector_id

			--> get_query_set method to filter the table rows acc. to the detector_id

			--> get_context_data to update the context sent to the template
	'''

	paginate_by = PER_PAGE_ROWS 

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
		class LocationTransferView(GenericDetectorInfoTableView)

		A generic class-based view for to show the LocationTransfer table rows of a given detector id. 
		It inherits GenericDetectorInfoTableView whose description is as follows: 

			class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView)

			IMPORTANT NOTE: This is an abstract view class: is not used for any actual routes

			A generic class-based abstract view for the all the tables in the more information section for each detector.
			This view inherits 3 classes: (1) LoginRequiredMixin for login restriction,
			(2) SingleTableMixin to publish the table, and
			(3) FilterView to add filters.


			It generates a table (use render_table in the template) using django-tables2.

				--> creates a small filter using django-filters

				--> paginates using django-tables2

			It overrides:

				--> get method to get the url param (GET) detector_id

				--> get_query_set method to filter the table rows acc. to the detector_id

				--> get_context_data to update the context sent to the template
	'''

	table_class			= LocationTransferTable
	model 				= LocationTransfer
	template_name		= 'location_transfer.html'
	filterset_class		= LocationTransferFilter


class AnnealingView(GenericDetectorInfoTableView):

	'''
		class AnnealingView(GenericDetectorInfoTableView)

		A generic class-based view for to show the Annealing table rows of a given detector id. 
		It inherits GenericDetectorInfoTableView whose description is as follows: 

			class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView)

			IMPORTANT NOTE: This is an abstract view class: is not used for any actual routes

			A generic class-based abstract view for the all the tables in the more information section for each detector.
			This view inherits 3 classes: (1) LoginRequiredMixin for login restriction,
			(2) SingleTableMixin to publish the table, and
			(3) FilterView to add filters.


			It generates a table (use render_table in the template) using django-tables2.

				--> creates a small filter using django-filters

				--> paginates using django-tables2

			It overrides:

				--> get method to get the url param (GET) detector_id

				--> get_query_set method to filter the table rows acc. to the detector_id

				--> get_context_data to update the context sent to the template
	'''

	table_class			= AnnealingTable
	model 				= Annealing
	template_name		= 'annealing.html'
	filterset_class		= AnnealingFilter


class IrradiationView(GenericDetectorInfoTableView):

	'''
		class IrradiationView(GenericDetectorInfoTableView)

		A generic class-based view for to show the Irradiation table rows of a given detector id. 
		It inherits GenericDetectorInfoTableView whose description is as follows: 

			class GenericDetectorInfoTableView(LoginRequiredMixin, SingleTableMixin, FilterView)

			IMPORTANT NOTE: This is an abstract view class: is not used for any actual routes

			A generic class-based abstract view for the all the tables in the more information section for each detector.
			This view inherits 3 classes: (1) LoginRequiredMixin for login restriction,
			(2) SingleTableMixin to publish the table, and
			(3) FilterView to add filters.


			It generates a table (use render_table in the template) using django-tables2.

				--> creates a small filter using django-filters

				--> paginates using django-tables2

			It overrides:

				--> get method to get the url param (GET) detector_id

				--> get_query_set method to filter the table rows acc. to the detector_id

				--> get_context_data to update the context sent to the template
	'''

	table_class			= IrradiationTable
	model 				= Irradiation
	template_name		= 'irradiation.html'
	filterset_class		= IrradiationFilter


# Create your function-based views here

@login_required(login_url='login/')
def more(request, detector_id):
	'''
		def more(request, detector_id)

		::param request is the http user request 

		::param detector_id is the url parameter

		A function-based view displaying options for each detector entry in the main table. 
		One can select to view annealing history etc of a given detector. 
	'''
	template_name = 'more.html'
	return render(request, template_name, {'detector_id': detector_id})
	

def logout_view(request):
	'''
		def logout_view(request)

		::param request is the http user request

		A function-based view to give the logout functionality 
		by deleting the session and rendering the logout page.
	'''
	template_name = 'logout.html'
	logout(request)
	return render(request, template_name)


@login_required(login_url='login/')
def export_detectors(request):
	'''
		def export_detectors(request)

		::param request is the http user request

		A function-based view to show the options to export Detector
		table in different formats.
	'''
	template_name = 'export/export_detectors.html'
	print("hallelujah")
	return render(request, template_name)


@login_required(login_url='login/')
def export_detectors_csv(request):
	'''
		def export_detectors_csv(request)

		::param request is the http user request

		A function-based view to export the Detector table in the csv format.
	'''
	response = HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="detectors.csv"'

	# write the csv file
	writer = csv.writer(response)
	# write the first row
	writer.writerow(['id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
		'wafer_number', 'trec_id', 'area', 'thickness', 'support_wafer_thickness',
		'resistivity', 'dead_or_alive', 'ssd_responsible', 'arrival_date', 
		'current_location', 'comment']) 

	# get all the detectors from the model
	detectors = Detector.objects.all().values_list('id', 'producer', 'project', 'bulk_type', 'type', 'run_number',
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
		def export_detectors_xls(request)

		::param request is the http user request

		A function-based view to export the Detector table in the xls format.
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
		def export_location_transfer(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to show the options to export LocationTransfer
		rows of a given detector_id in different formats.
	'''
	template_name = 'export/export_location_transfers.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_location_transfers_csv(request, detector_id):
	'''
		def export_location_transfers_csv(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to export the LocationTransfer rows of a given detector_id
		in the csv format.
	'''
	response = HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="location_transfers-{}.csv"'.format(detector_id)

	# write the csv file
	writer 	= csv.writer(response)
	# write the first row
	writer.writerow(['transfer_date', 'source_location', 'destination_location', 'internal_or_external',
		'responsible_party', 'comment'])

	# get data from the model
	location_transfers = LocationTransfer.objects.filter(detector_id=detector_id).values_list('transfer_date', 
		'source_location', 'destination_location', 'internal_or_external', 'responsible_party', 'comment')
	# write all remaining rows
	for lt in location_transfers:
		writer.writerow(lt)

	return response


@login_required(login_url='login/')
def export_location_transfers_xls(request, detector_id):
	'''
		def export_location_transfers_xls(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to export LocationTransfer rows of a given detector_id
		in the xls format.
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="location_transfers-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Location transfers')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['transfer_date', 'source_location', 'destination_location', 'internal_or_external',
		'responsible_party', 'comment']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()
	date_style 	= xlwt.XFStyle()
	date_style.num_format_str = 'DD-MMM-YY'


	# get data from the model
	rows 		= LocationTransfer.objects.filter(detector_id=detector_id).values_list('transfer_date', 'source_location', 
		'destination_location', 'internal_or_external', 'responsible_party', 'comment')
	# write each cell
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			if columns[col_num] == 'transfer_date':
				ws.write(row_num, col_num, row[col_num], date_style)
			else:
				ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


@login_required(login_url='login/')
def export_annealings(request, detector_id):
	'''
		def export_annealings(request, detector_id)

		::param request is the http use request

		::param detector_id the url param (GET)

		A function-based view to show the options to export the
		Annealing table rows for a given detector_id in different
		formats. 
	'''
	template_name = 'export/export_annealings.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_annealings_csv(request, detector_id):
	'''
		def export_annealings_csv(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to export the Annealing table rows for a given detector_id
		in the csv format.
	'''
	response 	= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="annealings-{}.csv"'.format(detector_id)

	# write the csv file
	writer 		= csv.writer(response)
	# write the first row
	writer.writerow(['annealing_date', 'temperature', 'time'])

	# get data from the model
	annealings 	= Annealing.objects.filter(detector_id=detector_id).values_list('annealing_date', 
		'temperature', 'time')
	# write all remaining rows
	for ann in annealings:
		writer.writerow(ann)

	return response


@login_required(login_url='login/')
def export_annealings_xls(request, detector_id):
	'''
		def export_annealings_xls(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)
		
		A function-based view to export the Annealing table rows of a given detector_id
		in the xls format. 
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="annealings-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Annealing history')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['annealing_date', 'temperature', 'time']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()
	date_style 	= xlwt.XFStyle()
	date_style.num_format_str = 'DD-MMM-YY'

	# get data from the model
	rows 		= Annealing.objects.filter(detector_id=detector_id).values_list('annealing_date', 
		'temperature', 'time')
	# write each cell
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			if columns[col_num] == 'annealing_date':
				ws.write(row_num, col_num, row[col_num], date_style)
			else:
				ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


@login_required(login_url='login/')
def export_irradiations(request, detector_id):
	'''
		def export_irradiations(request, detector_id)

		::param request is the http use request

		::param detector_id the url param (GET)

		A function-based view to show the options to export the Irradiation 
		table rows of the given detector_id in different formats.
	'''
	template_name = 'export/export_irradiations.html'
	return render(request, template_name, {'detector_id': detector_id})


@login_required(login_url='login/')
def export_irradiations_csv(request, detector_id):
	'''
		def export_irradiations_csv(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to export the Irradiation table rows of the given detector_id 
		in the csv format.
	'''
	response 		= HttpResponse(content_type='text/csv')
	response['Content-Disposition'] = 'attachment; filename="irradiations-{}.csv"'.format(detector_id)

	# write the csv file
	writer 			= csv.writer(response)
	# write the first row
	writer.writerow(['location', 'irradiation_particle', 'fluence_or_dose', 'fd_unit', 'energy_magnitude',
		'energy_unit', 'hardness_factor', 'irradiation_date'])

	# get data from the model
	irradiations 	= Irradiation.objects.filter(detector_id=detector_id).values_list('location', 'irradiation_particle', 
		'fluence_or_dose', 'fd_unit', 'energy_magnitude', 'energy_unit', 'hardness_factor', 'irradiation_date')
	# write all remaining rows
	for irr in irradiations:
		writer.writerow(irr)

	return response


@login_required(login_url='login/')
def export_irradiations_xls(request, detector_id):
	'''
		def export_irradiations_xls(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to export the Irradiation table rows of the given detector_id
		in the xls format.
	'''
	response 	= HttpResponse(content_type='application/ms-excel')
	response['Content-Disposition'] = 'attachment; filename="irradiations-{}.xls"'.format(detector_id)

	wb 			= xlwt.Workbook(encoding='utf-8')
	ws 			= wb.add_sheet('Irradiaton history')

	# sheet header, first row
	row_num 	= 0

	font_style				= xlwt.XFStyle()
	font_style.font.bold 	= True

	columns		= ['location', 'irradiation_particle', 'fluence_or_dose', 'fd_unit', 'energy_magnitude',
		'energy_unit', 'hardness_factor', 'irradiation_date']

	for col_num in range(len(columns)):
		ws.write(row_num, col_num, columns[col_num], font_style)

	# sheet body, remaining rows
	font_style	= xlwt.XFStyle()
	date_style 	= xlwt.XFStyle()
	date_style.num_format_str = 'DD-MMM-YY'

	# get data from the model
	rows 		= Irradiation.objects.filter(detector_id=detector_id).values_list('location', 
		'irradiation_particle', 'fluence_or_dose', 'fd_unit', 'energy_magnitude','energy_unit', 'hardness_factor', 
		'irradiation_date')
	# write each cell
	for row in rows:
		row_num += 1
		for col_num in range(len(row)):
			if columns[col_num] == 'irradiation_date':
				ws.write(row_num, col_num, row[col_num], date_style)
			else:
				ws.write(row_num, col_num, row[col_num], font_style)

	wb.save(response)
	return response


@login_required(login_url='login/')
def measurement_index(request, detector_id):
	'''
		def measurement_index(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to show the different kinds of measurements
		done on detector_id with the number of each measurements (given
		by root scripts)
	'''
	template_name 		= 'measurement_index.html'
	measurement_dict 	= {
							'CV': get_ner_of_meas(detector_id, 'cv'),
							'IV': get_ner_of_meas(detector_id, 'iv'),
							'TCT': get_ner_of_meas_tct(detector_id, 'tct'),
						}
	context 			= {
							'detector_id': detector_id,
							'measurement_dict': measurement_dict,
						}
	return render(request, template_name, context)


@login_required(login_url='login/')
def measurement_list(request, detector_id, meastype):
	'''
		def measurement_list(request, detector_id, meastype)

		::param request is the http user request

		::param detector_id is the url param (GET)

		::param meastype is the url param (GET) suggesting
		the type of measurement list the user wants

		A function-based view to show the date and time (clickable to download a file)
		of each measurement on detector 'detector_id' and of type 'meastype'
	'''
	template_name 		= 'measurement_list.html'
	datetime_list 		= get_list_of_datetimes(detector_id, meastype)

	# add pagination
	paginator 			= Paginator(datetime_list, PER_PAGE_READINGS)
	page 				= request.GET.get('page')

	if datetime_list is not None:
		pag_datetimes	= paginator.get_page(page)
	else:
		pag_datetimes 	= None 

	context 			= {
							'detector_id': detector_id,
							'meastype': meastype,
							'datetime_list': pag_datetimes,
							'no_entry_msg': 'No {} measurements available' \
									' for this detector'.format(meastype.upper()),
						}

	return render(request, template_name, context)


@login_required(login_url='login/')
def get_measurement(request, detector_id, meastype, datetime):
	'''
		def get_measurement(request, detector_id, type, datetime)

		::param request is the http user request

		::param detector_id is the url param (GET)

		::param type is the url param (GET) suggesting the
		type of measurement it is

		::param datetime is the url param(GET)

		A function-based view to open the measurement file in a new tab
		when clicked on a datetime link in the measurement_list page
		of each type
	'''
	pdf_name = "{}{}{}.pdf".format(detector_id,meastype,datetime)

	if getpass.getuser() == 'root':
		pdf_path = '/home/ubuntu/ssd-dbportal/tmp/pdfs/'
	else:
		pdf_path = '/home/{}/ssd-dbportal/tmp/pdfs/'.format(getpass.getuser())
		
	pdf_file = Path('{}{}'.format(pdf_path, pdf_name))

	if not pdf_file.is_file():
		create_measurement_pdf(detector_id, meastype, datetime)

	return FileResponse(open('{}{}'.format(pdf_path, pdf_name), 'rb'), 
			content_type='application/pdf')



@login_required(login_url='login/')
def tct_index(request, detector_id):
	'''
		def tct_index(request, detector_id)

		::param request is the http user request

		::param detector_id is the url param (GET)

		A function-based view to show to different kinds of tct
		measurements available for a detector with id 'detector_id' 
	'''
	template_name 		= 'tct_index.html'

	measurement_dict 	= { 
							'Red_Top_TCT': get_ner_of_meas_tct(detector_id, 
								'red_top_tct'),
							'Red_Bottom_TCT': get_ner_of_meas_tct(detector_id,
								'red_bottom_tct'),
							'IR_Top_TCT': get_ner_of_meas_tct(detector_id,
								'ir_top_tct'),
							'IR_Bottom_TCT': get_ner_of_meas_tct(detector_id,
								'ir_bottom_tct'),
							'Edge_TCT': get_ner_of_meas_tct(detector_id, 'edge_tct'),
							'TPA_TCT': get_ner_of_meas_tct(detector_id, 'tpa_tct'),
							'Timing': get_ner_of_meas_tct(detector_id, 'timing'),
						}
	context 			= {
							'detector_id': detector_id,
							'measurement_dict': measurement_dict
						}

	return render(request, template_name, context)


@login_required(login_url='login/')
def tct_list(request, detector_id, meastype):
	'''
		def tct_list(request, detector_id, meastype)

		::param request is the http user request

		::param detector_id is the url param (GET)

		::param meastype is the url param (GET) suggesting
		the type of tct measurement list the user wants

		A function-based view to show the name of file (clickable to download a file)
		of each measurement on detector 'detector_id' and of type 'meastype'
	'''
	template_name 		= 'tct_list.html'
	file_list 			= get_list_of_files_tct(detector_id, meastype)

	# add pagination
	paginator 			= Paginator(file_list, PER_PAGE_READINGS)
	page 				= request.GET.get('page')

	if file_list is not None:
		pag_files 		= paginator.get_page(page)
	else:
		pag_files		= None

	context 			= {
							'detector_id': detector_id,
							'meastype': meastype,
							'file_list': pag_files,
							'no_entry_msg': 'No {} measurements available' \
								' for this detector'.format(meastype.upper().replace('_', ' ')),
						}

	return render(request, template_name, context)
