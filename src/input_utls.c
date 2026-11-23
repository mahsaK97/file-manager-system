#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../include/input_utls.h"




void clear_input_buffer()
{
    int c;
    while((c=getchar())!= '\n' && c !=EOF)
    {

    }

}



static void report(const char *op, const char *detail)
{
    fprintf(stderr, "[%s] %s: %s\n", op, detail ? detail : "-", strerror(errno));
}

FILE *safe_fopen(const char *path, const char *mode, IoStatus *st)
{
    if (!path || !mode) {
        if (st) *st = IO_ERR_NULL_ARG;
        return NULL;
    }

    FILE *fp = fopen(path, mode);
    if (!fp) {
        report("fopen", path);
        if (st) *st = IO_ERR_OPEN;
        return NULL;
    }

    if (st) *st = IO_OK;
    return fp;
}


IoStatus safe_fclose(FILE **fp)
{
    if (!fp || !*fp)
        return IO_ERR_NULL_ARG;

    int rc = fclose(*fp);
    *fp = NULL;

    if (rc != 0) {
        report("fclose", NULL);
        return IO_ERR_CLOSE;
    }
    return IO_OK;
}

IoStatus safe_fputs(const char *s, FILE *fp)
{
    if (!s || !fp)
        return IO_ERR_NULL_ARG;

    if (fputs(s, fp) == EOF) {
        report("fputs", NULL);
        return IO_ERR_WRITE;
    }
    return IO_OK;
}

IoStatus safe_fgets(char *buf, int size, FILE *fp)
{
    if (!buf || size <= 0 || !fp)
        return IO_ERR_NULL_ARG;

    if (fgets(buf, size, fp) == NULL) {
        if (feof(fp))
            return IO_EOF;
        report("fgets", NULL);
        return IO_ERR_READ;
    }
    return IO_OK;
}
