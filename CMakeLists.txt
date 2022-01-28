cmake_minimum_required(VERSION 3.16)

project(QuickDict VERSION 0.2 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(QuickDict)
add_subdirectory(third_party)
