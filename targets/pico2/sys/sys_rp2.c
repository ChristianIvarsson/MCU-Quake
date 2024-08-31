/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pico/stdlib.h>

#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>

// #include "pico.h"


#include "ff.h"

#include "../../../quake/quakedef.h"
#include "../../../quake/qfile.h"
#include "errno.h"

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10
static FILE    *sys_handles[MAX_HANDLES];

int             findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}
/*
================
filelength
================
*/
int filelength (FILE *f)
{
	if ( f == 0 )
	{
		panic("filelength zero pointer");
	}

	return f_size((FIL*)f);
}

int Sys_FileOpenRead (const char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = Qfopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int Sys_FileOpenWrite (const char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = Qfopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	if ( f_close( (FIL*)sys_handles[handle] ) != FR_OK )
	{
		printf("f_close() failed\r\n");
	}

	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	if ( position < 0 )
	{
		printf("f_lseek() < 0\r\n");
		position = 0;
	}

	if ( f_lseek((FIL*)sys_handles[handle], (FSIZE_t)position) != FR_OK )
	{
		printf("f_lseek() failed\r\n");
	}
}

int Sys_FileRead(int handle, void *dest, int count)
{
	UINT bytesRead = 0;

	if ( f_read((FIL*)sys_handles[handle], dest, count, &bytesRead) != FR_OK || count != bytesRead )
	{
		printf("f_read() failed\r\n");
	}

	return (int)bytesRead;
}

int Sys_FileWrite (int handle, const void *data, int count)
{
	return Qfwrite (data, 1, count, sys_handles[handle]);
}

int Sys_FileTime (const char *path)
{
	FILE    *f;
	
	f = Qfopen(path, "rb");
	if (f)
	{
		Qfclose(f);
		return 1;
	}
	
	return -1;
}

void Sys_mkdir (const char *path)
{
}

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{ }

void Sys_Error(const char *error, ...)
{
	va_list argptr;

	printf ("Sys_Error: ");   
	va_start (argptr,error);
	vprintf (error,argptr);
	va_end (argptr);
	printf ("\n");

	panic("Sys_Error()");
}

void Sys_Printf(const char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);
}


void Sys_Quit (void)
{
	panic("Sys_Quit()");
}



/*
// TODO:
double Sys_FloatTime(void)
{
	static double t;
	
	// t += 0.1;
	return t;
}
*/

double Sys_FloatTime (void)
{
    return (double)time_us_64() / 1000000.0; // (tp.tv_sec - secbase) + tp.tv_usec/1000000.0;
}

char *Sys_ConsoleInput(void) { return NULL; }
void Sys_Sleep(void) { }
void Sys_SendKeyEvents(void) { }
void Sys_HighFPPrecision(void) { }
void Sys_LowFPPrecision(void) { }

//=============================================================================
// 

void quakeLoop(void *buf, uint32_t bufSize, int argc, const char *argv [])
{
	double time, oldtime, newtime;
	quakeparms_t parms;
	extern int vcrFile;
	extern int recording;

	FATFS fs;
    FRESULT fr;

#ifdef TRACE_STACK
	double stackTime = 0;
	COM_InitStackTrace(&stackTime);
#endif


	printf ("=== Quake() ===\n");

	printf("Using buffer at %08lx ( %lu bytes )\n", (uint32_t)buf, bufSize);

	printf ("Sdcard init..\n");

	// quake
    if ( (fr = f_mount(&fs, "", 1)) != FR_OK )
	{
		panic("f_mount error: %d\n", fr);
	}

	parms.memsize = (int)bufSize;
	parms.membase = buf;
	parms.basedir = ".";

	printf ("COM_InitArgv..\n");
	COM_InitArgv(argc, argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	printf ("Host_Init..\n");
	Host_Init (&parms);

	printf("At main loop\n");

    oldtime = Sys_FloatTime();
#ifdef TRACE_STACK
	stackTime = oldtime;
#endif

    while (1)
    {
// find time spent rendering last frame
        newtime = Sys_FloatTime();
        time = newtime - oldtime;

        if (cls.state == ca_dedicated)
        {   // play vcrfiles at max speed
            if (time < sys_ticrate.value && (vcrFile == -1 || recording) )
            {
				sleep_us(1);
                continue;       // not time to run a server only tic yet
            }
            time = sys_ticrate.value;
        }

        if (time > sys_ticrate.value*2)
            oldtime = newtime;
        else
            oldtime += time;

        Host_Frame ( time );

#ifdef TRACE_STACK
		if ((newtime - stackTime) >= 1)
		{
			stackTime = newtime;
			printf("Max Depth: %lu\n", COM_GetMaxStack());
		}
#endif

/*
// graphic debugging aids
        if (sys_linerefresh.value)
            Sys_LineRefresh ();*/
    }




/*
	while ( 1 )
	{
		Host_Frame (0.1);
	}
*/

}
