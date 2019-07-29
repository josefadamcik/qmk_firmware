# Setup Instructions

* Copy `leader.c` and `leader.h` into your keymap folder.
* Add `SRC += leader.c` in the `rules.mk` file in your keymap folder.
* In your `keymap.c` file:
  * add `#include leader.h` with other includes at the top of the file.
  * put `LEAD_EXTERNS();` below includes at the top of the file;
  * add a leader keycode to your layout, e.g., `LEAD`;
  * below where you've defined the leader keycode, add `uint16_t leader_keycode = LEAD` (where `LEAD` is whatever you defined your leader keycode to be);
  * define the function `bool check_leaders(void)` in which you'll define your leader sequences using provided macros (details below), e.g.:
    ```c
    bool check_leaders(void) {
      // Spaces
      LEAD2_TAP(KC_T, KC_H, C(KC_LEFT));  // Move one space to the left
      LEAD2_TAP(KC_T, KC_L, C(KC_RIGHT)); // Move one space to the right

      // Move tab to next Safari window
      LEAD4(KC_A, KC_S, KC_T, KC_N) {
        tap_code16(G(KC_L));
        tap_code16(G(KC_C));
        tap_code16(G(KC_W));
        tap_code16(G(KC_GRAVE));
        wait_ms(500);
        tap_code16(G(KC_T));
        tap_code16(G(KC_V));
        tap_code16(KC_ENTER);
        return false;
      }
      return true;
    }
    ```
  * then include the following in your `process_record_user()` function:
    ```c
      if (!process_leader(keycode, record)) { return false; }
    ```
# Leader Sequence Macros

Leader sequence can be up to five keycodes long. They can be defined using two types of macros:

  * `LEADX_TAP`, which lets you specify the keycodes of the leader sequence along with a keycode to sent if the sequence is executed.
  * `LEADX`, which lets you specify the keycodes of the leader sequence along with arbitrary code that will be executed if the sequence is executed.

(Where `X` is the number of keycodes in the sequence. See examples of both in use above. Note that for `LEADX` style macros, if you want to terminate leader mode you must explicitly include `return false`.)
