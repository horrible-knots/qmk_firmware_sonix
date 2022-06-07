#include <stdbool.h>
#include <stdint.h>
#include "keymap.h"
#include "keyboard.h"
#include "command_extra.h"
#include "openrgb_eeprom.h"
#include "command.h"

bool command_extra(uint8_t code) {
#if OPENRGB_ENABLE
    switch (code) {
        case MAGIC_KC(OPENRGB_EEPROM_SAVE):
            openrgb_save_rgb_state_eeprom();
            break;
        case MAGIC_KC(OPENRGB_EEPROM_LOAD):
            openrgb_load_rgb_state_eeprom();
            break;
        default:
	    return false;
    }
    return true;
#endif
}
