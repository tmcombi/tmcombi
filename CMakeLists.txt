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
add_executable(test_io test/test_io.cc)
