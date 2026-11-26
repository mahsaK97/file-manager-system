cmake_minimum_requieed(VERSION 3.20)
project(file_manager_system VERSION 2.0 LANGUEAGES C)


add_library(file_manager_system_core 
src/file_operations.c
src/folder_operations.c
src/search_operation.c
src/file_manager.c)

 