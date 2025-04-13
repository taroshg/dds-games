cmake_minimum_required(VERSION 3.20)

project(DDSGame)

# =======================
# Find FastDDS & FastCDR
# =======================
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED)
endif()

# =======================
# Find wxWidgets
# =======================
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Optional: manually include wxWidgets include directories
# This helps with "wx/setup.h" not found issues
include_directories(${wxWidgets_INCLUDE_DIRS})

# =======================
# C++11 support
# =======================
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

# =======================
# Source files
# =======================
message(STATUS "Configuring Game publisher/subscriber example...")
file(GLOB DDS_GAME_SOURCES_CXX "src/*.cxx")


# =======================
# Build executable
# =======================
add_executable(DDSGameSuite
    # src/GameUser.cpp
    src/GameSuite.cpp
    src/Grid.cpp
    src/Player.cpp
    src/AbstractGameGUI.cpp
    src/TTTGameGUI.cpp
    ${DDS_GAME_SOURCES_CXX}
)

target_link_libraries(DDSGameSuite
    fastdds
    fastcdr
    ${wxWidgets_LIBRARIES}
)
