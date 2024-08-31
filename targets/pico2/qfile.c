#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pico/stdlib.h>

#include "ff.h"

#include "../../quake/qfile.h"

QFILE *Qfopen(const char *filename, const char *mode)
{
    printf("fopen( %s )\r\n", filename);

    FIL *f = malloc( sizeof(FIL) );
    uint8_t FLAGS = 0;

    if ( f == 0 )
    {
        panic("fopen failed malloc");
        return 0;
    }

    memset((void*)f, 0, sizeof(FIL));

/*
"r"     FA_READ
"r+"    FA_READ          | FA_WRITE
"w"     FA_CREATE_ALWAYS | FA_WRITE
"w+"    FA_CREATE_ALWAYS | FA_WRITE | FA_READ
"a"     FA_OPEN_APPEND   | FA_WRITE
"a+"    FA_OPEN_APPEND   | FA_WRITE | FA_READ
"wx"    FA_CREATE_NEW    | FA_WRITE
"w+x"   FA_CREATE_NEW    | FA_WRITE | FA_READ
*/

    // This logic is flawed but it works well enough for quake
    if ( mode ) {
        while (*mode) {
            switch (*mode) {
            case 'r': FLAGS |= FA_READ; break;
            // case 'w': FLAGS |= FA_WRITE; break;
            // case 'a': FLAGS |= FA_OPEN_APPEND; break;
            case 'b': break; // Always in binary mode
            default: panic("Unknown file flag %c", *mode); // printf("Unknown file flag %c\r\n", *mode); break;
            }
            mode++;
        }
    }

    if ( FLAGS == 0 )
    {
        panic("No file flags set");
    }

	if ( f_open(f, filename, FLAGS) != FR_OK )
	{
        printf("f_open() failed\r\n");
        free( f );
		return 0;
	}

    return (QFILE*)f;
}

int Qfclose(QFILE *stream)
{
    FRESULT res;

    if ( stream == 0 )
    {
        panic("Qfclose() invalid arguments");
        return EOF;
    }

    res = f_close( (FIL*) stream );

    free( stream );

    return (res == FR_OK) ? 0 : EOF;
}

size_t Qfread(void *ptr, size_t size, size_t nmemb, QFILE *stream)
{
    UINT bytesRead = 0;
    size_t totBytes = (size * nmemb);

    if ( totBytes == 0 || ptr == 0 || stream == 0 )
    {
        panic("Qfread() invalid arguments");
        return 0;
    }

    // FSIZE_t currPos = f_tell((FIL*) stream);

	if ( f_read((FIL*) stream, ptr, totBytes, &bytesRead) != FR_OK || totBytes != bytesRead )
	{
		printf("fread() failed\r\n");
        panic("f_read failed");
	}

    // f_lseek((FIL*) stream, currPos + bytesRead);

    // Return number of elements, not number of read bytes
    return (size_t)(bytesRead / size);
}

size_t Qfwrite(const void *ptr, size_t size, size_t nmemb, QFILE *stream)
{
    UINT bytesWritten = 0;

    panic("Qfwrite() too early");

    if ( size == 0 || nmemb == 0 || ptr == 0 || stream == 0 )
    {
        panic("Qfwrite() invalid arguments");
        return 0;
    }

	if ( f_write((FIL*) stream, ptr, size * nmemb, &bytesWritten) != FR_OK )
	{
		printf("fwrite() failed\r\n");
	}

    // Return number of elements, not number of read bytes
    return (size_t)(bytesWritten / size);
}

int Qfgetc(QFILE *stream)
{
    UINT bytesRead = 0;
    uint8_t data = 0;

    if ( stream == 0 )
    {
        panic("Qfgetc no ptr");
        return EOF;
    }

    // FSIZE_t currPos = f_tell((FIL*) stream);

	if ( f_read((FIL*) stream, &data, 1, &bytesRead) != FR_OK || bytesRead != 1 )
	{
		panic("fgetc() failed\r\n");
        return EOF;
	}

    // f_lseek((FIL*) stream, currPos + 1);

    return data;
}

int Qfseek(QFILE *stream, long int offset, int whence)
{
    // FSIZE_t currPos;

    if ( stream == 0 )
    {
        panic("Qfseek no ptr");
        return EOF;
    }

    switch (whence) {
    case SEEK_SET: // Set absolute position
        if ( offset < 0 || offset >= f_size((FIL*)stream) )
        {
            panic("fseek oor\r\n");
        }

    	if ( f_lseek((FIL*) stream, (FSIZE_t)offset) != FR_OK )
	    {
		    printf("f_lseek( SEEK_SET ) failed\r\n");
            return EOF;
	    }
        return 0;

/*
    case SEEK_CUR: // Set relative position
        currPos = f_tell((FIL*)stream);
    	if ( f_lseek((FIL*) stream, (FSIZE_t)(currPos + offset)) != FR_OK )
	    {
		    printf("f_lseek( SEEK_CUR ) failed\r\n");
            return EOF;
	    }
        return 0;

    case SEEK_END: // From last byte + 1 +/- offset
        currPos = f_size((FIL*)stream);
        if ( offset > 0 )
        {
            panic("SEEK_END - positive offset");
        }
    	if ( f_lseek((FIL*) stream, (FSIZE_t)((long int)offset + currPos)) != FR_OK )
	    {
		    printf("f_lseek( SEEK_END ) failed\r\n");
            return EOF;
	    }
        return 0;
*/
    default:
        panic("Qfseek unknown seek mode");
        return EOF;
    }

    return 0;
}
















int Qfflush(QFILE *stream)
{
    panic("Qfflush too early");

    if ( stream == 0 )
    {
        panic("Qfflush no ptr");
        return EOF;
    }

	if ( f_sync((FIL*) stream) != FR_OK )
	{
		printf("fflush() failed\r\n");
        return EOF;
	}

    return 0;
}



int Qfeof(QFILE *stream)
{
    panic("Qfeof() used\n");
    return EOF;
}

int Qfscanf(QFILE *stream, const char *format, ...)
{
    panic("Qfscanf() used\n");
    printf("Warning: Qfscanf ( %s )\r\n", format);
    return EOF;
}

int Qfprintf(QFILE *stream, const char *format, ...)
{
    panic("Qfprintf() used\n");
    printf("Warning: Qfprintf ( %s )\r\n", format);
    return EOF;
}

int Qopen(const char *pathname, int flags, ... )
{
    panic("Qopen() used\n");
    printf("Warning: Qopen() used\r\n");
    return EOF;
}

ssize_t Qwrite(int fd, const void *buf, size_t count)
{
    panic("Qwrite() used\n");
    printf("Warning: Qwrite() used\r\n");
    return EOF;
}

int Qclose(int fildes)
{
    panic("Qclose() used\n");
    printf("Warning: Qclose() used\r\n");
    return EOF;
}

