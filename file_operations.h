#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H
#include "file_manager.h"

void create_file();
void read_file();
void update_file();
void delete_file(FileManager *fm);
void rename_file(FileManager *fm);
void copy_file(FileManager *fm);
void move_file(FileManager *fm);


#endif
