thread_pool_sources = files([
	'thread_pool.cpp'
])

thread_pool_lib = static_library('thread_pool', thread_pool_sources)
thread_pool_dep = declare_dependency(include_directories : '.', link_with: thread_pool_lib, dependencies: [pthread_dep])

thread_pool_test_dep = declare_dependency(include_directories : 'test', dependencies : [thread_pool_dep])

test_thread_pool_sources = files([
    'test/thread_pool.cpp'
])

test_thread_pool = executable('test_thread_pool', 
    test_thread_pool_sources, 
    dependencies : [
        catch2_dep,
        thread_pool_test_dep
    ]
)

test('test_thread_pool', test_thread_pool)
