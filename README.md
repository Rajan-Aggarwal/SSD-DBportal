# SSD-DBportal

SSD-DBportal is a web portal to access the inventory of various detectors used in SSD operations.

The portal is designed to operate under two distinct privileges:
1. admin or superuser
2. normal user

A normal user is able to retrieve tables enlisting all relevant details about a detector, and is also able to view its location transfer history, annealing history and its irradiation history.

An admin is able to do CRUD operations on all information, i.e, he can register and edit detectors and its relevant characterisitics. An admin is also able to export the data to various formats like csv, json etc. 

It runs officially under Django 2.2.1 and Python 3

# Third party applications used

1. django-tables2
2. django-filters
3. import-export-django
4. bootstrap3
5. crispy-forms
6. admin-reorder