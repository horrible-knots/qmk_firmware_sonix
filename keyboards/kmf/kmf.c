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
uint8_t hybrid_hid_mode = _HID_ORGB;
#endif
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
//       dprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", 
 //          keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif
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
#ifdef VIA_OPENRGB_HYBRID
        case HID_ORGB:
	    writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state ^= 1);
            if (record->event.pressed) {
                hybrid_hid_mode = _HID_ORGB;
                return false;
            }
	    break;
        case HID_VIA:
	    writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state ^= 1);
            if (record->event.pressed) {
                hybrid_hid_mode = _HID_VIA;
                return false;
            }
	    break;
        case HID_RAW:
	    writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state ^= 1);
            if (record->event.pressed) {
                hybrid_hid_mode = _HID_RAW;
                return false;
            }
	    break;
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

