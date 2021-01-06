cmake_minimum_required(VERSION 3.10)

# set the project name
project(TMCombi)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")



# add the executable
add_executable(test_feature_transform_subset lib/feature_transform_subset.cc)
add_executable(test_feature_vector lib/feature_vector.cc)
add_executable(test_feature_names lib/feature_names.cc)
add_executable(test_sample lib/sample.cc)
add_executable(test_sample_creator lib/sample_creator.cc)
add_executable(test_statistics lib/statistics.cc)
add_executable(test_border lib/border.cc)
add_executable(test_graph_creator lib/graph_creator.cc)
add_executable(test_layer_partitioner lib/layer_partitioner.cc)
#target_link_libraries(test_layer_partitioner /usr/local/lib/libglpk.a)
add_executable(test_layer_partitioning lib/layer_partitioning.cc)
add_executable(test_layer_partitioning_creator lib/layer_partitioning_creator.cc)
#target_link_libraries(test_layer_partitioning_creator /usr/local/lib/libglpk.a)
add_executable(test_border_system lib/border_system.cc)
add_executable(test_classifier_tmc lib/classifier_tmc.cc)
add_executable(test_evaluator lib/evaluator.cc)
add_executable(test_classifier_creator_train_tmc lib/classifier_creator_train_tmc.cc)
add_executable(test_forward_selection lib/forward_selection.cc)
add_executable(test_classifier_transformed_features lib/classifier_transformed_features.cc)
add_executable(test_classifier_creator_dispatch_ptree lib/classifier_creator_dispatch_ptree.cc)
add_executable(test_classifier_creator_feature_selection lib/classifier_creator_feature_selection.cc)


add_executable(test_rtree4d test/test_rtree4d.cc)
add_executable(test_rtree25d test/test_rtree25d.cc)
add_executable(test_tmc_paper_dataset test/test_tmc_paper_dataset.cc)
#target_link_libraries(test_tmc_paper_dataset /usr/local/lib/libglpk.a)
add_executable(test_fs_tmc_paper_dataset test/test_fs_tmc_paper_dataset.cc)
#target_link_libraries(test_fs_tmc_paper_dataset /usr/local/lib/libglpk.a)
add_executable(test_tmc_paper_weighted_dataset test/test_tmc_paper_weighted_dataset.cc)
#target_link_libraries(test_tmc_paper_weighted_dataset /usr/local/lib/libglpk.a)
add_executable(test_graph_creator_26kNodes test/test_graph_creator_26kNodes.cc)
add_executable(test_adult_transformed_dataset test/test_adult_transformed_dataset.cc)
#target_link_libraries(test_adult_transformed_dataset /usr/local/lib/libglpk.a)
add_executable(test_fs_adult_transformed_dataset test/test_fs_adult_transformed_dataset.cc)
#target_link_libraries(test_fs_adult_transformed_dataset /usr/local/lib/libglpk.a)
add_executable(test_feature_selection_tmc_paper_dataset test/test_feature_selection_tmc_paper_dataset.cc)
add_executable(test_feature_selection_adult_transformed_dataset test/test_feature_selection_adult_transformed_dataset.cc)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(tmc main/tmc.cc)
target_link_libraries(tmc ${Boost_LIBRARIES})

add_executable(tmc_classify_example main/tmc_classify_example.cc)
target_link_libraries(tmc_classify_example ${Boost_LIBRARIES})
