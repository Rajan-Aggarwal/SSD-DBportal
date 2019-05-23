from django.shortcuts import render
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django_tables2 import RequestConfig
from .models import Detector
from .tables import DetectorTable
from .filters import DetectorFilter


PER_PAGE_ROWS = 25

# Create your views here.

@login_required(login_url='login/')
def home(request):
	detectors 				= Detector.objects.all()
	filtered_detectors		= DetectorFilter(request.GET, queryset=detectors)
	detectors_table 		= DetectorTable(filtered_detectors.qs)
	RequestConfig(request, paginate={'per_page': PER_PAGE_ROWS}).configure(detectors_table)
	return render(request, 'home.html', {'detectors_table': detectors_table})

def logout_view(request):
	logout(request)
	return render(request, 'logout.html')