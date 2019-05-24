from django.shortcuts import render, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django_tables2 import RequestConfig
from .models import Detector, LocationTransfer
from .tables import DetectorTable, LocationTransferTable
from .filters import DetectorFilter, LocationTransferFilter

from django.views import View
from django.views.generic.base import TemplateView
from django.views.generic.list import ListView
from django.contrib.auth.mixins import LoginRequiredMixin
from django_filters.views import FilterView
from django_tables2.views import SingleTableMixin



PER_PAGE_ROWS = 25

# Create your views here.

@login_required(login_url='login/')
def home(request):
	detectors 				= Detector.objects.all()
	filtered_detectors		= DetectorFilter(request.GET, queryset=detectors)
	detectors_table 		= DetectorTable(filtered_detectors.qs)
	RequestConfig(request, paginate={'per_page': PER_PAGE_ROWS}).configure(detectors_table)
	return render(request, 'home.html', {'detectors_table': detectors_table})

class DetectorView(SingleTableMixin, FilterView, LoginRequiredMixin):
	table_class 		= DetectorTable
	model 				= Detector
	template_name 		= 'home.html'
	filterset_class		= DetectorFilter
	paginate_by 		= 20

class LocationTransferView(SingleTableMixin, FilterView, LoginRequiredMixin):
	table_class			= LocationTransferTable
	model 				= LocationTransfer
	template_name		= 'location_transfer.html'
	filterset_class		= LocationTransferFilter
	paginate_by			= 20

	def get(self, request, **kwargs):
		self.detector_id = self.kwargs['detector_id']
		return super(LocationTransferView, self).get(self, request, **kwargs)

	def get_queryset(self):
		queryset = super(LocationTransferView, self).get_queryset()
		queryset = queryset.filter(detector_id=self.detector_id)
		return queryset

def logout_view(request):
	logout(request)
	return render(request, 'logout.html')

@login_required(login_url='login/')
def more(request, detector_id):
	return render(request, 'more.html', {'detector_id': detector_id})