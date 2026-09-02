#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "search_operation.h"
#include <sys/stat.h>

void search_by_extension(FileManager *fm)
{

    char answer;
    char extension[100];
    char folder_name[100];
    DIR *dir;
    struct dirent *entry;

    printf("do you want to search by extension?[y/n]\n");
    fgets(fm->buffer, 1024, stdin);
    answer =fm->buffer[0];
    if(answer =='n' || answer == 'N')
    {
        printf("okay. back to menu...\n");
        return;
    }
    else if (answer =='\0')
    {
        printf("input can't be empty.\n");
        return;
    }
    else if (answer == 'y' || answer =='Y')
    {
        printf("enter extension (.example):");
        fgets(extension, sizeof(extension) , stdin);
        extension[strcspn(extension, "\n")] ='\0';
        if(extension[0] == '\0')
        {
            printf("EXTENSION CAN'T BE EMPTY.\n");
            return;
        }

        printf("enter folder name:");
        fgets(folder_name , sizeof(folder_name) , stdin);
        folder_name[strcspn(folder_name , "\n")] = '\0';
        if(folder_name[0]== '\0')
        {
            printf("FOLDER NAME CAN'T BE EMPTY.\n");
            return;
        }

        dir = opendir(folder_name);

        if(dir == NULL)
        {
            printf("folder not found.\n");
            return;
        }
        while((entry = readdir(dir)) != NULL)
        {

            char *dot = strrchr(entry->d_name, '.');
            if(dot != NULL)
            {
                if(strcmp(dot , extension) == 0)
                  {
                      printf("%s\n" , entry->d_name);
                  }
            }

        }

        closedir(dir);
    }

    else
    {
        printf("INVALID INPUT.\n");
        return;
    }

}



void search_by_name(FileManager *fm)
{
    char file_name[100];
    char folder_name[100];
    int found =0;
    char answer;
    DIR  *dir;
    struct  dirent *entry;

    printf("do you want to search a file in a folder?[y/n]");
    fgets(fm->buffer, 1024 , stdin);
    answer =fm->buffer[0];


    if(answer == 'n' || answer == 'N')
       {
           printf("back to menu...\n");
            return;
       }

    else if (answer == 'y' || answer == 'Y')
    {
        printf("enter folder name: ");
        fgets(folder_name, sizeof(folder_name) ,stdin);
        folder_name[strcspn(folder_name, "\n")] = '\0';
        if(folder_name[0] == '\0')
        {
            printf("folder name can't be empty.\n");
            return;

        }
        else
        {
            dir=opendir(folder_name);
            if(dir == NULL)
            {
                printf("folder can't be found.\n");
                return;

            }

            printf("enter file name: ");
            fgets(file_name, sizeof(file_name) , stdin);
            file_name[strcspn(file_name, "\n")]= '\0';
            if(file_name[0]== '\0')
            {
                printf("file name can't be empty.\n");
                closedir(dir);
                return;
            }
                while((entry = readdir(dir)) != NULL)
                {
                    if((strcmp(file_name , entry->d_name)== 0))
                    {
                        found = 1;
                         break;
                    }
                }

                closedir(dir);
                if(found == 1)
                        {
                           printf("file found.\n");
                        }
                else if (found == 0)
                        {
                            printf("file not found.\n");
                        }
        }
    }
    else
    {
        printf("invalid input.\n");
    }


}



