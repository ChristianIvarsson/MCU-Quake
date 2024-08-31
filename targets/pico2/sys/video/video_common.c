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
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/dma.h"

#include "../video.h"

extern viddef_t	vid; // global video state

byte vid_buffer[ BASEWIDTH * BASEHEIGHT ] __attribute__((aligned(4)));   // 75 K x 2
byte d_buffer[ BASEWIDTH * BASEHEIGHT ] __attribute__((aligned(4))); 

__RAM_1 static short zbuffer[ BASEWIDTH * BASEHEIGHT ]; // 150 K

unsigned short d_8to16table[ 256 ]; // 512 B
// unsigned d_8to24table[256];

// PSRAM is waaay too slow for this..
// Min is 256k
__RAM_1 byte surfcache[ 512 * 1024 ];

static int dma_chan = -1;

void VID_SetPalette(unsigned char *palette)
{
    for (uint32_t i = 0; i < 256; i++)
	{
    	uint8_t r = (*palette++ >>  3) & 31;
    	uint8_t g = (*palette++ >>  2) & 63;
    	uint8_t b = (*palette++ >>  3) & 31;
    	d_8to16table[ i ] = r << 11 | g << 5 | b;
    }
}

void VID_Init(unsigned char *palette)
{
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;
	
	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));

	VID_SetPalette( palette );

    dma_chan = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(
        dma_chan,      // Channel to be configured
        &c,            // The configuration we just created
        d_buffer,      // The initial write address
        vid_buffer,    // The initial read address
        0,             // Number of transfers
        false          // Start immediately.
    );
}

void VID_Update(vrect_t *rects)
{
	dma_channel_hw_addr(dma_chan)->write_addr = (uintptr_t) d_buffer;
	dma_channel_transfer_from_buffer_now(dma_chan, vid_buffer, sizeof(vid_buffer) / 4);
	// dma_channel_wait_for_finish_blocking( dma_chan );
}

void VID_ShiftPalette(unsigned char *palette)
{
	VID_SetPalette( palette );
}

void VID_Shutdown(void)
{
	if ( dma_chan >= 0 )
		dma_channel_unclaim( dma_chan );
}

void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height) {}
void D_EndDirectRect (int x, int y, int width, int height) {}
