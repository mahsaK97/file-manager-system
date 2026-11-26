#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "../include/file_operations.h"
#include "../include/input_utls.h"

#ifdef _WIN32

     #define PATH_SEPARATOR "\\"

#else
     #define PATH_SEPARATOR  "/"

#endif

#define NAME_SIZE     250
#define CONTENT_SIZE  2048
#define PROMPT_SIZE   320
#define COPY_CHUNK    4096


/* ---------- helpers ---------- */

static int read_line(char *buf, size_t size)
{
    if(fgets(buf,(int)size,stdin)== NULL)
    {
        if(feof(stdin))
        {
            printf("END OF FILE\n");
        }

        else
        {
            perror("fgets");
        }
        return 0;
    }
    if(strchr(buf,'\n')== NULL)
    {
        clear_input_buffer();
    }
    buf[strcspn(buf, "\n")] ='\0';
    return 1;
}

/* returns 1 = yes, 0 = no, -1 = invalid input or read error */
static int ask_yes_no(const char *prompt)
{
    char answer[16];

    printf("%s", prompt);
    if(!read_line(answer, sizeof(answer)))
    {
        return -1;
    }

    if(answer[0] == 'y' || answer[0] == 'Y')
    {
        return 1;
    }

    if(answer[0] == 'n' || answer[0] == 'N')
    {
        return 0;
    }

    printf("INVALID INPUT.\n");
    return -1;
}

/* prints the prompt, reads a name, rejects empty names. returns 1 on success */
static int read_name(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    if(!read_line(buf, size))
    {
        return 0;
    }

    if(buf[0] == '\0')
    {
        printf("NAME CAN'T BE EMPTY.\n");
        return 0;
    }
    return 1;
}

typedef enum
{
    KIND_MISSING = 0,
    KIND_FILE,
    KIND_OTHER,
    KIND_ERROR
} PathKind;

static PathKind path_kind(const char *path)
{
    struct stat info;

    if(stat(path, &info) != 0)
    {
        if(errno == ENOENT)
        {
            return KIND_MISSING;
        }
        perror("stat");
        return KIND_ERROR;
    }

    return S_ISREG(info.st_mode) ? KIND_FILE : KIND_OTHER;
}

/* returns 1 only when path exists and is a regular file; prints the reason otherwise */
static int require_regular_file(const char *path)
{
    switch(path_kind(path))
    {
        case KIND_FILE:
            return 1;
        case KIND_MISSING:
            printf("THERE IS NO FILE WITH THIS NAME.\n");
            return 0;
        case KIND_OTHER:
            printf("THIS IS NOT A REGULAR FILE.\n");
            return 0;
        default:
            return 0;
    }
}

static const char *base_name(const char *path)
{
    const char *p = strrchr(path, '/');
#ifdef _WIN32
    const char *q = strrchr(path, '\\');
    if(q != NULL && (p == NULL || q > p))
    {
        p = q;
    }
#endif
    return (p == NULL) ? path : p + 1;
}

/* on Windows rename() fails when the target exists, so it must be removed first */
static void prepare_overwrite(const char *path)
{
#ifdef _WIN32
    remove(path);
#else
    (void)path;
#endif
}

/* copies bytes (binary safe). removes the partial destination on failure. returns 1 on success */
static int copy_file_data(const char *source, const char *dest)
{
    char chunk[COPY_CHUNK];
    size_t n;
    int failed = 0;

    FILE *src = safe_fopen(source, "rb", NULL);
    if(src == NULL)
    {
        return 0;
    }

    FILE *dst = safe_fopen(dest, "wb", NULL);
    if(dst == NULL)
    {
        safe_fclose(&src);
        return 0;
    }

    while((n = fread(chunk, 1, sizeof(chunk), src)) > 0)
    {
        if(fwrite(chunk, 1, n, dst) != n)
        {
            perror("ERROR WRITING FILE");
            failed = 1;
            break;
        }
    }

    if(!failed && ferror(src))
    {
        perror("ERROR READING FILE");
        failed = 1;
    }

    safe_fclose(&src);
    if(safe_fclose(&dst) != IO_OK)
    {
        failed = 1;
    }

    if(failed)
    {
        remove(dest);
        return 0;
    }
    return 1;
}


/* ---------- file operations ---------- */

