cmake_minimum_required(VERSION 3.7)
project(sample_server)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.70 REQUIRED )
SET(CMAKE_CXX_FLAGS -pthread)

include_directories(inc)

add_executable(sample_server src/main.cpp src/server.cpp src/threadpool.cpp src/session.cpp src/request_handler.cpp)
target_link_libraries(sample_server ${Boost_LIBRARIES})