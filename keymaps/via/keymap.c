#include QMK_KEYBOARD_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_turbopad(TO(1), KC_F13, KC_F14, KC_F15, KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, KC_F21),
    [1] = LAYOUT_turbopad(TO(2), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),
    [2] = LAYOUT_turbopad(TO(3), KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO),
    [3] = LAYOUT_turbopad(TO(0), KC_AT, KC_T, KC_U, KC_R, KC_B, KC_I, KC_N, KC_A, KC_4),
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = {ENCODER_CCW_CW(KC_PGDN, KC_PGUP)},
    [1] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
    [2] = {ENCODER_CCW_CW(KC_NO, KC_NO)},
    [3] = {ENCODER_CCW_CW(KC_LEFT, KC_RIGHT)},
};
#endif

#ifdef OLED_ENABLE
#    include "action_layer.h"
#    include "oled_driver.h"

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    oled_clear();
    return rotation;
}

static uint16_t get_resolved_keycode(uint8_t top_layer, uint8_t row, uint8_t col) {
    for (int8_t l = top_layer; l >= 0; l--) {
        if (l == 0 || layer_state_is(l)) {
            uint16_t kc = dynamic_keymap_get_keycode(l, row, col);
            if (kc != KC_TRANSPARENT) {
                return kc;
            }
        }
    }
    return KC_NO;
}

static uint16_t get_resolved_encoder(uint8_t top_layer, uint8_t encoder_id, bool clockwise) {
    for (int8_t l = top_layer; l >= 0; l--) {
        if (l == 0 || layer_state_is(l)) {
            uint16_t kc = dynamic_keymap_get_encoder(l, encoder_id, clockwise);
            if (kc != KC_TRANSPARENT) {
                return kc;
            }
        }
    }
    return KC_NO;
}

static void format_keycode_4chars(uint16_t kc, char *out_buf) {
    if (kc == KC_NO) {
        strcpy(out_buf, "----");
        return;
    }

    if (kc >= KC_A && kc <= KC_Z) {
        snprintf(out_buf, 5, " %c  ", 'A' + (kc - KC_A));
        return;
    }
    if (kc >= KC_1 && kc <= KC_9) {
        snprintf(out_buf, 5, " %c  ", '1' + (kc - KC_1));
        return;
    }
    if (kc == KC_0) {
        strcpy(out_buf, " 0  ");
        return;
    }
    if (kc >= KC_F1 && kc <= KC_F12) {
        snprintf(out_buf, 5, "F%-2d ", (kc - KC_F1) + 1);
        return;
    }
    if (kc >= KC_F13 && kc <= KC_F24) {
        snprintf(out_buf, 5, "F%-2d ", (kc - KC_F13) + 13);
        return;
    }
    if (kc >= QK_MACRO && kc <= QK_MACRO_MAX) {
        snprintf(out_buf, 5, "M%02d ", kc - QK_MACRO);
        return;
    }
    if (kc >= JS_0 && kc <= JS_31) {
        snprintf(out_buf, 5, "JS%-2d", kc - JS_0);
        return;
    }

    if (IS_QK_MODS(kc)) {
        uint8_t mods     = QK_MODS_GET_MODS(kc);
        uint8_t basic    = QK_MODS_GET_BASIC_KEYCODE(kc);
        char    mod_char = (mods & MOD_MASK_CTRL) ? 'C' : (mods & MOD_MASK_SHIFT) ? 'S' : (mods & MOD_MASK_ALT) ? 'A' : 'G';

        if (basic >= KC_A && basic <= KC_Z) {
            snprintf(out_buf, 5, "%c+%c ", mod_char, 'A' + (basic - KC_A));
            return;
        }
    }

    switch (kc) {
        case KC_AT:
            strcpy(out_buf, " @  ");
            return;
        case KC_ENT:
            strcpy(out_buf, "ENTR");
            return;
        case KC_ESC:
            strcpy(out_buf, "ESC ");
            return;
        case KC_BSPC:
            strcpy(out_buf, "BSPC");
            return;
        case KC_TAB:
            strcpy(out_buf, "TAB ");
            return;
        case KC_SPC:
            strcpy(out_buf, "SPC ");
            return;
        case KC_DEL:
            strcpy(out_buf, "DEL ");
            return;
        case KC_PGUP:
            strcpy(out_buf, "PGUP");
            return;
        case KC_PGDN:
            strcpy(out_buf, "PGDN");
            return;
        case KC_UP:
            strcpy(out_buf, " UP ");
            return;
        case KC_DOWN:
            strcpy(out_buf, "DOWN");
            return;
        case KC_LEFT:
            strcpy(out_buf, "LEFT");
            return;
        case KC_RGHT:
            strcpy(out_buf, "RGHT");
            return;
        case KC_MUTE:
            strcpy(out_buf, "MUTE");
            return;
        case KC_VOLU:
            strcpy(out_buf, "VOL+");
            return;
        case KC_VOLD:
            strcpy(out_buf, "VOL-");
            return;
        case KC_MPLY:
            strcpy(out_buf, "PLAY");
            return;
        case KC_MNXT:
            strcpy(out_buf, "NEXT");
            return;
        case KC_MPRV:
            strcpy(out_buf, "PREV");
            return;
#    ifdef RGBLIGHT_ENABLE
        case RGB_SAI:
            strcpy(out_buf, "SAI+");
            return;
        case RGB_SAD:
            strcpy(out_buf, "SAD-");
            return;
        case RGB_VAI:
            strcpy(out_buf, "BRI+");
            return;
        case RGB_VAD:
            strcpy(out_buf, "BRI-");
            return;
        case RGB_HUI:
            strcpy(out_buf, "HUE+");
            return;
        case RGB_HUD:
            strcpy(out_buf, "HUE-");
            return;
        case RGB_MOD:
            strcpy(out_buf, "RMOD");
            return;
        case RGB_TOG:
            strcpy(out_buf, "RTOG");
            return;
#    endif
        default:
            snprintf(out_buf, 5, "%04X", kc);
            return;
    }
}

bool oled_task_user(void) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    char    line_buf[21];

    oled_set_cursor(0, 0);
    snprintf(line_buf, sizeof(line_buf), "TURBO_PAD  WARSTWA:%1d", layer);
    oled_write(line_buf, false);

    char enc_ccw[5], enc_cw[5];
    format_keycode_4chars(get_resolved_encoder(layer, 0, false), enc_ccw);
    format_keycode_4chars(get_resolved_encoder(layer, 0, true), enc_cw);
    oled_set_cursor(0, 1);
    snprintf(line_buf, sizeof(line_buf), "<-[%s]    [%s]->", enc_ccw, enc_cw);
    oled_write(line_buf, false);

    oled_set_cursor(0, 2);
    oled_write_P(PSTR("--------------------"), false);

    for (uint8_t row = 1; row <= 3; row++) {
        char k0[5], k1[5], k2[5];
        format_keycode_4chars(get_resolved_keycode(layer, row, 0), k0);
        format_keycode_4chars(get_resolved_keycode(layer, row, 1), k1);
        format_keycode_4chars(get_resolved_keycode(layer, row, 2), k2);

        snprintf(line_buf, sizeof(line_buf), "[%s] [%s] [%s]", k0, k1, k2);

        oled_set_cursor(0, (row * 2) + 1);
        oled_write(line_buf, false);
    }

    return false;
}
#endif
