cmake_minimum_required(VERSION 3.3)
project(for-fun)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O2 -pthread")
find_package(Boost REQUIRED COMPONENTS system filesystem serialization)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("/opt/dlib-18.18")

add_executable(pretty_print pretty_print.cpp)
add_executable(sort_benchmark sort_benchmark.cpp)

#target_link_libraries(pretty_print ${Boost_LIBRARIES} zmq)

add_executable(functional functional.cpp)
target_link_libraries(functional jemalloc)

add_executable(k-way-merge-generator k-way-merge/generator.cpp k-way-merge/message.cpp)
target_link_libraries(k-way-merge-generator ${Boost_LIBRARIES} zmq uuid)
add_executable(k-way-merge-merger k-way-merge/merger.cpp k-way-merge/message.cpp)
target_link_libraries(k-way-merge-merger ${Boost_LIBRARIES} zmq)