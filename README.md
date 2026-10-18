# File Manager System

A command-line file manager written in C. It provides everyday file and folder operations, several kinds of search (including recursive search), input validation, and system-level error reporting through `errno` and `perror`.

The project was built as a learning exercise to practice file I/O, directory handling, dynamic memory, and defensive programming in C.

## Features

**File operations**
- Create, read, update, delete, rename, copy, and move files
- Update supports two modes: append and overwrite
- Destructive actions (delete, move) ask for confirmation first
- Copy and move refuse to overwrite an existing destination file

**Folder operations**
- Create a folder
- Delete a folder (the folder must be empty)
- List the entries of a folder

**Search operations**
- Search for a file by name inside a folder
- Search for files by extension (for example `.txt`)
- Search for files by exact size
- Recursive search by file name, starting from the current working directory

**Robustness**
- Every prompt validates its input: empty names, invalid choices, and out-of-range numbers are rejected
- Long input lines are handled safely, so leftover characters do not leak into the next prompt
- Failures of `fopen`, `opendir`, `mkdir`, `rmdir`, `remove`, `rename`, and `malloc` are reported with `perror`, which shows the real cause (for example "Permission denied")
- Resources such as file handles, directory handles, and heap buffers are released on error paths
- Path building uses `snprintf` with length checks to avoid buffer overflows
- Platform macros (`MKDIR`, `PATH_SEPARATOR`) keep the code portable between Linux/macOS and Windows

## Menu

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
11. Search menu
12. Exit
```

The search menu offers:

```
1. Search by name
2. Search by extension
3. Search by size
4. Search with recursive
```

## Project Structure

```
.
├── include/
│   ├── file_manager.h          # FileManager struct (shared state)
│   ├── file_operations.h       # File operation declarations
│   ├── folder_operations.h     # Folder operation declarations
│   ├── search_operations.h     # Search declarations
│   └── input_utls.h            # Input helper declarations
├── src/
│   ├── main.c                  # Entry point and menu loop
│   ├── file_operations.c       # Create, read, update, delete, rename, copy, move
│   ├── folder_operations.c     # Create, delete, and list folders
│   ├── search_operations.c     # Search by name, extension, size, and recursive search
│   ├── input_utls.c            # Input buffer helper
│   └── file_manager.c          # Reserved for shared helpers
├── logs/                       # Reserved for a future activity log
└── README.md
```

## Requirements

- A C compiler such as `gcc` or `clang`
- Linux or macOS, or Windows with a compatible toolchain such as MinGW

## Build

From the project root, compile all source files together:

```bash
gcc src/*.c -Iinclude -Wall -Wextra -o file_manager
```

## Run

```bash
./file_manager
```

Enter the number of the operation you want, then follow the prompts. Enter `12` to exit.

## Example

```
enter the number of option you want to do:
1.Create file
...
12.Exit
11
search menu:
1.search by name
2.search by extension
3.search by size
4.search with recursive
```

## Notes

- Run the program in a directory where you have the read and write permissions you need.
- File and folder names are entered relative to the directory where the program runs, unless you type a full path.

## Roadmap

- Write errors and actions to a log file inside `logs/`
- Return status codes from the operations instead of `void`, so callers can react to failures
- Add a Makefile or CMake build
- Add automated tests

## License

This project is open source and available for personal and educational use.