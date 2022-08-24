/* Copyright 2021 IsaacDynamo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "white.h"

// Manage Windows and Mac LEDs
// - Show status of mode switch
// - Turn LEDs off durring USB suspend
static bool mode_leds_show = true;
static bool mode_leds_windows;

static void mode_leds_update(void){
    writePin(LED_WIN_PIN, mode_leds_show && mode_leds_windows);
    writePin(LED_MAC_PIN, mode_leds_show && !mode_leds_windows);
}

void dip_switch_update_kb(uint8_t index, bool active){
    if(index == 0) {
        if(active) { // Mac mode
            layer_move(MAC_BASE);
        } else { // Windows mode
            layer_move(WIN_BASE);
        }

        // Update mode and update leds
        mode_leds_windows = !active;
        mode_leds_update();
    }

    dip_switch_update_user(index, active);
}

void keyboard_pre_init_kb(void) {
    // Setup Win & Mac LED Pins as output
    setPinOutput(LED_WIN_PIN);
    setPinOutput(LED_MAC_PIN);

    // WORKAROUND: Mac & Windows LED flicker.
    // Normally the GPIOs in DIP_SWITCH_PINS will be initialized by dip_switch_init().
    // But during startup of the keyboard the Mac/Windows dip switch seems to jitter, causing the Mac and Windows LEDs to flicker.
    // Maybe the internal pull-up of this chip is really weak, and needs some time to pullup the input voltage to the high level? Seems unlikely but cannot think of a better explanation.
    // By doing the configuration of the GPIOs here the pullup is enabled earlier and the flicker is gone.
    const pin_t dip_switch_pad[] = DIP_SWITCH_PINS;
    const size_t size = sizeof(dip_switch_pad) / sizeof(dip_switch_pad[0]);
    for (size_t i=0; i<size; i++) {
        setPinInputHigh(dip_switch_pad[i]);
    }

    keyboard_pre_init_user();
}

void suspend_power_down_kb(void) {
    // Turn leds off
    mode_leds_show = false;
    mode_leds_update();

    // Suspend backlight
    rgb_matrix_set_suspend_state(true);

    suspend_power_down_user();
}

/// TODO: Clean-up workaround
/// Currently the suspend_wakeup_init_kb() has issues. See https://github.com/SonixQMK/qmk_firmware/issues/80
/// A workaround is to use housekeeping_task_kb() instead.
void housekeeping_task_kb(void) {
    // Turn on
    mode_leds_show = true;
    mode_leds_update();

    // Restore backlight
    rgb_matrix_set_suspend_state(false);

    housekeeping_task_user();
}


#ifdef RGB_MATRIX_ENABLE

#include "rgb_matrix.h"

#define XX NO_LED

// Mark keys that are black & orange with the default keychron keycaps.
// This is used by the custom rgb matrix effect to create a high contrast mode that only lights up black keys or black and orange keys. To make them better readable.
#define B (128 | 4) // Black
#define O (64  | 4) // Orange, Esc and lightbulb are assumed to be orange

/// Force g_led_config into flash, because there is no space in RAM.
/// This should be safe because g_led_config should never be written to.
/// We cannot make g_led_config const, because rgb_matrix.h, exports it as mutable.
__attribute__(( section(".rodata.g_led_config") ))
led_config_t g_led_config = { {
  {  0, 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,  13, 14, 15},
  { 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, XX, 30},
  { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, XX, 45},
  { 46, 47, 48, 49 ,50, 51, 52, 53, 54, 55, 56, 57, XX, 58, XX, 59},
  { 60, XX, 61, 62, 63 ,64, 65, 66, 67, 68, 69, 70, XX, 71, 72, 73},
  { 74, 75, 76, XX, XX, XX, 77, XX, XX, XX, 78, 79, 80, 81, 82, 83}
}, {

   { 1*12, 1 },  { 2*12, 1 },  { 3*12, 1 },  { 4*12, 1 },  { 5*12, 1 },  { 6*12, 1 },  { 7*12, 1 },  { 8*12, 1 },  { 9*12, 1 },  { 10*12, 1 },  { 11*12, 1 },  { 12*12, 1 },  { 13*12, 1 },  { 14*12, 1 },  { 15*12, 1 },  { 16*12, 1 },
   { 1*12, 13 }, { 2*12, 13 }, { 3*12, 13 }, { 4*12, 13 }, { 5*12, 13 }, { 6*12, 13 }, { 7*12, 13 }, { 8*12, 13 }, { 9*12, 13 }, { 10*12, 13 }, { 11*12, 13 }, { 12*12, 13 }, { 13*12, 13 }, { 14*12, 13 },                { 16*12, 13 },
   { 1*12, 26 }, { 2*12, 26 }, { 3*12, 26 }, { 4*12, 26 }, { 5*12, 26 }, { 6*12, 26 }, { 7*12, 26 }, { 8*12, 26 }, { 9*12, 26 }, { 10*12, 26 }, { 11*12, 26 }, { 12*12, 26 }, { 13*12, 26 }, { 14*12, 26 },                { 16*12, 26 },
   { 1*12, 37 }, { 2*12, 37 }, { 3*12, 37 }, { 4*12, 37 }, { 5*12, 37 }, { 6*12, 37 }, { 7*12, 37 }, { 8*12, 37 }, { 9*12, 37 }, { 10*12, 37 }, { 11*12, 37 }, { 12*12, 37 },                { 14*12, 37 },                { 16*12, 37 },
   { 1*12, 49 },               { 3*12, 49 }, { 4*12, 49 }, { 5*12, 49 }, { 6*12, 49 }, { 7*12, 49 }, { 8*12, 49 }, { 9*12, 49 }, { 10*12, 49 }, { 11*12, 49 }, { 12*12, 49 },                { 14*12, 49 }, { 15*12, 49 }, { 16*12, 49 },
   { 1*12, 61 }, { 2*12, 61 }, { 3*12, 61 },                                           { 7*12, 61 },                                            { 11*12, 61 }, { 12*12, 61 }, { 13*12, 61 }, { 14*12, 61 }, { 15*12, 61 }, { 16*12, 61 },
}, {
   O, 4, 4, 4, 4, B, B, B, B, 4, 4, 4, 4, B, B, B,
   B, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, B,    B,
   B, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, B,    B,
   B, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    B,    B,
   B,    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,    B, 4, B,
   B, B, B,          4,          B, B, B, 4, 4, 4,
} };

#endif
