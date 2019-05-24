from django.shortcuts import render
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django_tables2 import RequestConfig
from .models import Detector
from .tables import DetectorTable
from .filters import DetectorFilter

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

def logout_view(request):
	logout(request)
	return render(request, 'logout.html')

@login_required(login_url='login/')
def more(request, detector_id):
	return render(request, 'more.html', {'detector_id': detector_id})