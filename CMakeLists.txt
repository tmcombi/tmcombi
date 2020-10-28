cmake_minimum_required(VERSION 3.10)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()



# set the project name
project(TMCombi)


# add the executable
add_executable(test_feature_vector lib/feature_vector.cc)
add_executable(test_feature_names lib/feature_names.cc)
add_executable(test_sample lib/sample.cc)
add_executable(test_sample_creator lib/sample_creator.cc)
add_executable(test_border lib/border.cc)
add_executable(test_less_relation_iterator lib/less_relation_iterator.cc)
add_executable(test_graph_creator lib/graph_creator.cc)
add_executable(test_layer_partitioner lib/layer_partitioner.cc)
target_link_libraries(test_layer_partitioner /usr/local/lib/libglpk.a)
add_executable(test_layer_partitioning lib/layer_partitioning.cc)
add_executable(test_border_system lib/border_system.cc)
add_executable(test_rtree4d test/test_rtree4d.cc)
add_executable(test_rtree25d test/test_rtree25d.cc)
