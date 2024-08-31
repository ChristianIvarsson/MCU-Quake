#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/multicore.h>

#include "sys/audio.h"
#include "sys/video.h"
#include "sys/sys.h"

#include "external/sfe_psram.h"

#include "ff.h"

// This is for the Pimoroni Pico Plus 2
// QMI CS1n
#define PSRAM_PIN    47

#define CPUFREQ ( 150000000UL )


// Declared in our custom pico2.ld linker script
extern const uint8_t *const __psram_end__;
extern const uint8_t *const __psram_start__;
extern const uint8_t *const __psram_used__;
//    "-basedir",
//    "id1",

static const char *args[] = {
    "quake",
};

typedef struct {
    void *startFrom;
    size_t numBytes;
} psram_t;

static void initPSRAM(psram_t *const prm)
{
    printf("Init PSRAM..\n");

    size_t psram_size = sfe_setup_psram( PSRAM_PIN );

    printf("PSRAM setup complete. PSRAM size 0x%X\n", psram_size);

    // SDK is currently, afaik (famous last words), not capable of copying .data sections to psram during init so we ..
    // A - Have to handle things manually and ..
    // B - Can only easily use it for buffers and .bss
    // - We know for a fact that everything in .psram is .bss so we just do a memset of the lot -
    memset((void*)&__psram_start__, 0, (size_t)&__psram_used__);

    // Base of PSRAM and first address of the remaining free space in PSRAM
/*  printf("psram_base: %08x\n", (uint32_t)&__psram_start__);
    printf("psram_buff: %08x\n", (uint32_t)&__psram_end__);
    printf("psram_size: %u\n", psram_size);
    printf("psram_used: %u\n", (uint32_t)&__psram_used__);
    printf("psram_left: %u\n", psram_size - ((size_t)&__psram_used__)); */

    // __psram_end__ points at the first free address after compiler-placed data
    prm->startFrom = (void*)&__psram_end__;

    if ( psram_size >= (size_t)&__psram_used__ )
        prm->numBytes = psram_size - ((size_t)&__psram_used__);
    else
        prm->numBytes = 0;

/*
    uint32_t *bt;
    uint32_t ln = psramdata->numBytes;
    for ( uint32_t b = 0; b < 32; b++ ) {
        printf("Checking pat %02x\r\n", b);

        bt = psramdata->startFrom;
        for ( uint32_t i = 0; i < (ln / 4); i++ ) {
            *bt++ = (1u << b);
        }

        bt = psramdata->startFrom;
        for ( uint32_t i = 0; i < (ln / 4); i++ ) {
            if ( *bt != (1u << b) )
                panic("Memcheck failed\r\n");
            bt++;
        }
    }
    printf("PSRAM verification passed\r\n");
*/
}

static void secondCore(void)
{
    time_t ti;

    // Set systick to processor clock and configure reload.
    // This timer is used for profiling, measurements of time taken to do stuff etc
    systick_hw->csr = 0x5;
    systick_hw->rvr = 0xffffff;
    srand((unsigned) time(&ti));

    initVideo();
    // initAudio();

    printf("Subsystems initialised\r\n");
    while ( 1 ) {}
}

int main()
{
    time_t ti;
    psram_t psramdata;

    set_sys_clock_khz(CPUFREQ / 1000, true);
    stdio_init_all();

    printf("Hello world!\n");

    // Set systick to processor clock and configure reload.
    // This timer is used for profiling, measurements of time taken to do stuff etc
    systick_hw->csr = 0x5;
    systick_hw->rvr = 0xffffff;
    srand((unsigned) time(&ti));

    initPSRAM( &psramdata );

    // Prevent mayhem after flashing
    multicore_reset_core1();

    // rp2350 will crash after flashing if not given enough time to do.. something?
    sleep_ms(150);
    multicore_launch_core1(secondCore);

    quakeLoop(
        psramdata.startFrom, (uint32_t)psramdata.numBytes,
        // Pass number of / arguments
        sizeof(args) / sizeof(args[0]), args
    );

    while( 1 ) {}

    return 0;
}
