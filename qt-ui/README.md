# File Manager Qt UI

A standalone Qt Widgets interface for the existing C File Manager project.

## Important architecture note

This UI is deliberately kept separate from the existing `src/` and `include/` files. It uses Qt's filesystem APIs (`QFile`, `QDir`, `QFileSystemModel`) for the graphical operations. The existing C command-line implementation is not modified or called by this GUI.

This separation avoids calling the current `stdin`/`stdout`-driven C functions from the Qt event loop. Those functions are interactive console routines, not GUI-callable APIs.

## Requirements

- Qt 5.10+ or Qt 6
- CMake 3.16+
- A C++17 compiler

## Build

Run from this directory:

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run the generated `file_manager_gui` executable.

## Features

- Choose a working directory
- Browse files and folders
- Filter visible entries by name
- Create files and folders
- Rename, copy, and delete files
- Delete empty folders
- Open and edit text files
- Show basic file/folder counts

## Add to the existing repository

Copy this folder into the root of `file-manager-system` as `qt-ui/`.
Do not replace the existing C `CMakeLists.txt` with this one; this folder has its own CMake project.
