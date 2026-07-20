#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


void search_by_extension()
{
    char buffer[10];
    char answer;
    char extension[100];
    char folder_name[100];
    DIR *dir;
    struct dirent *entry;

    printf("do you want to search by extension?[y/n]\n");
    fgets(buffer, sizeof(buffer), stdin);
    answer = buffer[0];
    if(answer =='n' || answer == 'N')
    {
        printf("okay. back to menu...");
        return;
    }
    else if (answer == '\n' || answer =='\0')
    {
        printf("input can't be empty.\n");
        return;
    }
    else if (answer == 'y' || answer =='Y')
    {
        printf("enter extension (.example):");
        fgets(extension, sizeof(extension) , stdin);
        extension[strcspn(extension, "\n")] ='\0';

        printf("enter folder name:");
        fgets(folder_name , sizeof(folder_name) , stdin);
        folder_name[strcspn(folder_name , "\n")] = '\0';

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
        perror("error");
    }

}



void search_by_name()
{
    char file_name[100];
    char folder_name[100];
    int found =0;
    char buffer[10];
    char answer;
    DIR  *dir;
    struct  dirent *entry;

    printf("do you want to search a file in a folder?[y/n]");
    fgets(buffer,sizeof(buffer) , stdin);
    answer = buffer[0];


    if(answer == 'n' || answer == 'N')
       {
           printf("back to menu...");
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
        }
    }
    else
    {
        printf("invalid input.\n");
    }
    if(found == 1)
    {
        printf("file found.\n");
    }
    else if (found == 0)
    {
        printf("file not found.\n");
    }

}










void search_menu()
{
    long option;
    char buffer[10];
    char *endptr;

    printf("search menu:\n");
    printf("1.search by name\n");
    printf("2.search by extension\n");

    fgets(buffer, sizeof(buffer) , stdin);
    option = strtol(buffer , &endptr , 10);

    if(endptr == buffer || (*endptr != '\0' && *endptr != '\n'))
    {
        printf("invalid input.\n");
        return;
    }

    switch(option)
    {
    case 1:
        search_by_name();
        break;


    case 2:
        search_by_extension();
        break;
    }



}


