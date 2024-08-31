
/* hw_config.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
/*   
This file should be tailored to match the hardware design.

See 
https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/tree/main#customizing-for-the-hardware-configuration
*/

#include "hw_config.h"


/*
 5 - SD_CLK
18 - SD_CMD    (MOSI)
19 - SD_DAT0   (MISO)
20 - SD_DAT1
21 - SD_DAT2
22 - SD_DAT3   (CS)
*/

/* SDIO Interface */
static sd_sdio_if_t sdio_if = {
    .SDIO_PIO = pio1,
    .DMA_IRQ_num = DMA_IRQ_0,
    /*
    Pins CLK_gpio, D1_gpio, D2_gpio, and D3_gpio are at offsets from pin D0_gpio.
    The offsets are determined by sd_driver\SDIO\rp2040_sdio.pio.
        CLK_gpio = (D0_gpio + SDIO_CLK_PIN_D0_OFFSET) % 32;
        As of this writing, SDIO_CLK_PIN_D0_OFFSET is 30,
            which is -2 in mod32 arithmetic, so:
        CLK_gpio = D0_gpio -2.
        D1_gpio = D0_gpio + 1;
        D2_gpio = D0_gpio + 2;
        D3_gpio = D0_gpio + 3;
    */
    .CMD_gpio = 18,
    .D0_gpio = 19,
    .baud_rate = 1 * 1000 * 1000  // 15 MHz
};

/* Hardware Configuration of the SD Card socket "object" */
static sd_card_t sd_card = {
    .type = SD_IF_SDIO,
    .sdio_if_p = &sdio_if
};

/* Callbacks used by the library: */
size_t sd_get_num() { return 1; }

sd_card_t *sd_get_by_num(size_t num) {
    if (0 == num)
        return &sd_card;
    else
        return NULL;
}
