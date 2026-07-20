# File Manager System

A simple command-line file manager written in C, providing basic file and folder operations along with search functionality.

## Features

- **File operations**: create, delete, update, read, rename, copy, and move files
- **Folder operations**: create folders, delete folders, and list files inside a folder
- **Search operations**: search for files by name or by extension

## Menu Options

```
1.  Create file
2.  Delete file
3.  Update file
4.  Read file
5.  Rename file
6.  Copy file
7.  Create folder
8.  Delete folder
9.  List of file in folder
10. Move file
11. Search file in folder
12. Search file by extension
13. Exit
```

## Project Structure

```
.
├── main.c                    # Entry point of the program (menu-driven interface)
├── file_operations.c         # Implementation of file-related functions
├── file_operations.h         # Header file for file operations
├── directory_operation.c     # Implementation of folder-related functions
├── directory_operation.h     # Header file for folder operations
├── search_operation.c        # Implementation of search functionality
├── search_operation.h        # Header file for search operations
└── README.md
```

## Requirements

- A C compiler (e.g., `gcc`)
- A Unix-like environment (Linux/macOS) or Windows with a compatible toolchain (e.g., MinGW)

## Build

Compile all source files together:

```bash
gcc main.c file_operations.c directory_operation.c search_operation.c -o file_manager
```

## Run

```bash
./file_manager
```

## Usage

After running the program, you'll see a numbered menu (shown above). Enter the number corresponding to the operation you want to perform, then follow the prompts. Enter `13` to exit the program.

## Notes

- Make sure you have the necessary read/write permissions in the directory where you run the program.
- This project was built as a learning exercise to practice file I/O and directory handling in C.

## License

This project is open source and available for personal or educational use.
