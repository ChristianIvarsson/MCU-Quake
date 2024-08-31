#ifndef __PICOLCD_CONFIG_H__
#define __PICOLCD_CONFIG_H__

#include "../sys/video.h"

// Using pin numbers above 31 on pico2 necessitates offsetting pin base by 16 pins (done automatically)
// It does, however, have the nasty side-effect that ALL state machines in that PIO instance are affected by that offset so care must be taken when selecting which PIO to use
#define LCD_PIO        (pio0)



// FREQ = (FCPU / PIODIV) / 2
#define LCD_PIODIV     (1)

// Display resolution
#define LCD_WIDTH      (BASEWIDTH)
#define LCD_HEIGHT     (BASEHEIGHT)

// Pin definitions
#define LCD_CS         (33)
#define LCD_DC         (32)

#define LCD_CLK        (34)
#define LCD_DAT        (35)

#endif
