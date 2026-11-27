
# File Manager System

A command-line file manager written in C, with an optional Qt Widgets graphical interface.

The project provides file and folder operations, file searching, input validation, and error handling.

It was developed as a learning project to practice file I/O, directory handling, modular programming, defensive programming, and CMake-based project management.

## Features

### File Operations

- Create files
- Read files
- Update files
- Delete files
- Rename files
- Copy files
- Move files

### Folder Operations

- Create folders
- Delete empty folders
- List folder contents

### Search Operations

- Search by file name
- Search by extension
- Search by file size
- Recursive file search

### Error Handling

- Input validation
- File and directory error handling
- Resource cleanup
- Path length validation
- Safe file I/O helpers

## Technologies

- C
- CMake
- Qt Widgets (optional GUI)

## Project Structure

```text
.
├── CMakeLists.txt
├── README.md
├── include/
│   ├── file_manager.h
│   ├── file_operations.h
│   ├── folder_operations.h
│   ├── search_operations.h
│   └── input_utls.h
│
├── src/
│   ├── main.c
│   ├── file_manager.c
│   ├── file_operations.c
│   ├── folder_operations.c
│   ├── search_operations.c
│   └── input_utls.c
│
└── qt-ui/
    ├── CMakeLists.txt
    ├── main.cpp
    └── README.md
```

## Requirements

- CMake 3.20 or newer
- A C compiler such as GCC, Clang, or MinGW
- Qt 5 or Qt 6 for the optional graphical interface

## Build

### Command-Line Application

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

The executable will be generated inside the build directory.

On Windows with MinGW, it will usually be:

```text
build/file_manager.exe
```

On Linux:

```text
build/file_manager
```

## Run

On Windows:

```bash
build/file_manager.exe
```

On Linux:

```bash
./build/file_manager
```

## Build the Qt GUI

The Qt interface is optional.

To enable it:

```bash
cmake -S . -B build -DFILE_MANAGER_BUILD_QT=ON
cmake --build build
```

Qt development packages and a compatible compiler must be installed.

## Notes

- Run the application in a directory where you have the required permissions.
- Relative paths are resolved from the application's current working directory.
- The GUI and command-line application are separate executable targets.

## Future Improvements

- Add automated tests
- Improve error reporting and operation return values
- Add activity logging
- Improve GUI integration with the core library
- Add cross-platform build verification

## License

This project is intended for personal and educational use.
