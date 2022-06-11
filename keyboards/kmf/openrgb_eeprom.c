#include <stdlib.h>
#include <string.h>
#include "openrgb_eeprom.h"
#include "config_led.h"
#include "eeprom.h"
#include "eeconfig.h"
#include "quantum/color.h"
#include "openrgb.h"
#include "rgb_matrix.h"

#ifdef OPENRGB_ENABLE
uint8_t openrgb_buf[EECONFIG_OPENRGB_DIRECT_ARRAY_SIZE];

void openrgb_load_rgb_state_eeprom(int force) {
    if (force == false)
        if ((uint32_t)rgb_matrix_get_mode != (uint32_t)RGB_MATRIX_OPENRGB_DIRECT)
            return;

    memset(&openrgb_buf, 0x00, EECONFIG_OPENRGB_DIRECT_ARRAY_SIZE);
    eeprom_read_block(&openrgb_buf, (void*)EECONFIG_OPENRGB_DIRECT_ARRAY, EECONFIG_OPENRGB_DIRECT_ARRAY_SIZE);

    rgb_matrix_mode(RGB_MATRIX_OPENRGB_DIRECT);

    int openrgb_index = 0;
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        for (int j = 0; j < LED_MATRIX_COLS; j++) {
            if (g_led_config.matrix_co[i][j] != NO_LED) {
		g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].r = openrgb_buf[openrgb_index+0];
		g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].g = openrgb_buf[openrgb_index+1];
		g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].b = openrgb_buf[openrgb_index+2];

		openrgb_index += 3;
	    }
	}
    }
}

void openrgb_save_rgb_state_eeprom(void) {
    memset(&openrgb_buf, 0x00, EECONFIG_OPENRGB_DIRECT_ARRAY_SIZE);

    int openrgb_index = 0;
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        for (int j = 0; j < LED_MATRIX_COLS; j++) {
            if (g_led_config.matrix_co[i][j] != NO_LED) {
                openrgb_buf[openrgb_index+0] = g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].r;
                openrgb_buf[openrgb_index+1] = g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].g;
                openrgb_buf[openrgb_index+2] = g_openrgb_direct_mode_colors[g_led_config.matrix_co[i][j]].b;
		
		openrgb_index += 3;
            }
        }
    }

    eeprom_write_block(&openrgb_buf, (void*)EECONFIG_OPENRGB_DIRECT_ARRAY, EECONFIG_OPENRGB_DIRECT_ARRAY_SIZE);
}
#endif
