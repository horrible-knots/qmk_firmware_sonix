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

#define GAMER_MODE_LED_BIT 31

int gamer_mode_led_state = !LED_PIN_ON_STATE;

enum ARM_MODE {
    ARM_MODE_NOSEL,
    ARM_MODE_PLY,
    ARM_MODE_REC
};

typedef struct {
    bool ready;
    bool armed;
    bool playing;
    bool recording;
    bool play_start;
    bool rec_start;
    bool led_needs_update;
    bool warn_blink;
    RGB warn_color_rgb;
    enum ARM_MODE arm_mode;
    uint8_t offset_ms;
    uint8_t last_key;
} rec_state_t;

typedef struct {
    uint8_t key;
    uint8_t delay_ms;
    uint8_t length_ms;
} note_t;

uint8_t debug_light;
rec_state_t rec_state;
note_t notes[700];
uint8_t rec_dermafrazit_last_n;
uint16_t notes_max = sizeof(notes) / sizeof(note_t);

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
void rec_state_blink_led_rgb_all(uint8_t *iterations, int r, int g, int b);
void rec_state_standby_led(uint16_t keycode);
uint8_t keycode_to_index(uint16_t keycode);
bool rec_active(void);
void rec_state_standby_led_rgb(uint16_t keycode, uint8_t r, uint8_t g, uint8_t b);
void rec_reset(void);
void rec_dermafrazit(uint32_t index, uint32_t max, bool mode);
void rec_set_warn_color(RGB rgb);
void rec_blank_keyboard(void);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
       dprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", 
           keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif

    // When recording/playing, only these keys will be active
    if (rec_state.ready && (rec_state.armed || rec_active())) {
        switch (keycode) {
            case KC_Z:
            case KC_X:
	       	if (rec_active()) {
		    switch (keycode) {
                        case KC_Z:
		            rec_state.last_key = record->event.pressed ? KC_Z : 254;
			    return true; // these have to get processed
			case KC_X:
			    rec_state.last_key = record->event.pressed ? KC_X : 254;
			    return true;
                    }
		} else if (rec_state.armed) {
		    switch (rec_state.arm_mode) {
			case ARM_MODE_PLY:
			    rec_state.play_start = true;
			    rec_state.armed = false;
			    break;
			case ARM_MODE_REC:
			    rec_state.rec_start = true;
			    rec_state.armed = false;
			    break;
			case ARM_MODE_NOSEL:
			    // Not possible
			    break;
		    }
		    return false;
		}
	    case TG(_REC):
		return true;
	    case REC_STOP:
		// catch and release to the next switch statement
		break;
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
	    break;
	case KC_LWIN:
	    return !(gamer_mode_led_state == LED_PIN_ON_STATE);
	case KC_APP:
	    return !(gamer_mode_led_state == LED_PIN_ON_STATE);
	case REC_ARM:
	    if (record->event.pressed) {
		if (rec_state.arm_mode == ARM_MODE_NOSEL) {
                    rec_set_warn_color((RGB){ 255, 0, 0 });
		    rec_state.warn_blink = 1;
		    return false;
		} else {
    		    rec_state.armed = true;
		    rec_state.led_needs_update = true;
		} 
	    }
	    break;
	case REC_REC:
	    if (record->event.pressed) {
		rec_state.arm_mode = ARM_MODE_REC;
		rec_state.armed = false;
		rec_state.led_needs_update = true;
		return false;
	    }
	    break;
	case REC_PLY:
	    if (record->event.pressed) {
		rec_state.arm_mode = ARM_MODE_PLY;
		rec_state.armed = false;
		rec_state.led_needs_update = true;
		return false;
	    }
	    break;
	case REC_CLR:
	    if (record->event.pressed && !rec_active()) {
	        rec_reset();
		memset(&notes, 0, sizeof(notes));
	        return false;
	    }
	    break;
	case REC_STOP:
	    if (record->event.pressed) {
		rec_reset();
	    }
	    return false;
	case REC_ADV:
	    if (rec_state.playing && record->event.pressed) {
		rec_state.offset_ms += 5;
	    }
	    return false;
	case REC_RET:
	    if (rec_state.playing && record->event.pressed) {
		rec_state.offset_ms -= 5;
	    }
	    return false;
    }

    return true;
}

void rec_reset(void) {
    rec_state.ready = true;
    rec_state.armed = false;
    rec_state.rec_start = false;
    rec_state.play_start = false;
    rec_state.playing = false;
    rec_state.recording = false;
    rec_state.led_needs_update = true;
    rec_state.arm_mode = ARM_MODE_NOSEL;
    rec_state.last_key = 0;
    rec_dermafrazit_last_n = 0;
    clear_keyboard();
}

