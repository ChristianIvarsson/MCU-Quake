#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "../../../quake/quakedef.h"
#include "../../../quake/render/d_local.h"

// Software-defined resolution
#define	BASEWIDTH	320
#define	BASEHEIGHT	240



void initVideo(void);

// video_common.c
extern unsigned short d_8to16table[ 256 ];
extern byte d_buffer[ BASEWIDTH * BASEHEIGHT ];

#endif
