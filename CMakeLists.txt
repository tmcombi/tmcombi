cmake_minimum_required(VERSION 3.10)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# set the project name
project(SampleTests)
project(IntegrationTests)


# add the executable
add_executable(SampleTests lib/sample.cc)
add_executable(IntegrationTests test/boost-test.cc)
