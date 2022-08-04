// Copyright 2022 nobody (@nobody)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "eeconfig.h"
#include "timer.h"
#include "kmf.h"
#include "config.h"
#include "keymaps/default/keymap.h"
#include "config_led.h"
#include "openrgb_eeprom.h"
#include <stdint.h>

#define GAMER_MODE_LED_BIT 31

int gamer_mode_led_state = !LED_PIN_ON_STATE;

bool process_record_user(uint16_t keycode, keyrecord_t *record);
void keyboard_post_init_user(void);
void gamer_mode_state_to_eeprom(int state);
extern void matrix_init(void);
void rec_play(void);
void rec_record(void);
void rgb_matrix_reload_from_eeprom(void);
void rec_state_leds(void);
void rec_state_blink_led(uint16_t keycode);
void rec_state_blink_led_rgb(uint16_t keycode, int r, int g, int b);
void rec_state_standby_led(uint16_t keycode);
uint8_t keycode_to_index(uint16_t keycode);

typedef struct {
    bool ready;
    bool armed;
    bool playing;
    bool recording;
    bool play_possible;
    bool rec_possible;
    bool led_needs_update;
} rec_state_t;

typedef struct {
    uint16_t delay;
    uint16_t length;
    uint8_t key;
} note_t;

note_t *note;
rec_state_t rec_state;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
       uprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", 
           keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif
    static int current_layer;

    if (rec_state.armed) {
	switch (keycode) {
	    case KC_Z:
	    case KC_X:
		rec_play();
		break;
	    default:
	        return false;
	}
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
	case KC_LWIN:
	    return !(gamer_mode_led_state == LED_PIN_ON_STATE);
	case KC_APP:
	    return !(gamer_mode_led_state == LED_PIN_ON_STATE);
	case REC_ARM:
	    rec_state.armed = 1;
	    return false;
	case TO(_REC):
	    current_layer = get_highest_layer(layer_state);

	    // Jumping out of this layer
	    if (current_layer == _REC) {
		rgb_matrix_reload_from_eeprom();
		rec_state.ready = 0;
	    } else {
		rgb_matrix_set_color_all(0, 0, 0);
		rec_state.ready = 1;
		rec_state.rec_possible = 1;
		rec_state.play_possible = 1;
		rec_state.led_needs_update = true;
	    } 

	    return false;
	case REC_STOP:
	    rec_state.playing = 0;
	    rec_state.recording = 0;
	    rec_state.led_needs_update = true;
	    return false;
             
    }
    return true;
}

int rec_state_led_blink_state = 1;
int rec_state_led_timer = 0;
void rec_state_leds() {
    if (!rec_state.ready)
	return;

    if (rec_state.armed) {
	rec_state_blink_led(KC_A);
	if (rec_state.led_needs_update) {
	    rec_state_standby_led(KC_Z);
	    rec_state_standby_led(KC_X);
	}
	return;
    }
    if (rec_state.ready && rec_state.led_needs_update) {
	rec_state_standby_led(KC_R);
	rec_state_standby_led(KC_A);
	rec_state_standby_led(KC_S);
	rec_state_standby_led(KC_LEFT);
	rec_state_standby_led(KC_RIGHT);
	rec_state_standby_led(KC_Q);
    }
}

void rec_state_standby_led(uint16_t keycode) {
    rgb_matrix_set_color(keycode_to_index(keycode), 0, 255, 0);
}
void rec_state_blink_led(uint16_t keycode) {
    return rec_state_blink_led_rgb(keycode, 255, 255, 255);
}
void rec_state_blink_led_rgb(uint16_t keycode, int r, int g, int b) {
    if (timer_elapsed(rec_state_led_timer) > 300) {
        switch (rec_state_led_blink_state) {
            case 0:
	        rgb_matrix_set_color(keycode_to_index(keycode), 0, 0, 0);
		break;
	    case 1:
	        rgb_matrix_set_color(keycode_to_index(keycode), r, g, b);
		break;
	}
	rec_state_led_timer = timer_read();
	rec_state_led_blink_state ^= 1;
    } 
} 

void matrix_scan_user() {
    rec_state_leds();

}

void rec_play() {

}

void rgb_matrix_reload_from_eeprom() {
    rgb_matrix_config.raw = pgm_read_byte(EECONFIG_RGB_MATRIX) << 0;
    rgb_matrix_config.raw = pgm_read_byte(EECONFIG_RGB_MATRIX+1) << 8;
    rgb_matrix_config.raw = pgm_read_byte(EECONFIG_RGB_MATRIX+2) << 16;
    rgb_matrix_config.raw = pgm_read_byte(EECONFIG_RGB_MATRIX+3) << 24;
    rgb_matrix_mode_noeeprom(rgb_matrix_config.mode);
}

uint8_t keycode_to_index(uint16_t keycode) {
    int layer = get_highest_layer(layer_state);

    for (int row = 0; row < MATRIX_ROWS; row++) {
	for (int col = 0; col < MATRIX_COLS; col++) {
	    uint8_t index = g_led_config.matrix_co[row][col];

            if (keymap_key_to_keycode(layer, (keypos_t){col,row}) == keycode) {
		return index;
	    }
	}
    }
    return -1;
}

void gamer_mode_state_to_eeprom(int state) {
    uint32_t eedata = eeconfig_read_user();
    eedata &= ~(1 << GAMER_MODE_LED_BIT);
    eedata |= (state << GAMER_MODE_LED_BIT);
    eeconfig_update_user(eedata);
}

void keyboard_post_init_user(void) {
    setPinOutput(LED_GAMER_MODE_PIN);
    uint32_t eedata = eeconfig_read_user();
    gamer_mode_led_state = ((eedata >> GAMER_MODE_LED_BIT) & 1);
    writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state);    
#ifdef OPENRGB_ENABLE
    openrgb_load_rgb_state_eeprom(true);
#endif
}

