project('ConceptMapComparator', 
    ['cpp', 'c'], 
    version: '0.1.0',
    meson_version: '>= 0.51.0',
    default_options : [
        'warning_level=3',
        'cpp_std=c++17'
    ]
)

cpp = meson.get_compiler('cpp')
pthread_dep = cpp.find_library('pthread', required : true)

catch2_dep = declare_dependency(include_directories : 'subprojects/catch2/single_include/catch2')

subdir('clustering')
subdir('thread_pool')
