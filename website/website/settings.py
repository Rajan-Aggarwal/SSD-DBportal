"""
Django settings for website project.

Generated by 'django-admin startproject' using Django 2.2.1.

For more information on this file, see
https://docs.djangoproject.com/en/2.2/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/2.2/ref/settings/
"""

import os

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/2.2/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = 'o1tuvr^@o^h9y&9x7!sb179o3_r2!7qr4rl#qva%!dj)#lt&vz'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

# Add URL and IP addresses of your server here
ALLOWED_HOSTS = ['ssddb.cern.ch', 'localhost', ]

# Add static folder to STATIC_DIRS

#STATICFILES_DIRS = [
#    os.path.join(PROJECT_ROOT, 'static'),
#]

# Application definition

INSTALLED_APPS = [
    'detectors',
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'django_tables2',
    'django_filters',
    'bootstrapform',
    'bootstrap3',
    'crispy_forms',
    'admin_reorder',
    'import_export',
    'django.contrib.admindocs',
]

MIDDLEWARE = [
    'django.middleware.security.SecurityMiddleware',
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
    'admin_reorder.middleware.ModelAdminReorder',
]

ROOT_URLCONF = 'website.urls'

TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': ['templates'],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]

WSGI_APPLICATION = 'website.wsgi.application'


# Database
# https://docs.djangoproject.com/en/2.2/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.postgresql_psycopg2',
        'NAME': 'ssddb',
        'USER': 'ssd',
        'PASSWORD': 'rd50.db',
        'HOST': 'localhost',
        'PORT': '',
    }
}


# Password validation
# https://docs.djangoproject.com/en/2.2/ref/settings/#auth-password-validators

AUTH_PASSWORD_VALIDATORS = [
    {
        'NAME': 'django.contrib.auth.password_validation.UserAttributeSimilarityValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.MinimumLengthValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.CommonPasswordValidator',
    },
    {
        'NAME': 'django.contrib.auth.password_validation.NumericPasswordValidator',
    },
]

ADMIN_REORDER = [
    
    # keep original label and models
    'sites',

    # reorder app models
    {'app': 'auth', 'models': ('auth.User', 'auth.Group')},
    {'app': 'detectors', 'models': ('detectors.Detector', 
                                    'detectors.LocationTransfer',
                                    'detectors.Annealing',
                                    'detectors.Irradiation',    
                                    )}
]


# Internationalization
# https://docs.djangoproject.com/en/2.2/topics/i18n/

LANGUAGE_CODE = 'en-us'

TIME_ZONE = 'UTC'

USE_I18N = False

USE_L10N = False

USE_TZ = True


# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/2.2/howto/static-files/

STATIC_URL  = '/static/'
STATIC_ROOT = os.path.join(BASE_DIR, 'static/') 

# Redirect to the login URL to access any file 

LOGIN_URL = '/login/'

# Redirect to home URL after login (Default redirects to /accounts/profile/)

LOGIN_REDIRECT_URL = '/'

# date formats

DATE_FORMAT             = 'd/m/Y'
SHORT_DATE_FORMAT       = 'd/m/Y'
DATE_INPUT_FORMATS      = ['%d/%m/%Y',]