static uint8_t rec_rgb_matrix_last_mode;
layer_state_t layer_state_set_kb(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _BASE:
	    rec_state.ready = false;

	    rec_blank_keyboard();
	    rgb_matrix_enable();
	    rgb_matrix_mode_noeeprom(rec_rgb_matrix_last_mode);
            break;
        case _REC:
	    rec_rgb_matrix_last_mode = rgb_matrix_get_mode();
	    rgb_matrix_disable();

	    rec_reset();
            break;
    }
    return (state);
}

bool rec_active() {
	if (rec_state.recording || rec_state.playing)
		return true;
	else
		return false;
}

static uint8_t rec_blink_led_iterations_remaining;
static enum ARM_MODE last_arm_mode = ARM_MODE_NOSEL;
void rec_state_leds() {
    if (rec_state.ready) {
        rec_state_standby_led_rgb(KC_ESC, 0, 255, 0);
    } else {
        rec_state_standby_led_rgb(KC_ESC, 255, 0, 0);
        return;
    }

    if (rec_state.led_needs_update) {
	rec_blank_keyboard();
    }

    if (rec_state.warn_blink) {
	if (rec_blink_led_iterations_remaining == 0) {
		rec_blink_led_iterations_remaining = 10;
	} 

	rec_state_blink_led_rgb_all(&rec_blink_led_iterations_remaining, rec_state.warn_color_rgb.r, rec_state.warn_color_rgb.g, rec_state.warn_color_rgb.b);
	if (rec_blink_led_iterations_remaining == 0) {
	    rec_state.warn_blink = false;
	    rec_state.led_needs_update = true;
	    rec_state_leds();
	}

	return;
    }

    if (rec_state.ready && rec_state.led_needs_update) {
        rec_blank_keyboard();
        rec_state_standby_led(REC_ADV);
        rec_state_standby_led(REC_RET);
        rec_state_standby_led(REC_CLR);
        rec_state_standby_led(REC_ARM);
        rec_state_standby_led(REC_STOP);
        rec_state_standby_led(TG(_REC));
	rec_state_standby_led_rgb(REC_REC, 255, 0, 0);
	rec_state_standby_led_rgb(REC_PLY, 0, 255, 0);
    }

    if (rec_state.armed && rec_state.led_needs_update) {
        rec_state_standby_led_rgb(KC_Z, 0, 0, 255);
        rec_state_standby_led_rgb(KC_X, 0, 0, 255);
    } else if (rec_state.armed) {
        rec_state_blink_led(REC_ARM);
    }

    if (rec_active()) {
	rec_state_standby_led_rgb(REC_ARM, 128, 128, 128);
    }

    switch (rec_state.arm_mode) {
	case ARM_MODE_REC:
            rec_state_blink_led(REC_REC);
	    break;
	case ARM_MODE_PLY:
	    rec_state_blink_led(REC_PLY);
	    break;
	case ARM_MODE_NOSEL:
	    rec_state_standby_led_rgb(REC_REC, 255, 0, 0);
	    rec_state_standby_led_rgb(REC_PLY, 0, 255, 0);
	    break;
    }

    rec_state.led_needs_update = false;
    last_arm_mode = rec_state.arm_mode;
}

void rec_blank_keyboard(void) {
    rgb_matrix_set_color_all(0, 0, 0);
}

void rec_set_warn_color(RGB rgb) {
    rec_state.warn_color_rgb.r = rgb.r;
    rec_state.warn_color_rgb.g = rgb.g;
    rec_state.warn_color_rgb.b = rgb.b;
}

void rec_state_standby_led(uint16_t keycode) {
    rgb_matrix_set_color(keycode_to_index(keycode), 0, 255, 0);
}
void rec_state_standby_led_rgb(uint16_t keycode, uint8_t r, uint8_t g, uint8_t b) {
	rgb_matrix_set_color(keycode_to_index(keycode), r, g, b);
}
void rec_state_blink_led(uint16_t keycode) {
    return rec_state_blink_led_rgb(keycode, 255, 255, 255);
}

