/* Copyright 2017 Baris Tosun
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
#include QMK_KEYBOARD_H

#define _______ KC_TRNS
#define XXXXXXX KC_NO

enum katana_layers {
  _COLEMAK,
  _QWERTY, 
  _NUMB,
  _SYMB,
  _M_EXT, /* mac */
  _W_EXT /* win/linux */
};

enum katana_keycodes {
  COLEMAK = SAFE_RANGE,
  QWERTY
};

#define K_SPCFN LT(_SYMB, KC_SPACE) // Tap for space, hold for symbols layer
#define K_BSPFN LT(_SYMB, KC_BSPC)  // Tap for backspace, hold for symbols layer
#define LEFT_MOD MT(MOD_RGUI, KC_LEFT)
#define DOWN_MOD MT(MOD_RALT, KC_DOWN)
#define UP_MOD MT(MOD_RCTL, KC_UP)
//linux/win
#define W_UNDO    LCTL(KC_Z)         // UNDO
#define W_CUT     LCTL(KC_X)         // CUT
#define W_COPY    LCTL(KC_C)         // COPY
#define W_PASTE   LCTL(KC_V)         // PASTE
#define W_PRVWD LCTL(KC_LEFT)   // Previous word
#define W_NXTWD LCTL(KC_RIGHT)  // Next word
#define W_LSTRT KC_HOME         // Start of line
#define W_LEND KC_END           // End of line
//mac
#define M_UNDO LGUI(KC_Z)            // UNDO
#define M_CUT LGUI(KC_X)             // CUT
#define M_COPY LGUI(KC_C)            // COPY
#define M_PASTE LGUI(KC_V)           // PASTE
#define M_PRVWD LALT(KC_LEFT)      // Previous word
#define M_NXTWD LALT(KC_RIGHT)     // Next word
#define M_LSTRT LGUI(KC_LEFT)      // Start of line
#define M_LEND LGUI(KC_RIGHT)      // End of line



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_COLEMAK] = LAYOUT(
  KC_ESC,    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,   TG(_NUMB),KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,
  KC_TAB,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,    KC_LBRC,          KC_RBRC, KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_BSPC,
  MO(_M_EXT),KC_A,    KC_R,    KC_S,    KC_T,    KC_D,    KC_MINS,          KC_QUOT, KC_H,    KC_N,    KC_E,    KC_I,    KC_O,    KC_ENT,
  KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_EQL,  KC_DEL,  KC_BSLS, KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  MO(_SYMB), KC_LCTL, KC_LALT, KC_LGUI,                   K_BSPFN, KC_ENT,  KC_SPACE,         LEFT_MOD,DOWN_MOD,UP_MOD,  KC_RIGHT,MO(_SYMB)
),
[_QWERTY] = LAYOUT(
  KC_ESC,   KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,   TG(_NUMB),KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,
  KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_LBRC,          KC_RBRC, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
  MO(_M_EXT), KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_MINS,          KC_QUOT, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
  KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_EQL,  KC_DEL,  KC_BSLS, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  MO(_SYMB),KC_LCTL, KC_LALT, KC_LGUI,                   K_BSPFN, KC_ENT,  KC_SPACE,         LEFT_MOD,DOWN_MOD,UP_MOD,   KC_RIGHT,MO(_SYMB)
),
[_NUMB] = LAYOUT(
  _______, _______, _______, _______, _______, _______, _______, _______, KC_PSLS, KC_PAST, KC_PMNS, _______, _______, _______, _______,
  _______, _______, _______, KC_MS_U, _______, _______, _______,          KC_7,    KC_8,    KC_9,    KC_PPLS, _______, _______, _______,
  _______, KC_BTN2, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______,          KC_4,    KC_5,    KC_6,    KC_PPLS, _______, _______, _______,
  _______, _______, KC_ACL0, KC_ACL1, KC_ACL2, _______, _______, _______, KC_1,    KC_2,    KC_3,    KC_PENT, _______, _______, _______,
  _______, _______, _______, _______,                   KC_BTN1, _______, KC_P0,            KC_PDOT, KC_PENT, _______, _______, _______
),
[_SYMB] = LAYOUT(
  RESET,   _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    _______,          _______, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
  _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_VOLD,          KC_VOLU, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, _______,
  _______, KC_PLUS, KC_MINS, KC_EQL,  KC_LCBR, KC_RCBR, KC_MPRV, KC_MPLY, KC_MNXT, KC_LBRC, KC_RBRC, KC_SCLN, KC_COLN, KC_BSLS, _______,
  _______, _______, _______, _______,                   KC_DEL,  _______, _______,          _______, _______, _______, _______, _______
),
[_M_EXT] = LAYOUT(
  RESET  , COLEMAK, QWERTY , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, KC_WH_U, _______, _______, _______, _______,          _______, KC_PGUP, M_PRVWD, KC_UP,   M_NXTWD, _______, _______,
  _______, KC_LCTL, KC_WH_D, KC_LSFT, _______, KC_CAPS, _______,          _______, KC_PGDN, KC_LEFT, KC_DOWN, KC_RIGHT,KC_DEL,  _______,
  _______, M_UNDO,  M_CUT,   M_COPY,  M_PASTE, _______, _______, _______, _______, _______, M_LSTRT, _______, M_LEND,  _______, _______,
  _______, _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
),
[_W_EXT] = LAYOUT(
  RESET  , COLEMAK, QWERTY , _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, KC_WH_U, _______, _______, _______, _______,          _______, KC_PGUP, W_PRVWD, KC_UP,   W_NXTWD, _______, _______,
  _______, KC_LCTL, KC_WH_D, KC_LSFT, _______, KC_CAPS, _______,          _______, KC_PGDN, KC_LEFT, KC_DOWN, KC_RIGHT,KC_DEL,  _______,
  _______, W_UNDO,  W_CUT,   W_COPY,  W_PASTE, _______, _______, _______, _______, _______, W_LSTRT, _______, W_LEND,  _______, _______,
  _______, _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
)

};

void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case COLEMAK:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_COLEMAK);
      }
      return false;
      break;
  }
  return true;
}

void led_set_user(uint8_t usb_led) {

}
