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
from detectors.views import logout_view, home
from detectors.forms import LoginForm

# customize the admin page
admin.site.site_header 	= 'SSD Administration'
admin.site.site_title 	= 'SSD Administration'
admin.site.index_title	= ''

# url routing end-points 
urlpatterns = [
    url(r'^admin/', admin.site.urls, name='admin'),
    url(r'^login/$', LoginView.as_view(), {'template_name': 'login.html', 'authentication_form': LoginForm}, name='user_login'),
    url(r'^logout/$', logout_view, name='user_logout'),
    url(r'', home, name='home'),
    url(r'^search/$', home, name='search')
]