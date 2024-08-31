#ifndef __QFILE_H__
#define __QFILE_H__

typedef void QFILE;

int Qopen(const char *pathname, int flags, ... );
ssize_t Qwrite(int fd, const void *buf, size_t count);
int Qclose(int fildes);


QFILE *Qfopen(const char *filename, const char *mode);
int Qfclose(QFILE *stream);

size_t Qfread(void *ptr, size_t size, size_t nmemb, QFILE *stream);
size_t Qfwrite(const void *ptr, size_t size, size_t nmemb, QFILE *stream);

int Qfseek(QFILE *stream, long int offset, int whence);
int Qfgetc(QFILE *stream);
int Qfflush(QFILE *stream);

int Qfscanf(QFILE *stream, const char *format, ...);
int Qfprintf(QFILE *stream, const char *format, ...);

int Qfeof(QFILE *stream);


#endif
