cmake_minimum_required(VERSION 3.20)

project(DDSGame)

# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED)
endif()

# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(-std=c++11 -pthread)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring Game publisher/subscriber example...")
file(GLOB DDS_GAME_SOURCES_CXX "src/*.cxx")

# add_executable(DDSGamePublisher src/GamePublisher.cpp ${DDS_GAME_SOURCES_CXX})
# target_link_libraries(DDSGamePublisher fastdds fastcdr)

# add_executable(DDSGameSubscriber src/GameSubscriber.cpp ${DDS_GAME_SOURCES_CXX})
# target_link_libraries(DDSGameSubscriber fastdds fastcdr)

add_executable(DDSGameUser src/GameUser.cpp ${DDS_GAME_SOURCES_CXX})
target_link_libraries(DDSGameUser fastdds fastcdr)