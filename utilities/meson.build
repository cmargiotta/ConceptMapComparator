word_to_singular_lib = static_library(
	'word_to_singular',
	[
		'./word_to_singular/word_to_singular.cpp'
	]
)

word_to_singular_dep = declare_dependency(
	link_with : [
		word_to_singular_lib
	],
	include_directories : ['./word_to_singular']
)

test_word_to_singular = executable('test_word_to_singular', 
    [
		'./word_to_singular/test/word_to_singular.cpp'
	], 
    dependencies : [
        catch2_dep,
		word_to_singular_dep
    ]
)

test('test_word_to_singular', test_word_to_singular)
