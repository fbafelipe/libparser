cmake_minimum_required(VERSION 0.0)

PROJECT(TEST)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../build)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

FILE(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
ADD_EXECUTABLE(test ${sources})

target_link_libraries(test parser cppunit)
