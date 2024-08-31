#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pico/stdlib.h>
#include <hardware/dma.h>
#include <hardware/pio.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>

#include "picolcd.pio.h"
#include "picolcd.h"
#include "picolcd_initdata.h"

// Thanks rp2350...
#if (LCD_CS > 31)
  #define CLEAR_CS   sio_hw->gpio_hi_clr = (uint32_t)(1u << (LCD_CS - 32))
  #define SET_CS     sio_hw->gpio_hi_set = (uint32_t)(1u << (LCD_CS - 32))
#else
  #define CLEAR_CS   sio_hw->gpio_clr = (uint32_t)(1u << LCD_CS)
  #define SET_CS     sio_hw->gpio_set = (uint32_t)(1u << LCD_CS)
#endif

#if (LCD_DC > 31)
  #define CLEAR_DC   sio_hw->gpio_hi_clr = (uint32_t)(1u << (LCD_DC - 32))
  #define SET_DC     sio_hw->gpio_hi_set = (uint32_t)(1u << (LCD_DC - 32))
#else
  #define CLEAR_DC   sio_hw->gpio_clr = (uint32_t)(1u << LCD_DC)
  #define SET_DC     sio_hw->gpio_set = (uint32_t)(1u << LCD_DC)
#endif

#if (LCD_CLK > 31)
  #define CLEAR_CLK   sio_hw->gpio_hi_clr = (uint32_t)(1u << (LCD_CLK - 32))
  #define SET_CLK     sio_hw->gpio_hi_set = (uint32_t)(1u << (LCD_CLK - 32))
#else
  #define CLEAR_CLK   sio_hw->gpio_clr = (uint32_t)(1u << LCD_CLK)
  #define SET_CLK     sio_hw->gpio_set = (uint32_t)(1u << LCD_CLK)
#endif

#if (LCD_DAT > 31)
  #define CLEAR_DAT   sio_hw->gpio_hi_clr = (uint32_t)(1u << (LCD_DAT - 32))
  #define SET_DAT     sio_hw->gpio_hi_set = (uint32_t)(1u << (LCD_DAT - 32))
#else
  #define CLEAR_DAT   sio_hw->gpio_clr = (uint32_t)(1u << LCD_DAT)
  #define SET_DAT     sio_hw->gpio_set = (uint32_t)(1u << LCD_DAT)
#endif


// Using pin numbers above 31 necessitates offsetting PIO base by 16 pins, thus making pins 0-15 unreachable by that PIO instance
#if (LCD_CLK > 31) || (LCD_DAT > 31)
  #define LCD_PIN_BASE   16
  #if (LCD_CLK < 16) || (LCD_DAT < 16)
    #error "LCD_CLK and LCD_DAT pin numbers are out of range"
  #endif
#else
  #define LCD_PIN_BASE   0
#endif


// These are automatically configured at setup so there's no point in changing them
static int lcd_dma_chan = 0;
static int pio_sm = 0;

static const void *lcdBuffer = 0;
static const uint16_t *lcdLut = 0;
static uint16_t *lcdLin = 0;


// You can override these with your own implementations
void __attribute__((weak)) *__not_in_flash_func(picoLCD_drawLine)(uint32_t line)
{
    const uint8_t *rdLine = &((uint8_t*)lcdBuffer)[ (line * LCD_WIDTH) ];
    uint16_t *wrLine = &lcdLin[ (line & 1) * LCD_WIDTH ];

    for ( uint32_t i = 0; i < LCD_WIDTH; i++ )
        wrLine[ i ] = lcdLut[ rdLine[ i ] ];

    return wrLine;
}

void __attribute__((weak)) __not_in_flash_func(picoLCD_onFrame)(void)
{}

static inline void waitIdle(void)
{
    uint32_t msk = 1u << (pio_sm + PIO_FDEBUG_TXSTALL_LSB);
    LCD_PIO->fdebug = msk;
    while (!(LCD_PIO->fdebug & msk))
        ;
}

static inline void putData(uint8_t x)
{
    while (pio_sm_is_tx_fifo_full(LCD_PIO, pio_sm))
        ;
    *(volatile uint8_t*)&LCD_PIO->txf[pio_sm] = x;
}

static inline void sndCommand(const uint8_t cmd)
{
    CLEAR_DC;
    CLEAR_CS;

    putData( cmd );

    waitIdle();

    SET_CS;
}

