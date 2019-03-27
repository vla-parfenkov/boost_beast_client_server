cmake_minimum_required(VERSION 3.7)
project(sample_client)

set(CMAKE_CXX_STANDARD 14)

link_directories(/usr/local/lib/)


find_package(Boost 1.68 REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(inc)

add_executable(sample_client src/main.cpp src/cmd_parser.cpp src/client.cpp src/responce_parser.cpp)

target_link_libraries(sample_client -lboost_program_options -pthread)