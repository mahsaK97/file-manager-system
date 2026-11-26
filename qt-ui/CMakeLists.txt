cmake_minimum_required(VERSION 3.16)
project(file_manager_qt_ui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(file_manager_gui
    main.cpp
)

target_link_libraries(file_manager_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
