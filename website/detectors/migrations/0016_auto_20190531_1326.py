# Generated by Django 2.2.1 on 2019-05-31 13:26

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('detectors', '0015_auto_20190531_1311'),
    ]

    operations = [
        migrations.AlterField(
            model_name='detector',
            name='dead_or_alive',
            field=models.CharField(choices=[('Dead', 'Dead'), ('Alive', 'Alive')], max_length=1, verbose_name='Dead/Alive'),
        ),
    ]
