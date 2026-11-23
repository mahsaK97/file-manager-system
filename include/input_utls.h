#ifndef INPUT_UTLS_H
#define INPUT_UTLS_H
#include <stdio.h>




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

FILE *safe_fopen(const char *path, const char *mode, IoState *st);
IoState safe_fclose(FILE **fp);
IoState safe_fput(const char *s, FILE *fp);
IoState safe_fgets(char *buf, int size, FILE *fp);


#endif // INPUT_UTLS_H

