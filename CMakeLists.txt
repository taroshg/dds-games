cmake_minimum_required(VERSION 3.20)

project(DDSHelloWorld)

# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED)
endif()

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include wxWidgets
include(${wxWidgets_USE_FILE})

# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(-std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring HelloWorld publisher/subscriber example...")
file(GLOB DDS_HELLOWORLD_SOURCES_CXX "src/*.cxx")

#add_executable(DDSHelloWorldPublisher src/HelloWorldPublisher.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
#target_link_libraries(DDSHelloWorldPublisher fastdds fastcdr)

#add_executable(DDSHelloWorldSubscriber src/HelloWorldSubscriber.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
#target_link_libraries(DDSHelloWorldSubscriber fastdds fastcdr)

add_executable(TicTacToe src/TicTacToe.cpp src/Player.cpp src/Grid.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
add_executable(TicTacToeGui src/TicTacToeGui.cpp src/Player.cpp src/Grid.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
target_link_libraries(TicTacToeGui PRIVATE fastdds fastcdr ${wxWidgets_LIBRARIES})
target_link_libraries(TicTacToe PRIVATE fastdds fastcdr ${wxWidgets_LIBRARIES})
