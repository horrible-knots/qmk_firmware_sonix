RGB_MATRIX_EFFECT(SINGLE_PRESS_FADE)
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
static HSV SINGLE_PRESS_FADE_math(HSV hsv, uint16_t offset) {
	hsv.s = scale8(hsv.s, offset);
	return hsv;
}

bool SINGLE_PRESS_FADE(effect_params_t *params) {
	return(effect_runner_reactive(params, &SINGLE_PRESS_FADE_math));
}
#endif
