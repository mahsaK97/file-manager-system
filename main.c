#include <stdio.h>
#include <stdlib.h>
#include "file_operations.h"
#include "directory_operation.h"
#include "search_operation.h"
#include "file_manager.h"



int main()
{

    FileManager fm;
    fm.buffer = malloc(1024 *sizeof(char));
    if(fm.buffer == NULL)
    {
         printf("Memory allocation failed.\n");
         return 1;
    }
    long option;
    do
    { printf("enter the number of option you want to do:\n");
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

        char buffer[10];
        fgets(buffer, sizeof(buffer) , stdin);
        char *endptr;
        option =strtol(buffer , &endptr , 10);
        if (buffer == endptr || ( *endptr != '\n' && *endptr != '\0'))
        {
            printf("invalid input.\n");
            continue;
        }

        switch(option)
    {
        case 1:
            create_file();
                      break;
        case 2:
            delete_file(&fm);
                      break;
        case 3:
            update_file();
                      break;
        case 4:
            read_file();
                      break;
        case 5:
            rename_file();
                      break;

        case 6:
            copy_file(&fm);
                      break;

        case 7:
            create_folder(&fm);
                      break;
        case 8:
            delete_folder(&fm);
                      break;

        case 9:
            file_list_in_folder(&fm);
                      break;

        case 10:
            move_file(&fm);
                  break;
        case 11:
            search_menu(&fm);
            break;


        case 12:
            printf("Exiting...\n");
                      break;

        default:
            printf("Invalid option!\n");


     }

} while(option != 12);

free(fm.buffer);


return 0;
}
