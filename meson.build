project('ConceptMapComparator', 
    ['cpp', 'c'], 
    version: '0.1.0',
    meson_version: '>= 0.51.0',
    default_options : [
        'warning_level=3',
        'cpp_std=c++17'
    ]
)

cmake = import('cmake')

cpp 		= meson.get_compiler('cpp')

pthread_dep = cpp.find_library('pthread', required : true)
dl_dep 		= cpp.find_library('dl', required : true)

sqlitecpp_dep  = declare_dependency(dependencies:  [cmake.subproject('SQLiteCpp').dependency('SQLiteCpp'), 
													cmake.subproject('SQLiteCpp').dependency('sqlite3'), 
													dl_dep, 
													pthread_dep])

catch2_main_lib = static_library(
    'catch2_main',
    ['catch_main.cpp'],
    include_directories : 'subprojects/Catch2/single_include/catch2'
)
catch2_dep = declare_dependency(
    include_directories : 'subprojects/Catch2/single_include/catch2',
    link_with: [catch2_main_lib]
)

subdir('utilities')
subdir('thread_pool')
subdir('clustering')
subdir('wordnet')
subdir('concept_map')
