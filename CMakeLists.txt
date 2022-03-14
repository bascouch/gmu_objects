cmake_minimum_required(VERSION 3.19)

string(REGEX REPLACE "(.*)/" "" THIS_FOLDER_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
project(${THIS_FOLDER_NAME})

set (C74_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/externals)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bufGranul~)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/liveGranul~)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/synGranul~)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rand_dist_list)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rand_dist_list~)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/idleubutton)
