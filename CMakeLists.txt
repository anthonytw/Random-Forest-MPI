cmake_minimum_required(VERSION 2.8)

project(RandomForests)

option(BUILD_APP "Build the commandline application" ON)
option(BUILD_UT  "Build the unit tests" ON)

if (BUILD_APP)
    add_subdirectory(app)
endif()

if (BUILD_UT)
    add_subdirectory(unit_tests)
endif()
