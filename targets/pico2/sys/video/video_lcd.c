#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pico/stdlib.h>

#include "../video.h"
#include "../../external/picolcd/picolcd.h"




uint16_t lcdLin[ LCD_WIDTH * 2 ];

void initVideo(void)
{
    lcd_config_t cfg;

    for (uint32_t i = 0; i < 256; i++)
        d_8to16table[ i ] = rand();

    cfg.type = typeILI;
    cfg.mode = rendLut8;
    cfg.buf = d_buffer;
    cfg.lut = d_8to16table;
    cfg.lin = lcdLin;

    initPicoLCD( &cfg );
}