uint8_t rec_state_led_blink_state;
uint16_t rec_state_led_timer;
void rec_state_blink_led_rgb(uint16_t keycode, int r, int g, int b) {
    if (timer_elapsed(rec_state_led_timer) > 250) {
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

static uint8_t rec_state_led_blink_state_all;
static uint16_t rec_state_led_timer_all;
void rec_state_blink_led_rgb_all(uint8_t *iterations, int r, int g, int b) {
    if (timer_elapsed(rec_state_led_timer_all) > 100) {
        switch (rec_state_led_blink_state_all) {
            case 0:
                rec_blank_keyboard();
                break;
            case 1:
                rgb_matrix_set_color_all(r, g, b);
                break;
        }
	(*iterations)--;

	if (!*iterations) {
	    rec_blank_keyboard();
	    rec_state.led_needs_update = true;
	    rec_state_led_blink_state_all = 1;
	    return;
	}

        rec_state_led_timer_all = timer_read();
        rec_state_led_blink_state_all ^= 1;
    } 
} 

static uint16_t rec_mode_active_led_blink_timer;
void matrix_scan_user() {
    if (layer_state_is(_REC) && timer_elapsed(rec_mode_active_led_blink_timer) > 500) {
        writePin(LED_GAMER_MODE_PIN, gamer_mode_led_state ^= 1);
	rec_mode_active_led_blink_timer = timer_read();
    }

    rec_state_leds();

    if (rec_state.warn_blink)
	return;

    if (rec_state.last_key > 0) {
	if (rec_active() || rec_state.rec_start || rec_state.play_start) {
	    if (rec_state.arm_mode == ARM_MODE_PLY) {
		rec_play();
	    } else if (rec_state.arm_mode == ARM_MODE_REC) {
		rec_record();
	    }
	}
    }
}

uint16_t rec_record_current_note;
uint16_t rec_record_timer;

// last_key is z or x, 254 to not record another note until changed again, and 255 is for key release
void rec_record() {
    if (rec_state.rec_start && !rec_state.recording) {
        memset(&notes, 0, sizeof(notes));
	rec_record_current_note = 0;
        rec_state.rec_start = false;
        rec_state.recording = true;
    } else if (rec_state.recording) {
	rec_dermafrazit(rec_record_current_note, notes_max, false);

        if (rec_state.last_key < 254) /* key pressed */ {
	    // We can't store this.  Maybe I can reduce the accuracy and double the time
	    if (timer_elapsed(rec_record_timer) > 255) {
                    rec_set_warn_color((RGB){ 0, 255, 255 });
		    rec_state.warn_blink = true;
		    rec_reset();
		    return;
	    }
	    // Difference between rec_record_timer and now will be time since last release
            notes[rec_record_current_note].key = rec_state.last_key; 
	    if (rec_record_current_note > 0) {
		notes[rec_record_current_note-1].delay_ms = timer_elapsed(rec_record_timer);
	    } else {
		notes[rec_record_current_note].delay_ms = timer_elapsed(rec_record_timer);
	    }
	    // Set up for key release
            rec_record_timer = timer_read();
	} else if (rec_state.last_key == 254)  { /* key released */ {
		notes[rec_record_current_note].length_ms = timer_elapsed(rec_record_timer);
		// Set up timer for next delay calculation
		rec_record_timer = timer_read();
		rec_record_current_note++;
		rec_state.last_key = 255;
	    }

	    if (rec_record_current_note >= notes_max) {
		rec_state.recording = false;
		rec_set_warn_color((RGB){ 64, 96, 128});
		rec_state.warn_blink = true;
		rec_reset();
	    }
	}
    }
}

uint16_t rec_play_timer;
uint16_t rec_play_current_note;
enum rec_play_state_t {
    PLAY_START,
    PLAY_LENGTH,
    PLAY_DELAY
};
enum rec_play_state_t rec_play_state;
void rec_play() {
    if (rec_state.play_start && !rec_state.playing) {
	rec_state.playing = true;
	rec_state.play_start = false;
        rec_play_current_note = 0;
	rec_play_state = PLAY_START;
    } else if (rec_state.playing == true) {
	rec_dermafrazit(rec_play_current_note, notes_max, false);

        if (rec_play_current_note >= rec_record_current_note) {
	    rec_set_warn_color((RGB){ 255, 255, 255 });
	    rec_state.warn_blink = true;
	    rec_reset();
	    return;
	}

        // There's no delay for starting the first note (the key press starts the play)
        switch (rec_play_state) {
	    case PLAY_START:
                register_code(notes[rec_play_current_note].key);
	    	rec_play_timer = timer_read();
	    	rec_play_state = PLAY_LENGTH;
		break;
	    case PLAY_LENGTH:
		if (timer_elapsed(rec_play_timer) >= notes[rec_play_current_note].length_ms) {
		    unregister_code(notes[rec_play_current_note].key);
	    	    rec_play_timer = timer_read();
		    rec_play_state = PLAY_DELAY;
		}
		break;
	    case PLAY_DELAY:
		if (timer_elapsed(rec_play_timer) >= notes[rec_play_current_note].delay_ms) {
               	    rec_play_timer = timer_read();
		    rec_play_state = PLAY_START;
		    rec_play_current_note++;
		}
	}
    }
}

void rec_dermafrazit(uint32_t index, uint32_t max, bool mode) {
    uint8_t light_n = ((index + 0) / notes_max) * 10;

    if (rec_dermafrazit_last_n == light_n) {
        return;
    } else {
        rec_dermafrazit_last_n = light_n;
    }

    for (uint8_t keycode = KC_1; keycode < KC_MINS; keycode++) {
	if (light_n >= keycode) {
            rgb_matrix_set_color(keycode_to_index(keycode), 255, 0, 0);
	} else {
	    rgb_matrix_set_color(keycode_to_index(keycode), 0, 255, 0);
	}
    }
}

void rgb_matrix_reload_from_eeprom() {
    eeprom_read_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX, sizeof(rgb_matrix_config));             
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
    return 255;
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
}

