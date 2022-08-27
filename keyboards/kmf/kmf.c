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
    int8_t offset_ms;
    uint8_t last_key;
    uint8_t last_key_raw;
} rec_state_t;

typedef struct {
    uint8_t key;
    uint8_t delay_ms;
    uint8_t length_ms;
} note_t;

uint8_t debug_light;
rec_state_t rec_state;
#ifdef RAW_ENABLE
note_t notes[300];
#else
note_t notes[800];
#endif
uint8_t rec_dermafrazit_last_n;
uint16_t notes_max = sizeof(notes) / sizeof(note_t);
uint16_t rec_record_current_note;

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
void rec_set_last_key(uint16_t keycode, keyrecord_t *record);
uint8_t rec_normalize(uint32_t input);
uint32_t rec_unnormalize(uint8_t input);
void hid_printf(const char *fmt, ...);
void toggle_led(uint32_t pin, uint8_t count, uint8_t delay_ms);
int8_t get_random_ms(uint8_t max);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef CONSOLE_ENABLE
//       dprintf("KL: kc: 0x%04X, col: %u, row: %u, pressed: %b, time: %u, interrupt: %b, count: %u\n", 
 //          keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
#endif

    // When recording/playing, only these keys will be active
    if (rec_state.ready && (rec_state.armed || rec_active())) {
        switch (keycode) {
            case KC_Z:
            case KC_X:
                if (rec_active()) {
                    rec_set_last_key(keycode, record);
                    return true;
                } else if (rec_state.armed) {
                    switch (rec_state.arm_mode) {
                        case ARM_MODE_PLY:
                            rec_state.play_start = true;
                            rec_state.armed = false;
                            return false; // actually don't want this key sent
                        case ARM_MODE_REC:
                            rec_state.rec_start = true;
                            rec_state.armed = false;
                            rec_set_last_key(keycode, record); // Can't lose the first key when recording
                            return true; // actually want this key sent
                        case ARM_MODE_NOSEL:
                            // Not possible
                            break;
                    }
               }
               break;
            case TG(_REC):
                if (record->event.pressed) {
                    rec_reset();
                    return true;
                }
            case REC_STOP:
                // catch and release to the next switch statement
                break;
        }
    }

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
#ifdef RAW_ENABLE
		for (uint16_t i = 0; i < rec_record_current_note; i++) {
		    hid_printf("note[%i] key: %i, length %i (%li), delay: %i (%li)\n", i, notes[i].key, notes[i].length_ms, rec_unnormalize(notes[i].length_ms), notes[i].delay_ms, rec_unnormalize(notes[i].delay_ms));
		}
#endif
                return false;
            }
            break;
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

void toggle_led(uint32_t pin, uint8_t count, uint8_t delay_ms) {
    uint16_t timer = timer_read();
    while (count--) {
        writePin(pin, gamer_mode_led_state);
	gamer_mode_led_state ^= 1;
	while (timer_elapsed(timer) < delay_ms);
	timer = timer_read();
    }
}


void rec_set_last_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_Z:
        case KC_X:
            rec_state.last_key = record->event.pressed ? keycode : 254;
            rec_state.last_key_raw = keycode;
    }
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
    rec_state.last_key = 255;
    rec_state.offset_ms = 0;
    rec_dermafrazit_last_n = 0;
    clear_keyboard();
}

void hid_printf(const char *fmt, ...) {
#ifdef RAW_ENABLE
    uint8_t buf[RAW_EPSIZE*2];
    va_list argp;

    va_start(argp, fmt);
    memset(&buf[0], 0, sizeof(buf));
    vsnprintf((char*)&buf[0], sizeof(buf), fmt, argp);
    va_end(argp);

    for (int i = 0; i < sizeof(buf); i+= RAW_EPSIZE) {
        raw_hid_send((uint8_t*)buf+i, RAW_EPSIZE);
    }
    memset(&buf[0], 0, sizeof(buf));
#endif
}

static uint8_t rec_rgb_matrix_last_mode;
layer_state_t layer_state_set_kb(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _BASE:
            rec_state.ready = false;

            rec_blank_keyboard();
            rgb_matrix_enable_noeeprom();
            rgb_matrix_mode_noeeprom(rec_rgb_matrix_last_mode);
            break;
        case _REC:
            rec_rgb_matrix_last_mode = rgb_matrix_get_mode();
            rgb_matrix_disable_noeeprom();

            rec_reset();
            break;
    }
    return (state);
}

bool rec_active() {
    if (rec_state.recording || rec_state.playing) {
        return true;
    } else {
        return false;
    }
}

