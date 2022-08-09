/*
Copyright 2022 Nobody

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#include "keymap.h"
#define XXX KC_NO

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*  Row:        0           1           2           3        4           5        6        7           8           9           10          11          12          13          14          15          16          17          18          19          20     */
    [_BASE] = LAYOUT_default(
	    	   KC_ESC,                 KC_F1,      KC_F2,   KC_F3,      KC_F4,   KC_F5,   KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,     KC_F11,     KC_F12,     KC_PSCR,    KC_SLCK,    KC_PAUS,                                        
                   KC_GRV,     KC_1,       KC_2,       KC_3,    KC_4,       KC_5,    KC_6,    KC_7,       KC_8,       KC_9,       KC_0,       KC_MINS,    KC_EQL,     KC_BSPC,    KC_INS,     KC_HOME,    KC_PGUP,    KC_NLCK,    KC_PSLS,    KC_PAST,    KC_PMNS ,
                   KC_TAB,     KC_Q,       KC_W,       KC_E,    KC_R,       KC_T,    KC_Y,    KC_U,       KC_I,       KC_O,       KC_P,       KC_LBRC,    KC_RBRC,    KC_BSLS,    KC_DEL,     KC_END,     KC_PGDN,    KC_P7,      KC_P8,      KC_P9,      KC_PPLS ,
		   KC_CAPS,    KC_A,       KC_S,       KC_D,    KC_F,       KC_G,    KC_H,    KC_J,       KC_K,       KC_L,       KC_SCLN,    KC_QUOT,                KC_ENT,                                         KC_P4,      KC_P5,      KC_P6,      
                   KC_LSFT,                KC_Z,       KC_X,    KC_C,       KC_V,    KC_B,    KC_N,       KC_M,       KC_COMM,    KC_DOT,     KC_SLSH,                KC_RSFT,                KC_UP,                  KC_P1,      KC_P2,      KC_P3,      KC_PENT ,
                   KC_LCTL,    KC_LWIN,    KC_LALT,                                  KC_SPC,                                      KC_RALT,    KC_APP,     MO(_FN),    KC_RCTL,    KC_LEFT,    KC_DOWN,    KC_RIGHT,               KC_P0,      KC_PDOT    
	      ),
    [_FN]   = LAYOUT_default(
	    	   RESET,                  KC_VOLD,    KC_VOLU, KC_MUTE,    KC_MSTP, KC_MPRV, KC_MPLY,    KC_MNXT,    KC_MAIL,    KC_WHOM,    KC_CALC,    RGB_TOG,    _______,    _______,    _______,    KC_SLEP,    
                   _______,    _______,    _______,    _______, _______,    _______, _______, _______,    _______,    _______,    _______,    RGB_SPD,    RGB_SPI,    _______,    RGB_M_P,    RGB_M_B,    RGB_MOD ,   _______,    _______,    _______,    _______ ,
                   _______,    _______,    _______,    _______,TG(_REC),    _______, _______, _______,    _______,    _______,    _______,    _______,                _______,    RGB_M_SW,   RGB_M_G,    RGB_RMOD,   _______,    _______,    _______,    _______ ,
                   _______,    _______,    _______,    _______, _______,    _______, _______, _______,    _______,    _______,    _______,    _______,    	      _______,                                        _______,    _______,    _______,    
                   _______,                _______,    _______, _______,    _______, _______, _______,    _______,    _______,    _______,    _______,    _______,    _______,                RGB_VAI,                KC_BRID,    KC_BRIU,    _______,    _______ ,
                   _______,    GAMER_MODE, _______,                                  _______,                                     _______,    _______,    _______,    _______,    RGB_HUD,    RGB_VAD,    RGB_HUI,                _______,    _______ 
	      ),
    [_REC]   = LAYOUT_default(
	    	   KC_ESC,                 XXX,        XXX,     XXX,        XXX,     XXX,     XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,      
                   XXX,        XXX,        XXX,        XXX,     XXX,        XXX,     XXX,     XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,  
                   XXX,        TG(_REC),   XXX,        XXX,     REC_CLR,    XXX,     XXX,     XXX,        REC_REC,    REC_PLY,    XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,        XXX,  
                   XXX,        REC_ARM,    REC_STOP,   XXX,     XXX,        XXX,     XXX,     XXX,        XXX,        XXX,        XXX,        XXX,                    XXX,                                            XXX,        XXX,        XXX,
                   XXX,                    KC_Z,       KC_X,    XXX,        XXX,     XXX,     XXX,        XXX,        XXX,        XXX,        XXX,                    XXX,                    XXX,                    XXX,        XXX,        XXX,        XXX,  
                   XXX,        XXX,        XXX,                                      XXX,                                         XXX,        XXX,        XXX,        XXX,        REC_RET,    XXX,        REC_ADV,                XXX,        XXX
	      ),
};

