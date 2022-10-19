#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "bootmagic.h"
#include "quantum.h"
#include "eeconfig.h"

#define BOOT0_GPIO SN_GPIO2
#define BOOT0_PIN 2
// Row
#define BLKEY_INPUT_GPIO SN_GPIO2
#define BLKEY_INPUT_PIN 15
// Col
#define BLKEY_OUTPUT_GPIO SN_GPIO0
#define BLKEY_OUTPUT_PIN 0


void bootmagic_lite_reset_eeprom(void) { 
    eeconfig_disable(); 
}

void bootmagic_lite(void) {
    int res;

    wait_ms(30);
    BLKEY_INPUT_GPIO->CFG &= ~(3 << (BLKEY_INPUT_PIN * 2));
    BLKEY_OUTPUT_GPIO->MODE |= (1 << BLKEY_OUTPUT_PIN);
    wait_ms(30);    
    BLKEY_OUTPUT_GPIO->BCLR = (1 << BLKEY_OUTPUT_PIN);
    wait_ms(30);
    res = BLKEY_INPUT_GPIO->DATA & (1 << BLKEY_INPUT_PIN);

    if (!res) {
        bootmagic_lite_reset_eeprom();

        // Jump to bootloader.
        bootloader_jump();
    }
}