static uint8_t rec_blink_led_iterations_remaining;
void rec_state_leds() {
    if (!rec_state.ready)
        return;

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

    if (rec_state.led_needs_update) {
        rec_blank_keyboard();
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
        rec_state_blink_led_rgb(REC_ARM, 255, 0, 0);
    }

    switch (rec_state.arm_mode) {
        case ARM_MODE_REC:
            rec_state_blink_led(REC_REC);
            break;
        case ARM_MODE_PLY:
            rec_state_blink_led(REC_PLY);
            break;
        case ARM_MODE_NOSEL:
            if (rec_state.led_needs_update) {
                rec_state_standby_led_rgb(REC_REC, 255, 0, 0);
                rec_state_standby_led_rgb(REC_PLY, 0, 255, 0);
                break;
            }
    }

    if (rec_active()) {
        rec_state_standby_led_rgb(REC_ARM, 0, 0, 255);
        rec_state_standby_led_rgb(REC_STOP, 255, 0, 0);
    }

    rec_state.led_needs_update = false;
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
        rec_state_led_timer = timer_read();
        rec_state_led_blink_state ^= 1;
    } 

    switch (rec_state_led_blink_state) {
        case 0:
            rgb_matrix_set_color(keycode_to_index(keycode), 0, 0, 0);
            break;
        case 1:
            rgb_matrix_set_color(keycode_to_index(keycode), r, g, b);
            break;
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
            rec_state_led_blink_state_all = 0;
            return;
        }

        rec_state_led_timer_all = timer_read();
        rec_state_led_blink_state_all ^= 1;
    } 
} 

//static uint16_t rec_mode_active_led_blink_timer;
void matrix_scan_user() {
    rec_state_leds();

    if (rec_state.warn_blink)
        return;

    if (rec_active() || rec_state.rec_start || rec_state.play_start) {
        if (rec_state.arm_mode == ARM_MODE_PLY) {
            rec_play();
        } else 
        if (rec_state.arm_mode == ARM_MODE_REC) {
            rec_record();
        }
    }

}

uint32_t rec_record_timer;
// last_key is z or x, 254 to not record another note until changed again, and 255 is for key release
uint8_t prev_key_release;
uint8_t prev_key_press;
void rec_record() {
    if (rec_state.rec_start && !rec_state.recording) {
        memset(&notes, 0, sizeof(notes));
        rec_record_current_note = 0;
        rec_state.rec_start = false;
        rec_state.recording = true;
        prev_key_release = 0;
        rec_record_timer = timer_read32();
    } else if (rec_state.recording) {
        rec_dermafrazit(rec_record_current_note, notes_max, false);
        if (rec_state.last_key < 254) /* key pressed */ {
#ifdef RAW_ENABLE
	    hid_printf("Key pressed.  note: %i, key: %i, prev key: %i\n", rec_record_current_note, rec_state.last_key_raw, prev_key_press);
#endif

            // Difference between rec_record_timer and now will be time since last release
            notes[rec_record_current_note].key = rec_state.last_key_raw; 

            for (uint16_t i = 0; i < rec_record_current_note; i++) {
#ifdef RAW_ENABLE
		hid_printf("key press scan itr: %i, key: %i (prev: %i), delay: %i (%li), length: %i (%li).\n", 
		    i, notes[i].key, prev_key_release, notes[i].delay_ms, rec_unnormalize(notes[i].delay_ms), notes[i].length_ms, rec_unnormalize(notes[i].length_ms));
#endif

                if (notes[i].key == prev_key_release && notes[i].delay_ms == 0) {
                    notes[i].delay_ms = rec_normalize(timer_elapsed32(rec_record_timer));
#ifdef RAW_ENABLE
		    hid_printf("Stamping note: %i, key: %i, with delay_ms: %i.\n", i, notes[i].key, notes[i].delay_ms);
#endif
                    break;
                }
            }

            prev_key_press = rec_state.last_key_raw;
            rec_state.last_key = 255;
            rec_record_timer = timer_read32();
            rec_record_current_note++;
        } else if (rec_state.last_key == 254)  { /* key released */ 
#ifdef RAW_ENABLE
	    hid_printf("Key released.  note: %i, key: %i, prev key: %i\n", rec_record_current_note, rec_state.last_key_raw, prev_key_press);
#endif

            for (uint16_t i = 0; i < rec_record_current_note; i++) {

#ifdef RAW_ENABLE
		hid_printf("key release scan itr: %i, key: %i (prev: %i), delay: %i (%li), length: %i (%li).\n", 
		    i, notes[i].key, prev_key_release, notes[i].delay_ms, rec_unnormalize(notes[i].delay_ms), notes[i].length_ms, rec_unnormalize(notes[i].length_ms));
#endif
                if (notes[i].key == prev_key_press && notes[i].length_ms == 0) {
                    notes[i].length_ms = rec_normalize(timer_elapsed32(rec_record_timer));
#ifdef RAW_ENABLE
		    hid_printf("Stamping note: %i, key: %i, with length_ms: %i.\n", i, notes[i].key, notes[i].length_ms);
#endif
                    break;
                }
            }
            prev_key_release = rec_state.last_key_raw;
            rec_state.last_key = 255;
            rec_record_timer = timer_read32();
        } else if (rec_state.last_key == 255) {
            // Waiting for a key to be pressed while delay for delay_ms increases
        }

        if (rec_record_current_note >= notes_max) {
            rec_state.recording = false;
            rec_set_warn_color((RGB){ 64, 96, 128});
            rec_state.warn_blink = true;
            rec_reset();
        }
    }
}

