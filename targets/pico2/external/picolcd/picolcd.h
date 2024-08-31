#ifndef __PCIOLCD_H__
#define __PICOLCD_H__

#include "picolcd_config.h"

typedef enum {
    typeST     = 0,
    typeILI,
    typeUnimplemented
} lcdType_t;

typedef enum {
    rendDefault    = 0, // 16-bit native
    rendLut8,           // Do it line-by-line and use a 256-entry LUT to determine colour
    renderUnimplemented
} renderMode_t;

typedef struct {
    lcdType_t type;
    renderMode_t mode;
    const void *buf;
    // These two are only required if you use anything else than mode "rendDefault"
    const uint16_t *lut;
    uint16_t *lin;  // Line buffer. Double-buffering is utilised so it needs to be "LCD_WIDTH" times two
} lcd_config_t;

void initPicoLCD(const lcd_config_t *config);

void __attribute__((weak)) *picoLCD_drawLine(uint32_t line);
void __attribute__((weak)) picoLCD_onFrame(void);

#endif
