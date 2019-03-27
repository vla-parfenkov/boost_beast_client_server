cmake_minimum_required(VERSION 3.7)
project(sample_server)

set(CMAKE_CXX_STANDARD 14)

link_directories(/usr/local/lib/)


find_package(Boost 1.68 REQUIRED)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(inc)

add_executable(sample_server src/main.cpp src/server_data.cpp src/session.cpp src/server.cpp)
target_link_libraries(sample_server -pthread -lboost_program_options)