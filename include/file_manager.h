#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <stdio.h>

typedef struct
{
    char current_path[1024];
    char *buffer;


} FileManager;



#endif // FILE_MANAGER_H
