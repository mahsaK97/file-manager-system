#include <stdio.h>
#include "directory_operation.h"
#include <dirent.h>
#include <string.h>


#ifdef _WIN32
   #include <direct.h>
   #define MKDIR(name) _mkdir(name)
   #define RMDIR(name) _rmdir(name)


#else
    #include <sys/stat.h>
    #include <unistd.h>
    #define MKDIR(name) mkdir(name,0777)
    #define RMDIR(name) rmdir(name)

#endif // WIN32










void create_folder()
{

    char name_of_folder[100];
    printf("do you want to make a folder?[y/n]\n");
    char buffer[10];
    fgets(buffer , sizeof(buffer) , stdin);
    char answer = buffer[0];
    if(answer == 'y' || answer == 'Y')
    {
        printf("enter name of folder:\n");
        fgets(name_of_folder , sizeof(name_of_folder) , stdin);
        name_of_folder[strcspn(name_of_folder , "\n")] = '\0';
        if(MKDIR(name_of_folder) == 0)
        {
             printf("folder created successfully.\n");
        }

        else
        {
            perror("mkdir failed");
        }

    }


    else if (answer == 'n' || answer == 'N')
    {
        printf("folder creation cancelled.\n");
    }

    else
    {
        printf("invalid input.\n");
    }

}





void delete_folder()
{
    char folder_name[100];

    printf("do you want to delete a folder?[y/n]\n");
    char buffer[10];
    fgets(buffer , sizeof(buffer) , stdin);
    char answer = buffer[0];


    if(answer == 'N' || answer =='n')
    {
        printf("folder is not deleted.\n");
        return;
    }

    else if(answer == 'y' || answer == 'Y')
    {
        printf("enter folder name:\n");
        fgets(folder_name , sizeof(folder_name) , stdin);
        folder_name[strcspn(folder_name , "\n")] = '\0';
        if(folder_name[0] == '\0')
        {
            printf("folder name cannot be empty.\n");
            return;
        }
        else if(RMDIR(folder_name) == 0)
        {
            printf("folder deleted successfully.\n");
            return;
        }
        else
        {
            perror("failed to delete folder.");
            return;

        }
    }
     else
    {
        printf("invalid input.\n");
        return;
    }

}




void file_list_in_folder()
{
    DIR *dir;
    char folder_name[100];
    char buffer[10];
    char answer;
    struct dirent *entry;




    printf("Do you want to see a list of a folder?[y/n]\n");
    fgets(buffer , sizeof(buffer) , stdin);
    answer = buffer[0];

    if(answer == 'N' || answer == 'n')
    {
        printf("okay. back to menu...\n");
        return;
    }

    else if(answer == 'y' || answer =='Y')
    {
        printf("enter folder name: ");
        fgets(folder_name , sizeof(folder_name) , stdin);
        folder_name[strcspn(folder_name,"\n")] = '\0';

        dir = opendir(folder_name);

        if(dir == NULL)
        {
            perror("cannot open folder");
            return;
        }

        while((entry = readdir(dir)) != NULL)
        {

            printf("%s\n" ,entry->d_name);
        }

        closedir(dir);
    }

    else
    {
        printf("invalid input.\n");
        return;
    }
}