void create_file(FileManager *fm)
{
      char file_name[NAME_SIZE];
      char content[CONTENT_SIZE];
      (void)fm;

      if(ask_yes_no("DO YOU WANT TO CREATE A FILE?[y/n]\n") != 1)
      {
          printf("OKAY. BACK TO MENU...\n");
          return;
      }

      if(!read_name("ENTER FILE NAME: ", file_name, sizeof(file_name)))
      {
          return;
      }

      switch(path_kind(file_name))
      {
          case KIND_FILE:
              if(ask_yes_no("A FILE WITH THIS NAME ALREADY EXISTS. REPLACE IT?[y/n]\n") != 1)
              {
                  printf("BACK TO MENU...\n");
                  return;
              }
              break;
          case KIND_OTHER:
              printf("A FOLDER OR SPECIAL FILE WITH THIS NAME ALREADY EXISTS.\n");
              return;
          case KIND_ERROR:
              return;
          default:
              break;
      }

      int add = ask_yes_no("DO YOU WANT TO ADD INITIAL CONTENT?[y/n]\n");
      if(add == -1)
      {
          return;
      }

      /* read the content BEFORE opening with "w", so a failed read never truncates an existing file */
      content[0] = '\0';
      if(add == 1)
      {
          printf("ENTER CONTENT:\n");
          if(!read_line(content, sizeof(content)))
          {
              return;
          }
      }

      FILE *fp = safe_fopen(file_name, "w", NULL);
      if(fp == NULL)
      {
          return;
      }

      if(add == 1 && safe_fput(content, fp) != IO_OK)
      {
          safe_fclose(&fp);
          return;
      }

      if(safe_fclose(&fp) != IO_OK)
      {
          return;
      }

      printf("FILE CREATED SUCCESSFULLY.\n");
}


void read_file(FileManager *fm)
{
     char file_name[NAME_SIZE];
     char line[1024];
     (void)fm;

     if(ask_yes_no("DO YOU WANT TO READ A FILE?[y/n]\n") != 1)
     {
         printf("OKAY. BACK TO MENU...\n");
         return;
     }

     if(!read_name("ENTER FILE NAME: ", file_name, sizeof(file_name)))
     {
         return;
     }

     if(!require_regular_file(file_name))
     {
         return;
     }

     FILE *fp = safe_fopen(file_name, "r", NULL);
     if(fp == NULL)
     {
         return;
     }

     printf("______FILE CONTENT______\n");
     while(fgets(line, sizeof(line), fp) != NULL)
     {
         fputs(line, stdout);
     }

     if(ferror(fp))
     {
         perror("ERROR READING FILE");
     }

     printf("\n_____END OF FILE_____\n");
     safe_fclose(&fp);
}


void update_file(FileManager *fm)
{
     char file_name[NAME_SIZE];
     char mode_text[16];
     char new_content[CONTENT_SIZE];
     (void)fm;

     if(!read_name("ENTER FILE NAME: ", file_name, sizeof(file_name)))
     {
         return;
     }

     if(!require_regular_file(file_name))
     {
         return;
     }

     printf("CHOOSE MODE:\n1.append\n2.overwrite\n");
     if(!read_line(mode_text, sizeof(mode_text)))
     {
         return;
     }

     char *endptr;
     long mode = strtol(mode_text, &endptr, 10);
     if(endptr == mode_text || *endptr != '\0')
     {
         printf("INVALID INPUT.\n");
         return;
     }

     if(mode != 1 && mode != 2)
     {
         printf("INVALID MODE.\n");
         return;
     }

     /* read the new content BEFORE opening: mode "w" truncates the file immediately,
        so a failed read after fopen would destroy the old content */
     printf("ENTER NEW CONTENT:\n");
     if(!read_line(new_content, sizeof(new_content)))
     {
         return;
     }

     FILE *fp = safe_fopen(file_name, (mode == 1) ? "a" : "w", NULL);
     if(fp == NULL)
     {
         return;
     }

     if(safe_fput(new_content, fp) != IO_OK)
     {
         safe_fclose(&fp);
         return;
     }

     if(safe_fclose(&fp) != IO_OK)
     {
         return;
     }

     printf("FILE UPDATED SUCCESSFULLY!\n");
}


void delete_file(FileManager *fm)
{
     char file_name[NAME_SIZE];
     char prompt[PROMPT_SIZE];
     (void)fm;

     if(!read_name("ENTER FILE NAME: ", file_name, sizeof(file_name)))
     {
         return;
     }

     if(!require_regular_file(file_name))
     {
         return;
     }

     snprintf(prompt, sizeof(prompt), "DO YOU WANT TO DELETE %s ?[y/n]\n", file_name);
     if(ask_yes_no(prompt) != 1)
     {
         printf("FILE NOT DELETED.\n");
         return;
     }

     if(remove(file_name) == 0)
     {
         printf("FILE DELETED SUCCESSFULLY.\n");
     }

     else
     {
         perror("ERROR DELETING FILE");
     }
}


