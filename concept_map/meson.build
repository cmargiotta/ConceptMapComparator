concept_map_lib = static_library(
	'concept_map',
	[
		'concept_map.cpp'
	],
	dependencies: [
		wordnet_dep,
		clustering_dep
	]
)

concept_map_dep = declare_dependency(
    link_with: [concept_map_lib],
    include_directories: ['.'],
    dependencies: [
		wordnet_dep,
		clustering_dep
	]
)

test = executable('test_cm', 
    ['main.cpp'], 
    dependencies : [
        concept_map_dep
    ]
)

