#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_operations.h"
#include <dirent.h>

#ifdef _WIN32

     #define PATH_SEPARATOR "\\"

#else
     #define PATH_SEPARATOR  "/"

#endif


void create_file()
{
      FILE *fp =NULL;
      char file_name[100];
      char answer_1;
      char buffer_1[10];
      char buffer_2[10];
      char answer_2;


      printf("do you want to make a file?[y/n]\n");
      fgets(buffer_1 , sizeof(buffer_1) , stdin);
      answer_1= buffer_1[0];
      if (answer_1 == 'n' || answer_1 == 'N')
      {
          printf("file creation cancelled.\n");
          return;
      }
      else if (answer_1 == 'y' || answer_1 == 'Y')
      {
          printf("enter file name:\n");
          fgets(file_name , sizeof(file_name) , stdin);
          file_name[strcspn(file_name ,"\n")] = '\0';
          if(file_name[0] == '\0')
          {
              printf("file name can not be empty.\n");
              return;
          }

          fp = fopen(file_name , "w");
          if(fp == NULL)
                 {
                  printf("error file creating.\n");
                   return;
                  }

          printf("file create successfully!\n");
          printf("Do you want to add initial content? (y/n)");
          fgets(buffer_2 , sizeof(buffer_2), stdin);
          answer_2 = buffer_2[0];
          if (answer_2 =='n' || answer_2 == 'N')
              {
                  printf("no content added.\n");

              }
          else if(answer_2 == 'y' || answer_2 == 'Y')

              {
                 char content[500];
                 printf("enter content:");
                 fgets(content , sizeof(content) , stdin);
                 content[strcspn(content, "\n")] = '\0';
                 fprintf(fp , "%s" , content);
              }


           else
              {
                printf("input is invalid.\n");
              }
      }

    else
        {
             printf("input is invalid.\n");
        }

      if(fp != NULL)
      {
          fclose(fp);
      }
}



void read_file()
{
     char file_name[100];
     char line[800];
     FILE *fp = NULL;

     printf("enter file name:\n");
     fgets(file_name , sizeof(file_name) , stdin);
     file_name[strcspn(file_name , "\n")] = '\0';

     fp = fopen(file_name , "r");

     if(fp == NULL)
     {
         printf("Error: Cannot open file. It may not exist or access is denied.\n");
         return;
     }

     while(fgets(line, sizeof(line) ,fp) != NULL )
     {

         printf("%s" , line);

     }

     printf("\n____END OF FILE____\n");
     fclose(fp);
}



void update_file()
{
     char file_name[100];
     char new_content[500];
     FILE *fp;

     printf("enter file name:\n");
     fgets(file_name , sizeof(file_name) , stdin);
     file_name[strcspn(file_name , "\n")] = '\0';

     printf("choose mode : \n1.append \n2.overwrite\n");
     char buffer[100];
     char *endptr;
     fgets(buffer, sizeof(buffer), stdin);
     if (buffer[0] == '\n' )
     {
         printf("invalid input.\n");
         return;
     }
     long mode = strtol(buffer, &endptr , 10);
     if(endptr == buffer || (*endptr !='\n' && *endptr != '\0'))
     {
         printf("invalid input.\n");
         return;
     }

     if(mode == 1)
     {
           fp = fopen(file_name , "a");
     }
     else if(mode == 2)
        {
            fp = fopen(file_name , "w");

        }

     else
     {
         printf("invalid mode.\n");
         return;
     }

     if(fp == NULL)
     {
         printf("error : file not found.\n");
         return;
     }

     printf("enter new content:\n");
     fgets(new_content, sizeof(new_content) , stdin);
     new_content[strcspn(new_content , "\n")] = '\0';
     fprintf(fp , "%s" , new_content);

     printf("file update successfully!\n");

     fclose(fp);

}


