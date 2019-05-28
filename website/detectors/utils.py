from django_tables2 import SingleTableView
from django_tables2 import RequestConfig

############################
###NOT UTILIZED AS OF YET###
############################

class PagedFilteredTableView(SingleTableView):
	'''
		In-case default pagination given by django_tables2.SingleTableMixin 
		(inherited in the generic-view classes) doesn't work
	'''
	filter_class 		= None
	formhelper_class 	= None
	context_filter_name = 'filter'

	def get_queryset(self, **kwargs):
		'''
			override the get_queryset to allow pagination
		'''
		qs 							= super(PagedFilteredTableView, self).get_queryset()
		self.filter 				= self.filter_class(self.request.GET, queryset=qs)
		self.filter.form.helper 	= self.formhelper_class()
		return self.filter.qs

	def get_context_data(self, **kwargs):
		'''
			override the get_context_data to allow pagination
		'''
		context = super(PagedFilteredTableView, self).get_context_data()
		context[self.context_filter_name] = self.filter
		return context