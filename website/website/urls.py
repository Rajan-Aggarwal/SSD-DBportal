"""website URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""

from django.contrib import admin
from django.conf.urls import url
from django.contrib.auth.views import LoginView
from detectors.views import (logout_view, more, export_detectors, export_detectors_csv,
        export_detectors_xls, )                                                                 # function-based views
from detectors.views import DetectorView, LocationTransferView, AnnealingView, IrradiationView  # class-based views
from detectors.forms import LoginForm

# customize the admin page
admin.site.site_header 	= 'SSD Administration'
admin.site.site_title 	= 'SSD Administration'
admin.site.index_title	= ''

# url routing end-points 
urlpatterns = [

    # main admin page
    url(r'^admin/', admin.site.urls, name='admin'),

    # login page for the normal user
    url(r'^login/$', LoginView.as_view(), {'template_name': 'login.html', 'authentication_form': LoginForm}, name='user_login'),

    # logout message page for the normal user
    url(r'^logout/$', logout_view, name='user_logout'),

    # page showing more info about each detector
    url(r'^more/(?P<detector_id>\w+)/$', more, name='more'),

    # page showing the table enlisting the location transfer history of a given detector
    url(r'^more/(?P<detector_id>\w+)/lt$', LocationTransferView.as_view(), name='location_transfer'),

    # page showing the table enlisting the annealing history of a given detector
    url(r'^more/(?P<detector_id>\w+)/an$', AnnealingView.as_view(), name='annealing'),

    # page showing the table enlisting the irradiation history of a given detector
    url(r'^more/(?P<detector_id>\w+)/ir$', IrradiationView.as_view(), name='irradiation'),

    # home page enlisting all the detectors in the database 
    url(r'^$', DetectorView.as_view(), name='home'),

    # route enabling the search filter on each of the tables 
    url(r'^search/$', DetectorView.as_view(), name='search'),

    # route displaying options to export detectors 
    url(r'^export_det/$', export_detectors, name='export_detectors'),

    # route enabling the export of detectors table into csv
    url(r'^export_det/csv/$', export_detectors_csv, name='export_detectors_csv'),

    # route enabling the export of detectors table into xls
    url(r'^export_det/xls/$', export_detectors_xls, name='export_detectors_xls'), 

]