void search_by_size(FileManager *fm)
{
    char answer;
    char size_in_str[50];
    char *endptr;
    DIR *dir;
    struct stat file_info;
    struct dirent *entry;
    char folder_name[250];
    int found =0;



    printf("DO YOU WANT TO SEARCH BY SIZE?[y/n]\n");
    fgets(fm->buffer , 1024 , stdin);
    answer=fm->buffer[0];
    if(answer =='n' || answer=='N')
    {
        printf("okay.back to menu...\n");
        return;
    }

    if(answer=='y' || answer=='Y')
    {


        printf("FOLDER NAME: ");
        fgets(folder_name, sizeof(folder_name), stdin);
        folder_name[strcspn(folder_name,"\n")] ='\0';
        if(folder_name[0] == '\0')
        {
            printf("FOLDER NAME CAN'T BE EMPTY.\n");
            return;
        }
        dir=opendir(folder_name);
        if(dir == NULL)
        {
            printf("FOLDER NOT FOUND.\n");
            return;
        }


        printf("ENTER SIZE: ");
        fgets(size_in_str, sizeof(size_in_str), stdin);
        long long_size=strtol(size_in_str,&endptr, 10);
        if(size_in_str == endptr||(*endptr != '\n' && *endptr !='\0'))
        {
            printf("INVALID SIZE.\n");
            return;
        }

        while((entry = readdir(dir))!= NULL)
        {

            if(stat(entry->d_name , &file_info)==0)
               {
                    if(file_info.st_size == long_size)
                    {
                        printf("file name : %s\n file size: %ld\n",entry->d_name,file_info.st_size);
                        found=1;
                    }
               }
        }

        closedir(dir);
    }

    if(found == 0)
    {
        printf("no file with this size was found.\n");
    }

    else
    {
        printf("INVALID INPUT.\n");
    }

}


void search_recursive(const char *current_path , const char *file_name)
{

    DIR *dir;
    struct  dirent *entry;

    dir = opendir(current_path);
    if(dir == NULL)
    {
        printf("FOLDER NOT FOUND.\n");
        return;
    }

    while((entry = readdir(dir)) != NULL)

    {
        if(strcmp(entry->d_name,".")== 0 ||
           strcmp(entry->d_name , "..") == 0)
        {
            continue;
        }

        if(strcmp(entry->d_name,file_name)==0)
        {
            printf("file found : %s/%s\n",current_path, entry->d_name);
        }

        if(entry->d_type == DT_DIR)
        {
            char new_path[1024];

            sprintf(
                    new_path,
                    sizeof(new_path),
                    "%s/%s",
                    current_path,
                    entry->d_name
                    );


            search_recursive(new_path,file_name);
        }

    }

     closedir(dir);
}

void search(FileManager *fm)
{

    char file_name[256];

    printf("DO YOU WANT SEARCH WITH RECURSIVE?\n");
    fgets(fm->buffer, 1024, stdin);
    if(fm->buffer[0] == 'n' || fm->buffer[0] == 'N')
    {
        printf("OKAY.BACK TO MENU....\n");
        return;
    }

    else if(fm->buffer[0] == 'Y' || fm->buffer[0] == 'y')
    {
        printf("FILE NAME: ");
        fgets(file_name, sizeof(file_name) , stdin);
        if(file_name[0] == '\0')
        {
            printf("FILE NAME CAN'T BE EMPTY.\n");
            return;
        }

        file_name[strcspn(file_name, "\n")] = '\0';

        search_recursive(fm->current_path, file_name);

    }
    else
    {
        printf("INVALID INPUT.\n");
        return;
    }
}


void search_menu(FileManager *fm)
{
    long option;
    char *endptr;

    printf("search menu:\n");
    printf("1.search by name\n");
    printf("2.search by extension\n");
    printf("3.search by size\n");
    printf("4.search with recursive\n");

    fgets(fm ->buffer, 1024 , stdin);
    option = strtol(fm ->buffer , &endptr , 10);

    if(endptr == fm->buffer || (*endptr != '\0' && *endptr != '\n'))
    {
        printf("invalid input.\n");
        return;
    }

    switch(option)
    {
    case 1:
        search_by_name(fm);
        break;


    case 2:
        search_by_extension(fm);
        break;


    case 3:
        search_by_size(fm);
        break;


    case 4:
        search(fm);
        break;


    default:
        printf("INVALID INPUT.\n");
        return;
    }
}
