# Generated by Django 2.2.1 on 2019-05-25 08:46

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('detectors', '0006_auto_20190525_0823'),
    ]

    operations = [
        migrations.AddField(
            model_name='locationtransfer',
            name='source_location',
            field=models.CharField(default='Something', editable=False, max_length=256, verbose_name='From'),
        ),
    ]