static void __isr __not_in_flash_func(dmaISR_16bit)(void)
{
    dma_hw->ints1 = (1u << lcd_dma_chan);

    picoLCD_onFrame();

    // Wait for SM completion
    waitIdle();

    // Deselect LCD
    SET_CS;

    // It seems the display just doesn't care if you send extra junk data so let's just send a 16-bit packet despite it only wanting an 8-bit one

    // Configure for 8-bit transfers
    // setShiftCount(8);

    // Start data transfer
    sndCommand( 0x2c );

    SET_DC;
    CLEAR_CS;

    dma_channel_hw_addr(lcd_dma_chan)->al3_read_addr_trig = (uintptr_t)lcdBuffer;
}

static void __isr __not_in_flash_func(dmaISR_lut8)(void)
{
    static uint32_t line = 1;
    static void *nextLine = 0;

    dma_hw->ints1 = (1u << lcd_dma_chan);

    // Last line has just been sent
    if ( line == 1 )
    {
        if ( nextLine == 0 )
        {
            nextLine = picoLCD_drawLine( 0 );
        }

        // Wait for SM completion
        waitIdle();

        // Deselect LCD
        SET_CS;

        // It seems the display just doesn't care if you send extra junk data so let's just send a 16-bit packet despite it only wanting an 8-bit one

        // Configure for 8-bit transfers
        // setShiftCount(8);

        // Start data transfer
        sndCommand( 0x2c );

        SET_DC;
        CLEAR_CS;

        dma_channel_hw_addr(lcd_dma_chan)->al3_read_addr_trig = (uintptr_t)nextLine;
        nextLine = picoLCD_drawLine( line++ );
        return;
    }

    dma_channel_hw_addr(lcd_dma_chan)->al3_read_addr_trig = (uintptr_t)nextLine;

/*
      0 -> send 239 and render 0
      1 -> send transfer cmd, send 0 and render 1
      2 -> send 1 and render 2
    . . .
    238 -> send 237 and render 238
    239 -> send 238 and render 239 -> step 0
*/
    nextLine = picoLCD_drawLine( line++ );

    if ( line >= LCD_HEIGHT )
        line = 0;
}

static void dma_init(renderMode_t mode)
{
    lcd_dma_chan = dma_claim_unused_channel( true );
    dma_channel_config channel_config = dma_channel_get_default_config( lcd_dma_chan );
    channel_config_set_dreq( &channel_config, pio_get_dreq(LCD_PIO, pio_sm, true) );
    channel_config_set_transfer_data_size( &channel_config, DMA_SIZE_16 );

    uint32_t transferCount = LCD_WIDTH;

    if ( mode == rendDefault )
    {
        transferCount *= LCD_HEIGHT;
    }

    dma_channel_configure(
        lcd_dma_chan,
        &channel_config,
        &LCD_PIO->txf[pio_sm],  // Target
        NULL,   // Source
        transferCount, // Transfer count
        false); // Don't start now

    if ( mode == rendDefault )
    {
        irq_set_exclusive_handler(DMA_IRQ_1, dmaISR_16bit);
    }
    else if ( mode == rendLut8 )
    {
        irq_set_exclusive_handler(DMA_IRQ_1, dmaISR_lut8);
    }

    dma_channel_set_irq1_enabled(lcd_dma_chan, true);
    irq_set_enabled(DMA_IRQ_1, true);
}

static void pio_init(renderMode_t mode)
{
    // This must be configured before adding the program
#if (LCD_PIN_BASE > 0)
    pio_set_gpio_base(LCD_PIO, LCD_PIN_BASE);
#endif

    uint prog_offs = pio_add_program(LCD_PIO, &lcd_spi_program);
    pio_sm_config c = lcd_spi_program_get_default_config(prog_offs);
    pio_sm = pio_claim_unused_sm(LCD_PIO, true);

    pio_gpio_init(LCD_PIO, LCD_CLK);
    pio_gpio_init(LCD_PIO, LCD_DAT);

    pio_sm_set_consecutive_pindirs(LCD_PIO, pio_sm, LCD_DAT, 1, true);
    pio_sm_set_consecutive_pindirs(LCD_PIO, pio_sm, LCD_CLK, 1, true);

    sm_config_set_sideset_pins(&c, LCD_CLK/* - LCD_PIN_BASE*/);
    sm_config_set_out_pins(&c, LCD_DAT/* - LCD_PIN_BASE*/, 1);

    sm_config_set_out_shift(
        &c,
        false, // Shift right
        true,  // autopull
        16);

    pio_sm_init(LCD_PIO, pio_sm, prog_offs, &c);
    pio_sm_set_clkdiv_int_frac(LCD_PIO, pio_sm, LCD_PIODIV, 0);
    pio_sm_set_enabled(LCD_PIO, pio_sm, true);
}