void rename_file(FileManager *fm)
{
    char old_name[NAME_SIZE];
    char new_name[NAME_SIZE];
    (void)fm;

    if(!read_name("ENTER THE NAME OF THE FILE YOU WANT TO RENAME:\n", old_name, sizeof(old_name)))
    {
        return;
    }

    if(!require_regular_file(old_name))
    {
        return;
    }

    if(!read_name("ENTER THE NEW NAME:\n", new_name, sizeof(new_name)))
    {
        return;
    }

    if(strcmp(old_name, new_name) == 0)
    {
        printf("OLD NAME AND NEW NAME ARE THE SAME.\n");
        return;
    }

    switch(path_kind(new_name))
    {
        case KIND_FILE:
            if(ask_yes_no("A FILE WITH THIS NAME ALREADY EXISTS. OVERWRITE?[y/n]\n") != 1)
            {
                printf("OKAY. BACK TO MENU...\n");
                return;
            }
            prepare_overwrite(new_name);
            break;
        case KIND_OTHER:
            printf("A FOLDER OR SPECIAL FILE WITH THIS NAME ALREADY EXISTS.\n");
            return;
        case KIND_ERROR:
            return;
        default:
            break;
    }

    if(rename(old_name, new_name) == 0)
    {
        printf("FILE RENAMED SUCCESSFULLY.\n");
    }

    else
    {
        perror("RENAME FAILED");
    }
}


void copy_file(FileManager *fm)
{
    char source[NAME_SIZE];
    char dest[NAME_SIZE];
    (void)fm;

    if(!read_name("ENTER SOURCE FILE NAME: ", source, sizeof(source)))
    {
        return;
    }

    if(!require_regular_file(source))
    {
        return;
    }

    if(!read_name("ENTER DESTINATION FILE NAME: ", dest, sizeof(dest)))
    {
        return;
    }

    if(strcmp(source, dest) == 0)
    {
        printf("SOURCE AND DESTINATION CAN NOT BE THE SAME FILE.\n");
        return;
    }

    if(path_kind(dest) != KIND_MISSING)
    {
        printf("THERE IS ALREADY A FILE OR FOLDER WITH THIS NAME.\n");
        return;
    }

    if(copy_file_data(source, dest))
    {
        printf("FILE COPIED SUCCESSFULLY!\n");
    }

    else
    {
        printf("COPY FAILED.\n");
    }
}


void move_file(FileManager *fm)
{
    char file_name[NAME_SIZE];
    char folder_name[NAME_SIZE];
    char destination[NAME_SIZE * 2];
    struct stat folder_info;
    (void)fm;

    if(ask_yes_no("DO YOU WANT TO MOVE A FILE?[y/n]\n") != 1)
    {
        printf("OKAY. BACK TO MENU...\n");
        return;
    }

    if(!read_name("ENTER FILE NAME: ", file_name, sizeof(file_name)))
    {
        return;
    }

    if(!require_regular_file(file_name))
    {
        return;
    }

    if(!read_name("ENTER THE NEW FOLDER NAME: ", folder_name, sizeof(folder_name)))
    {
        return;
    }

    if(stat(folder_name, &folder_info) != 0)
    {
        perror("FOLDER CAN'T BE OPENED");
        return;
    }

    if(!S_ISDIR(folder_info.st_mode))
    {
        printf("THIS IS NOT A FOLDER.\n");
        return;
    }

    /* only the base name goes into the destination folder ("a/b.txt" -> "folder/b.txt") */
    int result = snprintf(destination, sizeof(destination), "%s%s%s",
                          folder_name, PATH_SEPARATOR, base_name(file_name));

    if(result < 0 || (size_t)result >= sizeof(destination))
    {
        printf("DESTINATION PATH IS TOO LONG.\n");
        return;
    }

    if(strcmp(file_name, destination) == 0)
    {
        printf("THE FILE IS ALREADY IN THIS FOLDER.\n");
        return;
    }

    switch(path_kind(destination))
    {
        case KIND_FILE:
            if(ask_yes_no("A FILE WITH THIS NAME ALREADY EXISTS IN THE DESTINATION FOLDER. OVERWRITE?[y/n]\n") != 1)
            {
                printf("OKAY. BACK TO MENU...\n");
                return;
            }
            prepare_overwrite(destination);
            break;
        case KIND_OTHER:
            printf("A FOLDER OR SPECIAL FILE WITH THIS NAME ALREADY EXISTS THERE.\n");
            return;
        case KIND_ERROR:
            return;
        default:
            break;
    }

    if(rename(file_name, destination) == 0)
    {
        printf("FILE MOVED SUCCESSFULLY.\n");
        return;
    }

    /* rename() can't cross file systems (EXDEV): fall back to copy + delete */
    if(errno == EXDEV)
    {
        if(copy_file_data(file_name, destination))
        {
            if(remove(file_name) == 0)
            {
                printf("FILE MOVED SUCCESSFULLY.\n");
            }
            else
            {
                perror("COPIED, BUT COULD NOT REMOVE THE ORIGINAL FILE");
            }
        }
        else
        {
            printf("MOVE FAILED.\n");
        }
        return;
    }

    perror("MOVE FILE FAILED");
}
