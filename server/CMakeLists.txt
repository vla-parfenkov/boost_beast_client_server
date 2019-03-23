cmake_minimum_required(VERSION 3.7)
project(sample_server)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.68 REQUIRED system)

include_directories(inc)

add_executable(sample_server src/main.cpp src/server_data.cpp src/request_handler.cpp src/session.cpp src/threadpool.cpp src/server.cpp)
target_link_libraries(sample_server -lpthread ${Boost_LIBRARIES})