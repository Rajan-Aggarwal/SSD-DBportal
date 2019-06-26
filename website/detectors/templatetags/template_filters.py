from django import template

register = template.Library()

# register your custom template tags of filters here

@register.filter(name='uts_upper')
def underscore_to_space_upper(value):
	return value.upper().replace('_', ' ')