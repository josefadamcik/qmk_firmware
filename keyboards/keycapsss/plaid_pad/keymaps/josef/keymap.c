#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
 * ,-----------------------,
 * |  7  |  8  |  9  |  TAB  |
 * |-----+-----+-----+-----|
 * |  4  |  5  |  6  |  BSPC  |
 * |-----+-----+-----+-----|
 * |  1  |  2  |  3  |  +  |
 * |-----+-----+-----+-----|
 * |  0  |  0  |  .  |  KC_ENT  |
 * `-----------------------'
 */
LAYOUT_ortho_4x4(
		KC_P7, KC_P8,   KC_P9,      KC_NLCK,
		KC_P4, KC_P5,   KC_P6,      KC_BSPC,
		KC_P1, KC_P2,   KC_P3,      KC_PLUS,
		KC_P0, KC_P0, KC_PDOT,      KC_ENT
)

};

// Set led state during powerup
void keyboard_post_init_user(void) {
    writePinHigh(LED_RED);
}

bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);
    if(res) {
        writePin(LED_GREEN, led_state.num_lock);
    }
    return res;
}

void encoder_update_user(uint8_t index, bool clockwise) {
  if (index == 0) {         // First encoder - top left
    if (clockwise) {
      tap_code(KC_UP);
    } else {
      tap_code(KC_DOWN);
    }
  } else if (index == 1) {  // Second encoder - top right
    if (clockwise) {
      tap_code(KC_UP);
    } else {
      tap_code(KC_DOWN);
    }
  }
}
