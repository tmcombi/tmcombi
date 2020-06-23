cmake_minimum_required(VERSION 3.10)

# find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

# set the project name
project(Tutorial)

# add the executable
add_executable(Tutorial test/boost-test.cc)
