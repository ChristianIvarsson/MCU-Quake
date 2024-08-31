#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "quakedef.h"
#include "qfile.h"

#define __WEAK__   __attribute__((weak))

// Last argument is always 666 so we'll just ignore the vlb part
int __WEAK__ Qopen(const char *pathname, int flags, ... )
{ return open(pathname, flags, 666); }

ssize_t __WEAK__ Qwrite(int fd, const void *buf, size_t count)
{ return write(fd, buf, count); }

int __WEAK__ Qclose(int fildes)
{ return close(fildes); }

QFILE __WEAK__ *Qfopen(const char *filename, const char *mode)
{ return (QFILE*)fopen(filename, mode); }

int __WEAK__ Qfclose(QFILE *stream)
{ return fclose((FILE*)stream); }

size_t __WEAK__ Qfread(void *ptr, size_t size, size_t nmemb, QFILE *stream)
{ return fread(ptr, size, nmemb, (FILE*)stream); }

int __WEAK__ Qfgetc(QFILE *stream)
{ return fgetc((FILE*)stream); }

size_t __WEAK__ Qfwrite(const void *ptr, size_t size, size_t nmemb, QFILE *stream)
{ return fwrite(ptr, size, nmemb, (FILE*)stream); }

int __WEAK__ Qfseek(QFILE *stream, long int offset, int whence)
{ return fseek((FILE*)stream, offset, whence); }

int __WEAK__ Qfflush(QFILE *stream)
{ return fflush((FILE*)stream); }

int __WEAK__ Qfscanf(QFILE *stream, const char *format, ...)
{
    int retval;
    va_list argptr;
    va_start(argptr, format);
    retval = fscanf((FILE*)stream, format, argptr);
    va_end(argptr);
    return retval;
}

int __WEAK__ Qfprintf(QFILE *stream, const char *format, ...)
{
    int retval;
    va_list argptr;
    va_start(argptr, format);
    retval = fprintf((FILE*)stream, format, argptr);
    va_end(argptr);
    return retval;
}

int __WEAK__ Qfeof(QFILE *stream)
{
    return feof((FILE*)stream);
}