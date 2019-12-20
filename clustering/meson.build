clustering_dep = declare_dependency(include_directories : '.', dependencies: [thread_pool_dep])
clustering_test_dep = declare_dependency(include_directories : 'test', dependencies : [clustering_dep])

test_clustering_sources = files([
    'test/clustering.cpp'
])

test_clustering = executable('test_clustering', 
    test_clustering_sources, 
    dependencies : [
        catch2_dep,
        clustering_test_dep
    ]
)

test('test_clustering', test_clustering)
