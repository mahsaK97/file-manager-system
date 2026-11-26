#ifndef _WIN32
    #ifndef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 200809L   /* needed for lstat() with -std=c11 */
    #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>


#include "../include/search_operations.h"
#include "../include/input_utls.h"

#ifdef _WIN32
    #define PATH_SEPARATOR "\\"
    #define LSTAT(path, info) stat(path, info)   /* no symbolic links handling on Windows */
#else
    #define PATH_SEPARATOR "/"
    #define LSTAT(path, info) lstat(path, info)
#endif

#define NAME_SIZE  250
#define EXT_SIZE   100
#define PATH_SIZE  1024


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

/* prints the prompt, reads a line, rejects empty input. returns 1 on success */
static int read_name(const char *prompt, const char *what, char *buf, size_t size)
{
    printf("%s", prompt);
    if(!read_line(buf, size))
    {
        return 0;
    }

    if(buf[0] == '\0')
    {
        printf("%s CAN'T BE EMPTY.\n", what);
        return 0;
    }
    return 1;
}

/* asks the first y/n question of a search. returns 1 only when the user said yes */
static int confirm_search(const char *prompt)
{
    int answer = ask_yes_no(prompt);

    if(answer == 0)
    {
        printf("OKAY. BACK TO MENU...\n");
    }
    return answer == 1;
}

/* folder + name -> path. returns 0 when the result does not fit in out */
static int build_path(char *out, size_t size, const char *folder, const char *name)
{
    int result = snprintf(out, size, "%s%s%s", folder, PATH_SEPARATOR, name);

    if(result < 0 || (size_t)result >= size)
    {
        return 0;
    }
    return 1;
}

