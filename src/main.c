#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../include/file_operations.h"
#include "../include/folder_operations.h"
#include "../include/search_operations.h"
#include "../include/file_manager.h"
#include "../include/input_utls.h"

#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
#endif

#define BUFFER_SIZE  1024
#define PATH_SIZE    2048
#define OPTION_SIZE  32

typedef enum
{
    OPT_CREATE_FILE = 1,
    OPT_DELETE_FILE,
    OPT_UPDATE_FILE,
    OPT_READ_FILE,
    OPT_RENAME_FILE,
    OPT_COPY_FILE,
    OPT_CREATE_FOLDER,
    OPT_DELETE_FOLDER,
    OPT_LIST_FOLDER,
    OPT_MOVE_FILE,
    OPT_SEARCH_MENU,
    OPT_EXIT
} MenuOption;


static void print_menu(void)
{
    printf("\nenter the number of option you want to do:\n");
    printf("1.Create file\n");
    printf("2.Delete file\n");
    printf("3.Update file\n");
    printf("4.Read file\n");
    printf("5.Rename file\n");
    printf("6.Copy file\n");
    printf("7.Create folder\n");
    printf("8.Delete folder\n");
    printf("9.List of file in folder\n");
    printf("10.Move file\n");
    printf("11.search menu\n");
    printf("12.Exit\n");
}

/* returns  1 = *option holds a valid number,
            0 = invalid input (message already printed, ask again),
           -1 = end of input or read error (stop the program) */
static int read_option(long *option)
{
    char text[OPTION_SIZE];
    char *endptr;

    if(fgets(text, sizeof(text), stdin) == NULL)
    {
        if(!feof(stdin))
        {
            perror("fgets");
        }
        return -1;
    }

    /* a line longer than the buffer must be flushed, or its tail becomes the next answer */
    if(strchr(text, '\n') == NULL && !feof(stdin))
    {
        clear_input_buffer();
        printf("invalid input.\n");
        return 0;
    }

    text[strcspn(text, "\n")] = '\0';

    errno = 0;
    long value = strtol(text, &endptr, 10);

    if(errno == ERANGE)
    {
        printf("NUMBER IS OUT OF RANGE.\n");
        return 0;
    }

    if(endptr == text || *endptr != '\0')
    {
        printf("invalid input.\n");
        return 0;
    }

    *option = value;
    return 1;
}


int main(void)
{
    FileManager fm;
    int status = 0;
    int running = 1;

    fm.buffer = malloc(BUFFER_SIZE * sizeof(char));
    if(fm.buffer == NULL)
    {
        perror("malloc buffer");
        return 1;
    }

    fm.current_path = malloc(PATH_SIZE * sizeof(char));
    if(fm.current_path == NULL)
    {
        perror("malloc current_path");
        free(fm.buffer);
        return 1;
    }

    if(getcwd(fm.current_path, PATH_SIZE) == NULL)
    {
        perror("getcwd");
        free(fm.buffer);
        free(fm.current_path);
        return 1;
    }

    /* "option" is only ever set from a valid read, so invalid input can never end the loop by accident */
    while(running)
    {
        long option = 0;

        print_menu();

        int result = read_option(&option);
        if(result < 0)
        {
            if(ferror(stdin))
            {
                status = 1;
            }
            break;
        }

        if(result == 0)
        {
            continue;
        }

        switch(option)
        {
            case OPT_CREATE_FILE:
                create_file(&fm);
                break;

            case OPT_DELETE_FILE:
                delete_file(&fm);
                break;

            case OPT_UPDATE_FILE:
                update_file(&fm);
                break;

            case OPT_READ_FILE:
                read_file(&fm);
                break;

            case OPT_RENAME_FILE:
                rename_file(&fm);
                break;

            case OPT_COPY_FILE:
                copy_file(&fm);
                break;

            case OPT_CREATE_FOLDER:
                create_folder(&fm);
                break;

            case OPT_DELETE_FOLDER:
                delete_folder(&fm);
                break;

            case OPT_LIST_FOLDER:
                file_list_in_folder(&fm);
                break;

            case OPT_MOVE_FILE:
                move_file(&fm);
                break;

            case OPT_SEARCH_MENU:
                search_menu(&fm);
                break;

            case OPT_EXIT:
                printf("Exiting...\n");
                running = 0;
                break;

            default:
                printf("Invalid option!\n");
        }
    }

    free(fm.buffer);
    free(fm.current_path);

    return status;
}
