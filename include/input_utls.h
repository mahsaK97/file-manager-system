#ifndef INPUT_UTLS_H
#define INPUT_UTLS_H
#include <stdio.h>


/* throws away the rest of the current stdin line (use after fgets read a line without '\n') */
void clear_input_buffer(void);


typedef enum
{
    IO_OK = 0,
    IO_EOF,
    IO_ERR_NULL_ARG,
    IO_ERR_OPEN,
    IO_ERR_CLOSE,
    IO_ERR_READ,
    IO_ERR_WRITE
}IoState;

/* opens a file; prints the reason to stderr and returns NULL on failure.
   "st" may be NULL when the caller does not need the status code. */
FILE *safe_fopen(const char *path, const char *mode, IoState *st);

/* closes the file and sets *fp to NULL, so a second call can't double-close it */
IoState safe_fclose(FILE **fp);

/* writes the string to the file (fputs wrapper) */
IoState safe_fput(const char *s, FILE *fp);

/* reads one line (fgets wrapper); returns IO_EOF at the end of the file */
IoState safe_fgets(char *buf, int size, FILE *fp);


#endif // INPUT_UTLS_H
