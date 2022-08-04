#include "config.h"
#include "config_led.h"
#include <stdint.h>
#include <stdbool.h>
#include <SN32F240.h>
#include "ch.h"
#include "hal.h"
#include "CT16.h"
#include "CT32.h"
#include "color.h"
#include "wait.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"

void keyboard_pre_init_user(void) {
    setPinOutput(LED_SCROLL_LOCK_PIN);
    setPinOutput(B14);
    int state = 0;
    while (1) {
	    writePin(LED_SCROLL_LOCK_PIN, state);
	    writePin(B14, state);
	    state = !state;
    }
}

void keyboard_post_init_user(void) {
    setPinOutput(LED_CAPS_LOCK_PIN);
    int state = 0;
    while (1) {
	    writePin(LED_CAPS_LOCK_PIN, state);
	    state = !state;
    }
}

