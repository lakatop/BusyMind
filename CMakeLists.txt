cmake_minimum_required(VERSION 3.10)
project(BusyMind)

set(CMAKE_CXX_STANDARD 20)

file(GLOB TEST_FILES "*/*.cpp")

foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    get_filename_component(test_dir ${test_file} DIRECTORY)
    set(executable_name "test_${test_name}")  # Create a unique name

    add_executable(${executable_name} ${test_file})
endforeach()