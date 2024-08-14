#include <stdio.h>
#include <stdint.h>

#include "sys/audio.h"
#include "sys/video.h"
#include "sys/sys.h"


extern const uint8_t *__psram_end__;
extern const uint8_t *__psram_left__;

int main()
{
    initVideo();
    initAudio();



    // Figure out what is left after the compiler has used what it wants and where to start
    quakeLoop((void*)__psram_end__, (uint32_t)__psram_left__);

    while( 1 ) {}

    return 0;
}