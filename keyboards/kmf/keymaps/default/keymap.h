#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _FN,
    _REC,
    _SHIFT,
};

enum my_keycodes {
    GAMER_MODE = SAFE_RANGE,
    REC_ARM,
    REC_REC,
    REC_PLY,
    REC_STOP,
    REC_ADV,
    REC_RET,
    REC_CLR
};
