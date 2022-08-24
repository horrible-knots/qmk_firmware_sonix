#ifndef DISABLE_RGB_MATRIX_HIGH_CONTRAST
RGB_MATRIX_EFFECT(HIGH_CONTRAST)
RGB_MATRIX_EFFECT(HIGH_CONTRAST2)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

// Only light up black keys. These are marked with 128 in g_led_config.
bool HIGH_CONTRAST(effect_params_t* params) {
    RGB rgb = rgb_matrix_hsv_to_rgb(rgb_matrix_config.hsv);

    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        if (HAS_FLAGS(g_led_config.flags[i], 128)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        } else {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
    return led_max < DRIVER_LED_TOTAL;
}

// Only light up black and orange keys. These are marked with 128 and 64 in g_led_config.
// Use this high contrast mode when you use black keycaps for Esc and lightbulb key and what them to be lit as wel.
bool HIGH_CONTRAST2(effect_params_t* params) {
    RGB rgb = rgb_matrix_hsv_to_rgb(rgb_matrix_config.hsv);

    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        if (HAS_FLAGS(g_led_config.flags[i], 128) || HAS_FLAGS(g_led_config.flags[i], 64)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        } else {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }
    return led_max < DRIVER_LED_TOTAL;
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // DISABLE_RGB_MATRIX_HIGH_CONTRAST
