#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "../include/folder_operations.h"
#include "../include/input_utls.h"

#ifdef _WIN32
   #include <direct.h>
   #define MKDIR(name) _mkdir(name)
   #define RMDIR(name) _rmdir(name)


#else
    #include <unistd.h>
    #define MKDIR(name) mkdir(name,0777)
    #define RMDIR(name) rmdir(name)

#endif // WIN32

#define NAME_SIZE 250


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
        printf("FOLDER NAME CAN'T BE EMPTY.\n");
        return 0;
    }
    return 1;
}

typedef enum
{
    KIND_MISSING = 0,
    KIND_DIR,
    KIND_OTHER,
    KIND_ERROR
} PathKind;

/* unlike opendir(), stat() tells "does not exist" apart from "exists but is a file / no permission" */
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

    return S_ISDIR(info.st_mode) ? KIND_DIR : KIND_OTHER;
}


/* ---------- folder operations ---------- */

void create_folder(FileManager *fm)
{
    char name_of_folder[NAME_SIZE];
    (void)fm;

    int answer = ask_yes_no("DO YOU WANT TO MAKE A FOLDER?[y/n]\n");
    if(answer != 1)
    {
        if(answer == 0)
        {
            printf("FOLDER CREATION CANCELLED.\n");
        }
        return;
    }

    if(!read_name("ENTER NAME OF FOLDER:\n", name_of_folder, sizeof(name_of_folder)))
    {
        return;
    }

    switch(path_kind(name_of_folder))
    {
        case KIND_DIR:
            printf("THERE IS A FOLDER WITH THIS NAME ALREADY.\n");
            return;
        case KIND_OTHER:
            printf("THERE IS A FILE WITH THIS NAME ALREADY.\n");
            return;
        case KIND_ERROR:
            return;
        default:
            break;
    }

    if(MKDIR(name_of_folder) == 0)
    {
        printf("FOLDER CREATED SUCCESSFULLY.\n");
    }

    else
    {
        perror("MKDIR FAILED");
    }
}


void delete_folder(FileManager *fm)
{
    char folder_name[NAME_SIZE];
    (void)fm;

    int answer = ask_yes_no("DO YOU WANT TO DELETE A FOLDER?[y/n]\n");
    if(answer != 1)
    {
        if(answer == 0)
        {
            printf("FOLDER IS NOT DELETED.\n");
        }
        return;
    }

    if(!read_name("ENTER FOLDER NAME:\n", folder_name, sizeof(folder_name)))
    {
        return;
    }

    switch(path_kind(folder_name))
    {
        case KIND_MISSING:
            printf("THERE IS NO FOLDER WITH THIS NAME.\n");
            return;
        case KIND_OTHER:
            printf("THIS IS NOT A FOLDER.\n");
            return;
        case KIND_ERROR:
            return;
        default:
            break;
    }

    if(RMDIR(folder_name) == 0)
    {
        printf("FOLDER DELETED SUCCESSFULLY.\n");
        return;
    }

    if(errno == ENOTEMPTY || errno == EEXIST)
    {
        printf("FOLDER IS NOT EMPTY. DELETE ITS CONTENT FIRST.\n");
        return;
    }

    perror("RMDIR FAILED");
}


void file_list_in_folder(FileManager *fm)
{
    DIR *dir;
    char folder_name[NAME_SIZE];
    struct dirent *entry;
    int count = 0;
    (void)fm;

    int answer = ask_yes_no("DO YOU WANT TO SEE A LIST OF A FOLDER?[y/n]\n");
    if(answer != 1)
    {
        if(answer == 0)
        {
            printf("OKAY. BACK TO MENU...\n");
        }
        return;
    }

    if(!read_name("ENTER FOLDER NAME: ", folder_name, sizeof(folder_name)))
    {
        return;
    }

    dir = opendir(folder_name);
    if(dir == NULL)
    {
        perror("CANNOT OPEN FOLDER");
        return;
    }

    /* errno must be reset right before EVERY readdir() call: printf() inside the loop
       may change errno, and readdir() only signals an error by setting it */
    while(1)
    {
        errno = 0;
        entry = readdir(dir);
        if(entry == NULL)
        {
            break;
        }

        if((strcmp(entry->d_name, ".") == 0)
        || (strcmp(entry->d_name, "..") == 0))
        {
            continue;
        }

        printf("%s\n", entry->d_name);
        count++;
    }

    if(errno != 0)
    {
        perror("READDIR FAILED");
    }

    else if(count == 0)
    {
        printf("THE FOLDER IS EMPTY.\n");
    }

    if(closedir(dir) == -1)
    {
        perror("CLOSEDIR FAILED");
    }
}
