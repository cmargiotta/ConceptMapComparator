wordnet_lib = static_library(
	'wordnet',
	[
		'wordnet.cpp', 
		'similarity.cpp',
		'word.cpp'
	],
	dependencies : [
		sqlitecpp_dep,
		word_to_singular_dep
    ]
)

wordnet_dep = declare_dependency(
    link_with : [wordnet_lib],
    include_directories : ['.'],
    dependencies: [
		sqlitecpp_dep,
		word_to_singular_dep
    ]
)

test_wordnet_sources = files([
    'test/similarity.cpp'
])

test_comparison = executable('test_comparison', 
    test_wordnet_sources, 
    dependencies : [
        catch2_dep,
        wordnet_dep
    ]
)

test('test_comparison', test_comparison)

