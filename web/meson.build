executable(	'web_server', 
			'web_server.cpp', 
			dependencies : [
				cpp_httplib_dep,
				concept_map_dep 
			])

configure_file(input : 'resources/index.html',
  output : 'index.html',
  configuration : configuration_data())
