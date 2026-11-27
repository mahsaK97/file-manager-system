
cmake_minimum_required(VERSION 3.20)

project(file_manager_system
    VERSION 2.0
    LANGUAGES C
)

# Use a standard C version
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Core library
add_library(file_manager_core STATIC
    src/file_operations.c
    src/folder_operations.c
    src/search_operations.c
    src/file_manager.c
    src/input_utls.c
)

# Public headers
target_include_directories(file_manager_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Command-line application
add_executable(file_manager
    src/main.c
)

target_link_libraries(file_manager
    PRIVATE
        file_manager_core
)

# Optional Qt GUI
option(FILE_MANAGER_BUILD_QT "Build Qt GUI" OFF)

if(FILE_MANAGER_BUILD_QT)
    add_subdirectory(qt-ui)
endif()

 