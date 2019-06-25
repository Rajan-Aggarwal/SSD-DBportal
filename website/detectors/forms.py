from django.contrib.auth.forms import AuthenticationForm
from django import forms
from .models import Detector
from dal import autocomplete


class LoginForm(AuthenticationForm):
	'''
		Login form for a normal ssd user
	'''
	username = forms.CharField(label='Username',
				max_length=30,
				widget=forms.TextInput(attrs={'class': 'form-control', 'name': 'username'}),
				)
	password = forms.CharField(label='Password',
				max_length=30,
				widget=forms.TextInput(attrs={'class': 'form-control', 'name': 'password'}),
				)


class DetectorForm(forms.ModelForm):
	'''
		Custom form for Detector model

			---> allows autocomplete using django-autocomplete-light
	'''

	class Meta:
		model 	= Detector
		fields 	= ('__all__')
		widgets = {
			'current_location': autocomplete.ListSelect2(url='location_autocomplete'),
			'ssd_responsible': autocomplete.ListSelect2(url='person_autocomplete'),
		} 