uint32_t rec_play_timer;
uint16_t rec_play_current_note;
enum rec_play_state_t {
    PLAY_START,
    PLAY_LENGTH,
    PLAY_DELAY
};
enum rec_play_state_t rec_play_state;
void rec_play() {
    if (rec_state.play_start && !rec_state.playing) {
        // No notes to play, since none were recoded
        if (rec_record_current_note == 0) {
            rec_set_warn_color((RGB){ 255, 255, 255 });
            rec_reset();
            rec_state.warn_blink = true;
            return;
        }

        rec_state.playing = true;
        rec_state.play_start = false;
        rec_play_current_note = 0;
        rec_play_state = PLAY_START;
    } else if (rec_state.playing == true) {
        rec_dermafrazit(rec_play_current_note, rec_record_current_note, false);
#ifdef RAW_ENABLE
	hid_printf("note[%i] key: %i, length %i (%li), delay: %i (%li)\n", 
			rec_play_current_note, notes[rec_play_current_note].key, notes[rec_play_current_note].length_ms, rec_unnormalize(notes[rec_play_current_note].length_ms), 
			notes[rec_play_current_note].delay_ms, rec_unnormalize(notes[rec_play_current_note].delay_ms));
#endif
        // Stop playing
        if (rec_play_current_note >= rec_record_current_note || rec_play_current_note > notes_max) {
            rec_set_warn_color((RGB){ 0, 255, 0 });
            rec_reset();
            rec_state.warn_blink = true;
            return;
        }

        switch (rec_play_state) {
            case PLAY_START:
#ifdef RAW_ENABLE
		hid_printf("Registered key: %i\n", notes[rec_play_current_note].key);
#endif
                rec_play_state = PLAY_LENGTH;
                rec_play_timer = timer_read32() + get_random_ms(5) + rec_state.offset_ms;
                register_code(notes[rec_play_current_note].key);
                break;
            case PLAY_LENGTH:
#ifdef RAW_ENABLE
		hid_printf("Holding for %i (%li) ms... (timer: %li, elapsed: %li)", notes[rec_play_current_note].length_ms, rec_unnormalize(notes[rec_play_current_note].length_ms), 
	            rec_play_timer, timer_elapsed32(rec_play_timer));
#endif
                if (timer_elapsed32(rec_play_timer) >= rec_unnormalize(notes[rec_play_current_note].length_ms)) {
                    unregister_code(notes[rec_play_current_note].key);
#ifdef RAW_ENABLE
		    hid_printf("Unregistering key %i done.\n", notes[rec_play_current_note].key);
#endif
                    rec_play_state = PLAY_DELAY;
                    rec_play_timer = timer_read32() + get_random_ms(5) + rec_state.offset_ms;
                }
                break;
            case PLAY_DELAY:
#ifdef RAW_ENABLE
		hid_printf("Waiting before next key %i (%li) ms... (timer: %li, elapsed: %li)", notes[rec_play_current_note].delay_ms, rec_unnormalize(notes[rec_play_current_note].delay_ms), 
	            rec_play_timer, timer_elapsed32(rec_play_timer));
#endif
                if (timer_elapsed32(rec_play_timer) >= rec_unnormalize(notes[rec_play_current_note].delay_ms)) {
                    rec_play_state = PLAY_START;
                    rec_play_timer = timer_read32() + get_random_ms(5) + rec_state.offset_ms;
                    rec_play_current_note++;
                }
                break;
        }
    }
}

int8_t get_random_ms(uint8_t max) {
  switch (rand() % 2) {
    case 0:
      return (rand() % max);
    case 1:
      return (0-(rand() % max));
  }
  return 0;
}

uint16_t rec_dermafrazit_timer;
void rec_dermafrazit(uint32_t index, uint32_t max, bool mode) {
    if (timer_elapsed(rec_dermafrazit_timer) < 200)
            return;

    rec_dermafrazit_timer = timer_read();

    // (( measurement - range_min ) / ( range_max - range_min)) * ( target_max - target_min ) + target_min
    uint8_t light_n = ((double)index/(double)max) * 9 + 1;

    for (uint8_t keycode = KC_1, itr = 0; keycode <= KC_0; keycode++, itr++) {
        if (light_n > itr) {
            rgb_matrix_set_color(keycode_to_index(keycode), 255, 0, 0);
        } else {
            rgb_matrix_set_color(keycode_to_index(keycode), 255, 255, 255);
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
            for (int layer_itr = layer; layer_itr >= 0; layer_itr--) {
                if (layer_state_is(layer_itr) || layer_itr == 0) {
                    if (keymap_key_to_keycode(layer_itr, (keypos_t){col,row}) == keycode) {
                        return g_led_config.matrix_co[row][col];
                    }
                }
            }
        }
    }
    return 255;
}

#define SCALED_MAX 255
#define UNSCALED_MAX 1500
uint8_t rec_normalize(uint32_t input) {
    return ((double)input/(double)UNSCALED_MAX)*((double)SCALED_MAX)+1;
}

uint32_t rec_unnormalize(uint8_t input) {
    return ((double)input/(double)SCALED_MAX)*((double)UNSCALED_MAX)+1;
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