static int is_dot_entry(const char *name)
{
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

/* d_type is not available everywhere (and is DT_UNKNOWN on some file systems), so stat() is used */
static int is_regular_file(const char *path)
{
    struct stat info;

    if(stat(path, &info) != 0)
    {
        return 0;
    }
    return S_ISREG(info.st_mode);
}

static DIR *open_folder(const char *folder_name)
{
    DIR *dir = opendir(folder_name);

    if(dir == NULL)
    {
        perror("CANNOT OPEN FOLDER");
    }
    return dir;
}

/* closes the folder and reports a readdir() failure that ended the loop early */
static void finish_folder(DIR *dir)
{
    if(errno != 0)
    {
        perror("READDIR FAILED");
    }

    if(closedir(dir) == -1)
    {
        perror("CLOSEDIR FAILED");
    }
}


/* ---------- search operations ---------- */

void search_by_extension(FileManager *fm)
{
    char extension[EXT_SIZE];
    char wanted[EXT_SIZE + 2];
    char folder_name[NAME_SIZE];
    char path[PATH_SIZE];
    DIR *dir;
    struct dirent *entry;
    int found = 0;
    (void)fm;

    if(!confirm_search("DO YOU WANT TO SEARCH BY EXTENSION?[y/n]\n"))
    {
        return;
    }

    if(!read_name("ENTER EXTENSION (.example): ", "EXTENSION", extension, sizeof(extension)))
    {
        return;
    }

    /* accept both ".txt" and "txt" */
    snprintf(wanted, sizeof(wanted), "%s%s", (extension[0] == '.') ? "" : ".", extension);

    if(strcmp(wanted, ".") == 0)
    {
        printf("INVALID EXTENSION.\n");
        return;
    }

    if(!read_name("ENTER FOLDER NAME: ", "FOLDER NAME", folder_name, sizeof(folder_name)))
    {
        return;
    }

    dir = open_folder(folder_name);
    if(dir == NULL)
    {
        return;
    }

    while(1)
    {
        errno = 0;
        entry = readdir(dir);
        if(entry == NULL)
        {
            break;
        }

        if(is_dot_entry(entry->d_name))
        {
            continue;
        }

        /* a leading dot (".gitignore") is a hidden file name, not an extension */
        char *dot = strrchr(entry->d_name, '.');
        if(dot == NULL || dot == entry->d_name || strcmp(dot, wanted) != 0)
        {
            continue;
        }

        if(!build_path(path, sizeof(path), folder_name, entry->d_name))
        {
            printf("PATH IS TOO LONG: %s\n", entry->d_name);
            continue;
        }

        if(is_regular_file(path))
        {
            printf("%s\n", entry->d_name);
            found++;
        }
    }

    finish_folder(dir);

    if(found == 0)
    {
        printf("NO FILE WITH THIS EXTENSION WAS FOUND.\n");
    }
}


void search_by_name(FileManager *fm)
{
    char file_name[NAME_SIZE];
    char folder_name[NAME_SIZE];
    char path[PATH_SIZE];
    DIR *dir;
    struct dirent *entry;
    int found = 0;
    (void)fm;

    if(!confirm_search("DO YOU WANT TO SEARCH A FILE IN A FOLDER?[y/n]\n"))
    {
        return;
    }

    if(!read_name("ENTER FOLDER NAME: ", "FOLDER NAME", folder_name, sizeof(folder_name)))
    {
        return;
    }

    if(!read_name("ENTER FILE NAME: ", "FILE NAME", file_name, sizeof(file_name)))
    {
        return;
    }

    dir = open_folder(folder_name);
    if(dir == NULL)
    {
        return;
    }

    while(1)
    {
        errno = 0;
        entry = readdir(dir);
        if(entry == NULL)
        {
            break;
        }

        if(strcmp(file_name, entry->d_name) != 0)
        {
            continue;
        }

        if(build_path(path, sizeof(path), folder_name, entry->d_name)
        && is_regular_file(path))
        {
            found = 1;
            break;
        }
    }

    if(found)
    {
        errno = 0;   /* leaving the loop with break must not look like a readdir error */
    }
    finish_folder(dir);

    if(found)
    {
        printf("FILE FOUND: %s%s%s\n", folder_name, PATH_SEPARATOR, file_name);
    }

    else
    {
        printf("FILE NOT FOUND.\n");
    }
}


void search_by_size(FileManager *fm)
{
    char size_in_str[50];
    char folder_name[NAME_SIZE];
    char path[PATH_SIZE];
    char *endptr;
    DIR *dir;
    struct stat file_info;
    struct dirent *entry;
    int found = 0;
    (void)fm;

    if(!confirm_search("DO YOU WANT TO SEARCH BY SIZE?[y/n]\n"))
    {
        return;
    }

    if(!read_name("FOLDER NAME: ", "FOLDER NAME", folder_name, sizeof(folder_name)))
    {
        return;
    }

    printf("ENTER SIZE (IN BYTES): ");
    if(!read_line(size_in_str, sizeof(size_in_str)))
    {
        return;
    }

    errno = 0;
    long long wanted_size = strtoll(size_in_str, &endptr, 10);
    if(endptr == size_in_str || *endptr != '\0' || errno == ERANGE || wanted_size < 0)
    {
        printf("INVALID SIZE.\n");
        return;
    }

    dir = open_folder(folder_name);
    if(dir == NULL)
    {
        return;
    }

    while(1)
    {
        errno = 0;
        entry = readdir(dir);
        if(entry == NULL)
        {
            break;
        }

        if(is_dot_entry(entry->d_name))
        {
            continue;
        }

        if(!build_path(path, sizeof(path), folder_name, entry->d_name))
        {
            printf("PATH IS TOO LONG: %s\n", entry->d_name);
            continue;
        }

        if(stat(path, &file_info) != 0 || !S_ISREG(file_info.st_mode))
        {
            continue;
        }

        if((long long)file_info.st_size == wanted_size)
        {
            printf("file name : %s\nfile size : %lld\n", entry->d_name, (long long)file_info.st_size);
            found++;
        }
    }

    finish_folder(dir);

    if(found == 0)
    {
        printf("NO FILE WITH THIS SIZE WAS FOUND.\n");
    }
}


/* symbolic links are not followed (lstat), so a link that points to a parent folder can't cause an endless loop */
static void search_recursive(const char *current_path, const char *file_name, int *found)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    char new_path[PATH_SIZE];

    dir = opendir(current_path);
    if(dir == NULL)
    {
        /* one unreadable sub folder must not stop the whole search */
        fprintf(stderr, "[opendir] %s: %s\n", current_path, strerror(errno));
        return;
    }

    while(1)
    {
        errno = 0;
        entry = readdir(dir);
        if(entry == NULL)
        {
            break;
        }

        if(is_dot_entry(entry->d_name))
        {
            continue;
        }

        if(!build_path(new_path, sizeof(new_path), current_path, entry->d_name))
        {
            printf("PATH IS TOO LONG: %s\n", entry->d_name);
            continue;
        }

        if(LSTAT(new_path, &info) != 0)
        {
            fprintf(stderr, "[stat] %s: %s\n", new_path, strerror(errno));
            continue;
        }

        if(S_ISDIR(info.st_mode))
        {
            search_recursive(new_path, file_name, found);
            continue;
        }

        if(strcmp(entry->d_name, file_name) == 0)
        {
            printf("FILE FOUND: %s\n", new_path);
            (*found)++;
        }
    }

    finish_folder(dir);
}

void search(FileManager *fm)
{
    char file_name[NAME_SIZE];
    int found = 0;

    if(!confirm_search("DO YOU WANT TO SEARCH WITH RECURSIVE?[y/n]\n"))
    {
        return;
    }

    if(!read_name("FILE NAME: ", "FILE NAME", file_name, sizeof(file_name)))
    {
        return;
    }

    search_recursive(fm->current_path, file_name, &found);

    if(found == 0)
    {
        printf("FILE NOT FOUND.\n");
    }
}


void search_menu(FileManager *fm)
{
    char option_text[16];
    char *endptr;

    printf("search menu:\n");
    printf("1.search by name\n");
    printf("2.search by extension\n");
    printf("3.search by size\n");
    printf("4.search with recursive\n");

    if(!read_line(option_text, sizeof(option_text)))
    {
        return;
    }

    long option = strtol(option_text, &endptr, 10);
    if(endptr == option_text || *endptr != '\0')
    {
        printf("INVALID INPUT.\n");
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
