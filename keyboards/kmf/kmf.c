// Copyright 2022 nobody (@nobody)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "eeconfig.h"
#include "eeprom.h"
#include "timer.h"
#include "kmf.h"
#include "config.h"
#include "keymaps/default/keymap.h"
#include "config_led.h"
#include "openrgb_eeprom.h"
#include <stdint.h>
#include <stdio.h>
#include "raw_hid.h"
#include <stdarg.h>

#define GAMER_MODE_LED_BIT 31

int gamer_mode_led_state = !LED_PIN_ON_STATE;

void gamer_mode_state_to_eeprom(int state);
#ifdef VIA_OPENRGB_HYBRID
uint8_t is_orgb_mode = 0;
#endif
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
//       dprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", 
 //          keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif

    switch (get_highest_layer(layer_state)) {
	static uint16_t realcode;
	static bool shift_registered;
	case _SHIFT:
	    realcode = keymap_key_to_keycode(_SHIFT, record->event.key);
	    if (record->event.pressed) {
		if (realcode == KC_TRNS) {
		    shift_registered = true;
		    add_mods(MOD_MASK_SHIFT);
		    register_code(keycode);
		    return false;
		} 
	    } else {
		if (realcode == KC_TRNS && shift_registered) {
		    shift_registered = false;
		    unregister_code(keycode);
		    del_mods(MOD_MASK_SHIFT);
		    return true;
		}
	    }
            break;
    }

    switch (keycode) {
        case GAMER_MODE:
            // Don't handle the Windows key if Function-Windows has been pressed
            if (record->event.pressed) {
                gamer_mode_led_state ^= 1;
                writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state);
                gamer_mode_state_to_eeprom(gamer_mode_led_state);
                return false;
            }
            break;
        case KC_LWIN:
            return !(gamer_mode_led_state == LED_PIN_ON_STATE);
        case KC_APP:
            return !(gamer_mode_led_state == LED_PIN_ON_STATE);
        }
#ifdef VIA_OPENRGB_HYBRID
        case ORGB_TOG:
            is_orgb_mode = !is_orgb_mode;
	    return false;
#endif
    }
    return true;
}

void gamer_mode_state_to_eeprom(int state) {
    uint32_t eedata = eeconfig_read_user();
    eedata &= ~(1 << GAMER_MODE_LED_BIT);
    eedata |= (state << GAMER_MODE_LED_BIT);
    eeconfig_update_user(eedata);
}

void keyboard_pre_init_user(void) {
    debug_config.enable = 1;
}

void keyboard_post_init_user(void) {
    setPinOutput(LED_GAMER_MODE_PIN);
    uint32_t eedata = eeconfig_read_user();
    gamer_mode_led_state = ((eedata >> GAMER_MODE_LED_BIT) & 1);
    writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state);    
#ifdef OPENRGB_ENABLE
    openrgb_load_rgb_state_eeprom(true);
#endif
//    SN_SYS0->SWDCTRL_b.SWDDIS = 0; // You can enable SWD after boot with this.
}