void delete_file()
{

     char file_name[100];

     printf("enter file name:");
     fgets(file_name , sizeof(file_name) , stdin);
     file_name[strcspn(file_name , "\n")] = '\0';
     char buffer[10];
     char delete_option;

     printf("do you want to delete %s ? (y/n)" , file_name);
     fgets(buffer , sizeof(buffer) , stdin);
     delete_option = buffer[0];

     if(delete_option == 'y' || delete_option == 'Y')
     {

     if(remove(file_name)== 0)
     {
         printf("file delete successfully.\n");
     }

     else
     {
        perror("Error deleting file.");
     }

    }

    else if(delete_option == 'n' || delete_option == 'N')
    {
          printf("file not delete.");
          return;
    }

    else
    {
        printf("input is invalid.\n");
    }
}


void rename_file()
{
    char old_name[100];
    char new_name[100];

    printf("enter the name of the file you want to rename:\n");
    fgets(old_name , sizeof(old_name) , stdin);
    old_name[strcspn(old_name, "\n")] = '\0';

    FILE *fp = fopen(old_name , "r");
    if (fp == NULL)
    {
        printf("file not found.\n");
        return;
    }
    fclose(fp);

    printf("enter the new name:\n");
    fgets(new_name, sizeof(new_name), stdin);
    new_name[strcspn(new_name,"\n")] = '\0';
    if (new_name[0] == '\0')
    {
        printf("new name can't be empty.\n");
        return;
    }

    if(rename(old_name, new_name)== 0)
    {
        printf("file renamed successfully.\n");
    }

    else
    {
        perror("rename failed.");
    }


}



void copy_file()
{
    char dest[200];
    char source[200];
    char buffer[1024];

    printf("enter source file name:");
    fgets(source, sizeof(source) , stdin);
    source[strcspn(source, "\n")] ='\0';



    FILE *src = fopen(source , "r");
    if(src == NULL)
    {
        printf("source file not found\n");
        return;
    }

    printf("enter destination file name:");
    fgets(dest, sizeof(dest) , stdin);
    dest[strcspn(dest,"\n")] = '\0';


    FILE *dst = fopen(dest, "w");
    if(dst == NULL)
    {
        printf("cannot create destination file.\n");
        fclose(src);
        return;

    }

    while(fgets(buffer , sizeof(buffer) , src) != NULL)
    {
        fputs(buffer, dst);
    }

        printf("file copied successfully!\n");

    fclose(src);
    fclose(dst);


}











void move_file()
{
    char file_name[100];
    char buffer[10];
    char answer;
    FILE *fp;
    DIR *dir;
    char folder_name[100];
    char destination[300];
    printf("do you want to move a file?[y/n]\n");
    fgets(buffer , sizeof(buffer) , stdin);
    answer = buffer[0];

    if(answer == 'n' || answer =='N')
    {
        printf("okay. back to menu...\n");
        return;
    }
    else if (answer == 'y' || answer == 'Y')
     {
        printf("enter file name: ");
        fgets(file_name , sizeof(file_name) , stdin);
        file_name[strcspn(file_name, "\n")] = '\0';

        if (file_name[0] == '\0')
            {
                printf("file name can not be empty.\n");
                return;
            }
       fp = fopen(file_name,"r");
       if(fp == NULL)
           {
              printf("can not find the file.\n");
              return;
           }
        fclose(fp);
            printf("enter the new folder name: ");
            fgets(folder_name , sizeof(folder_name) , stdin);
            folder_name[strcspn(folder_name , "\n")] = '\0';
            if(folder_name[0] == '\0')
            {
                printf("folder name can not be empty.\n");
                return;
            }
            dir = opendir(folder_name);
            if (dir == NULL)
            {
                printf("folder can't be found.\n");
                return;
            }

        closedir(dir);
        sprintf(destination, "%s%s%s" ,folder_name, PATH_SEPARATOR,  file_name);
                if(rename(file_name , destination) == 0)
                  {
                    printf("file moved successfully.\n");
                    return;
                  }

                else
                {
                    perror("move file failed.");
                }
             }

    else
    {
        printf("invalid input.");
        return;
    }

}









