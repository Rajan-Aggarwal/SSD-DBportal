from django.contrib.auth.forms import AuthenticationForm
from django import forms


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
