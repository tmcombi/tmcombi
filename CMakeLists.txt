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
add_executable(unit_tests lib/unit_tests.cc)
#target_link_libraries(test_layer_partitioner /usr/local/lib/libglpk.a)

add_executable(test_rtree4d test/test_rtree4d.cc)
add_executable(test_rtree25d test/test_rtree25d.cc)
add_executable(test_tmc_paper_dataset test/test_tmc_paper_dataset.cc)
#target_link_libraries(test_tmc_paper_dataset /usr/local/lib/libglpk.a)
add_executable(test_tmc_paper_weighted_dataset test/test_tmc_paper_weighted_dataset.cc)
#target_link_libraries(test_tmc_paper_weighted_dataset /usr/local/lib/libglpk.a)
add_executable(test_graph_creator_26kNodes test/test_graph_creator_26kNodes.cc)
add_executable(test_adult_transformed_dataset test/test_adult_transformed_dataset.cc)
#target_link_libraries(test_adult_transformed_dataset /usr/local/lib/libglpk.a)
add_executable(test_fs_n_fold_tmc_paper_dataset test/test_fs_n_fold_tmc_paper_dataset.cc)
add_executable(test_fs_graph_tmc_paper_dataset test/test_fs_graph_tmc_paper_dataset.cc)
add_executable(test_fs_n_fold_adult_transformed_dataset test/test_fs_n_fold_adult_transformed_dataset.cc)
add_executable(test_fs_graph_adult_transformed_dataset test/test_fs_graph_adult_transformed_dataset.cc)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(tmc main/tmc.cc)
target_link_libraries(tmc ${Boost_LIBRARIES})

add_executable(tmc_classify_example main/tmc_classify_example.cc)
target_link_libraries(tmc_classify_example ${Boost_LIBRARIES})
