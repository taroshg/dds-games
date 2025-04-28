cmake_minimum_required(VERSION 3.20)

project(DDSGame)


if(WIN32)
    # =======================
    # Find OpenSSL for FastDDS on windows
    # =======================
    set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-3-1-8-Win64") #Point to the install path for OpenSSL optionally use version 3.1.4 since that's what fastDDS likes
    set(OPENSSL_USE_STATIC_LIBS TRUE)  # Optional: helps avoid linking errors on Windows
    find_package(OpenSSL REQUIRED)
endif()

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

if(WIN32)
    set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.2.7") #Point to your install path for wxWidgets
    set(wxWidgets_LIB_DIR "C:/wxWidgets-3.2.7/build/lib/vc_x64_lib") #build wxWidgets using Cmake using the same wxWidgets installs build folder point to the "lib/vc_x64_lib" path that should be in build therefater
    set(wxWidgets_CONFIGURATION mswu)  # Unicode build
endif()

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




if(WIN32)
# =======================
# Build executable Window
# =======================
add_executable(DDSGameSuite WIN32
    src/GameSuite.cpp
    ${DDS_GAME_SOURCES_CXX}
)
else()
# =======================
# Build executable Mac OS and Linux
# =======================
add_executable(DDSGameSuite
    src/GameSuite.cpp
    ${DDS_GAME_SOURCES_CXX}
)
endif()

target_link_libraries(DDSGameSuite
    fastdds
    fastcdr
    ${wxWidgets_LIBRARIES}
)