static inline void lcdBitbang(uint8_t dat)
{
    for (uint32_t i = 0; i < 8; i++)
    {
        if ( dat & 0x80 )
            SET_DAT;
        else
            CLEAR_DAT;

        SET_CLK;
        asm volatile("nop\n\tnop\n\tnop\n\tnop\n");
        dat <<= 1;
        CLEAR_CLK;
    }
}

static void bitbangCommand(const uint8_t cmd)
{
    CLEAR_DC;
    CLEAR_CS;

    lcdBitbang(cmd);

    SET_CS;
}

static void bitbangData(const uint8_t *data, uint32_t len)
{
    SET_DC;
    CLEAR_CS;

    while (len--)
    {
        lcdBitbang(*data++);
    }

    SET_CS;
}

static void pioLCD(const lcd_config_t *config)
{
    pio_init( config->mode );

    gpio_set_slew_rate(LCD_CS, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LCD_DC, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LCD_CLK, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(LCD_DAT, GPIO_SLEW_RATE_SLOW);

    gpio_set_drive_strength(LCD_CS, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(LCD_DC, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(LCD_CLK, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(LCD_DAT, GPIO_DRIVE_STRENGTH_2MA);

    dma_init( config->mode );

    // Kickstart DMA
    if ( config->mode == rendDefault )
    {
        dma_channel_hw_addr(lcd_dma_chan)->al3_read_addr_trig = (uintptr_t)lcdBuffer;
    }
    else if ( config->mode == rendLut8 )
    {
        dma_channel_hw_addr(lcd_dma_chan)->al3_read_addr_trig = (uintptr_t)lcdLin;
    }
}

static void initPins(void)
{
    // Can't use the regular masked init on rp2350 when dealing with pins numbered 32 or higher ..
    // and rp2040 doesn't support that call. So.. Well
    gpio_init(LCD_CS);
    gpio_init(LCD_DC);
    gpio_init(LCD_CLK);
    gpio_init(LCD_DAT);

#if (LCD_CS < 32) && (LCD_DC < 32) && (LCD_CLK < 32) && (LCD_DAT < 32)
    gpio_set_dir_out_masked(
        (1u << LCD_CS)  | (1u << LCD_DC)  | 
        (1u << LCD_CLK) | (1u << LCD_DAT) );
#else
    gpio_set_dir_out_masked64(
        (1ull << LCD_CS)  | (1ull << LCD_DC)  | 
        (1ull << LCD_CLK) | (1ull << LCD_DAT) );
#endif

    // De-select LCD
    SET_CS;
}

void initPicoLCD(const lcd_config_t *config)
{
    const lcd_init_cmd_t* lcd_init_cmds;
    int cmd = 0;

    // Needs error checking!
    lcdBuffer = config->buf;
    lcdLut = config->lut;
    lcdLin = config->lin;

    if ( config->mode < 0 || config->mode >= renderUnimplemented )
    {
        panic("picolcd: Unknown mode");
    }

    if ( config->type == typeST )
    {
        lcd_init_cmds = st_init_cmds;
    }
    else if ( config->type == typeILI )
    {
        lcd_init_cmds = ili_init_cmds;
    }
    else
    {
        panic("picolcd: Unknown lcd controller");
    }

    initPins();
    sleep_ms( 50 );

    // Send all the commands
    while ( lcd_init_cmds[cmd].databytes != 0xff )
    {
        bitbangCommand( lcd_init_cmds[cmd].cmd );
        bitbangData( lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes & 0x1F );
        
        if ( lcd_init_cmds[cmd].databytes & 0x80 )
        {
            sleep_us( 20 );
        }

        cmd++;
    }

    pioLCD( config );

    printf("picolcd init with resolution of %u x %u\r\n", LCD_WIDTH, LCD_HEIGHT);
}
