/*
Copyright 2022 Pierre Prablanc

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
#include QMK_KEYBOARD_H
#define   _BASE		0
#define   _FN0     1 
#define   _FN1     2


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

   
    /*  Main layout
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
 * │esc│f1 │ f2│ f3│ f4│ f5│ f6│ f7│ f8│ f9│f10│f11│f12│scr│hak│del│
 * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┴───┼───┤
 * │ ~ │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ BACKSP│PUP│
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┼───┤
 * │ TAB │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │  |  │PDO│
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┼───┤
 * │ CAPS │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │   RET  │HOM│
 * ├──────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┼───┤
 * │ SHIFT  │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │ SHIFT│UP │END│
 * ├────┬───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬───┼───┼───┤
 * │CTRL│ WIN│ALT │                        │ALT│FN1│FN2│LFT│DWN│RGT│
 * └────┴────┴────┴────────────────────────┴───┴───┴───┴───┴───┴───┘
    */

    [_BASE] = LAYOUT( \
/*Row:  0                     1         2         3         4         5         6         7         8         9         10        11          12          13                      14          15      */
        KC_ESC,   			      KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   	KC_PSCR,                MO(_FN1),   KC_DEL, \
        KC_GRV,   			      KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,    KC_EQL,   	KC_BSPC,                        	  KC_HOME, \
        MT(MOD_LGUI, KC_TAB),	KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,    KC_RBRC,  	MT(MOD_LGUI,  KC_BSLS),             KC_PGUP, \
        MT(MOD_LCTL, KC_ESC),	KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,                KC_ENT,                        	    KC_PGDN, \
        KC_LSFT,  			                KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,                KC_RSFT,                KC_UP,      KC_END, \
        KC_LCTL,  			      KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT,  MO(_FN0),	  KC_RCTL,    KC_LEFT,                  KC_DOWN,  	KC_RGHT \
    ),

    [_FN0] = LAYOUT( \
/*Row:  0         1         2         3         4         5         6         7         8         9          10        11        12        13        14        15      */
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,  RGB_MOD, \
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  RESET,              _______, \
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,            _______, \
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,            _______, \
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,  RGB_SPI,  _______, \
        _______,  _______,  _______,                                _______,                                 _______,  _______,  _______,  RGB_RMOD, RGB_SPD,  RGB_MOD  \
    ),

    [_FN1] = LAYOUT( \
/*Row:  0         1               2                   3                 4                5         6         7         8         9          10        11        12        13                   14                    15      */
        _______,  KC_AUDIO_MUTE,  KC_AUDIO_VOL_DOWN,  KC_AUDIO_VOL_UP,  _______,         _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,             _______,              _______,            \
        _______,  _______,        _______,            _______,          KC_SYSTEM_SLEEP, _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,                                   KC_MY_COMPUTER,     \
        _______,  _______,        _______,            _______,          _______,         _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,                                   _______,            \
        _______,  _______,        _______,            _______,          _______,         _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,                                   _______,            \
        _______,                  _______,            _______,          _______,         _______,  _______,  _______,  _______,  _______,   _______,  _______,            _______,             KC_MEDIA_STOP,        _______,            \
        _______,  _______,        _______,                                                         _______,                                 _______,  _______,  _______,  KC_MEDIA_PREV_TRACK, KC_MEDIA_PLAY_PAUSE,  KC_MEDIA_NEXT_TRACK \
    )
};
