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

test_concept_map = executable('test_concept_map', 
    [
		'test/concept_map.cpp'
    ], 
    dependencies : [
		catch2_dep,
        concept_map_dep
    ]
)

test('test_concept_map', test_concept_map)
