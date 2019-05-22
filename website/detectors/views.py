from django.shortcuts import render, redirect
from django.http import HttpResponse
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout

# Create your views here.

@login_required(login_url='login/')
def home(request):
	return render(request, 'home.html')

def logout_view(request):
	logout(request)
	return redirect(home)