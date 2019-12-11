project('ConceptMapComparator', ['cpp', 'c'], version: '0.1.0',
  meson_version: '>= 0.51.0'
)

add_global_arguments('-std=c++17', language : 'cpp')
add_global_arguments('-Wall', language : 'cpp')

catch2_dep = declare_dependency(include_directories : 'subprojects/catch2/single_include/catch2')

subdir('clustering')
