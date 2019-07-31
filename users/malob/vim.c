// WIP implementation of Vim commands using QMK macros on macOS
#include "vim.h"

/** Current Vim mode, must be a value from the `vim_modes` enum. */
uint8_t vim_mode = INSERT;

/** Whether find should search forward or backward. */
bool vim_find_forward = true;

/** Whether put should paste incline or on newline. */
bool vim_put_inline = true;

/** Hack to add mods currently pressed to keycode */
uint16_t add_mods_pressed(uint16_t kc) {
  return add_shift_if_pressed(add_ctlr_if_pressed(add_gui_if_pressed(add_alt_if_pressed(kc))));
}

//Helper functions for the above
uint16_t add_shift_if_pressed(uint16_t kc) { return (get_mods() & MOD_MASK_SHIFT) ? S(kc) : kc; }
uint16_t add_ctlr_if_pressed(uint16_t kc)  { return (get_mods() & MOD_MASK_CTRL)  ? C(kc) : kc; }
uint16_t add_gui_if_pressed(uint16_t kc)   { return (get_mods() & MOD_MASK_GUI)   ? G(kc) : kc; }
uint16_t add_alt_if_pressed(uint16_t kc)   { return (get_mods() & MOD_MASK_ALT)   ? A(kc) : kc; }

/**
 * This function should be called from `process_record_user()`, to enable simulating Vim commands.
 * It's return value follows QMKs convention for `proces_record_*` functions, i.e., it returns
 * `true` signal that QMK should continue to process the key event as normal, and returns `false`
 * otherwise.
 *
 * If `vim_mode` is `INSERT` then function returns `true` indicating that QMK should process the
 * keycode event as it normally would. Otherwise if uses `keycode` and `vim_mode` to determine which
 * Vim command to execute.
 *
 * If keycode does not have a corresponding macro in the current state, the state is set to `NORMAL`
 * and no action is performed.
 *
 * @param `keycode`: a QMK keycode (uint16_t)
 * @param `*record`: a pointer to the `keyrecord_t` struct.
 * @return `bool`
 */
bool process_record_vim(uint16_t kc, keyrecord_t *record) {

  if (vim_mode == INSERT) { return true; }

  // Filter layer/mod tap bits off
  if ((kc >= QK_MOD_TAP && kc <= QK_MOD_TAP_MAX) || (kc >= QK_LAYER_TAP && kc <= QK_LAYER_TAP_MAX)) {
    kc = kc & 0xFF;
  }

  // TODO: this filter is probably not sufficient
  if ( record->event.pressed
    && ( (kc >= KC_A    && kc <= KC_SLASH)
      || (kc >= KC_HOME && kc <= KC_UP)
      || (kc >= S(KC_A) && kc <= S(KC_SLASH))
      || (kc >= C(KC_A) && kc <= C(KC_SLASH))
       )
     )
  {

    // Add mods to so we can match on them.
    kc = add_mods_pressed(kc);

    // Save modes and clear them so they aren't help when we send Vim commands.
    uint8_t current_mods = get_mods();
    clear_mods();

    // Set correct paste direction for when `vim_mode` isn't `NORMAL`.
    // This is cumbersome, but less so then setting it manually in all the modes for each keycode.
    if (vim_mode >= NORM_c && vim_mode <= NORM_yi) {
      if (kc == KC_C || kc == KC_D || kc == KC_G || kc == S(KC_G) || kc == KC_J || kc == KC_K || kc == KC_Y) {
        vim_put_inline = false;
      } else {
        vim_put_inline = true;
      }
    }

    // Deselect highlighted char
    VIM_h;

    // Execute Vim command or mode change based on keycode.
    switch (vim_mode) {

      // Normal state
      case NORMAL:
        switch (kc) {
          case KC_A:    VIM_TO_INSERT(VIM_a);           break;
          case S(KC_A): VIM_TO_INSERT(VIM_A);           break;
          case KC_B:    VIM_b;                          break;
          case KC_C:    vim_mode = NORM_c;              break;
          case S(KC_C): VIM_TO_INSERT(VIM_C);
                        vim_put_inline = true;          break;
          case KC_D:    vim_mode = NORM_d;              break;
          case S(KC_D): VIM_D;
                        vim_put_inline = true;          break;
          case KC_E:    VIM_e;                          break;
          case KC_F:    vim_mode = NORM_f;              break;
          case S(KC_F): vim_mode = NORM_F;              break;
          case KC_G:    vim_mode = NORM_g;              break;
          case S(KC_G): VIM_G;                          break;
          case KC_H:    VIM_h;                          break;
          case KC_I:    vim_mode = INSERT;              break;
          case S(KC_I): VIM_TO_INSERT(VIM_I);           break;
          case KC_J:    VIM_j;                          break; // actually behaves like `gj`
          case S(KC_J): VIM_J;                          break;
          case KC_K:    VIM_k;                          break; // actually behaves like `gk`
          case S(KC_K): VIM_K;                          break;
          case KC_L:    VIM_l;                          break;
          case KC_N:    (vim_find_forward)
                        ? VIM_n : VIM_N;                break;
          case S(KC_N): (vim_find_forward)
                        ? VIM_N : VIM_n;                break;
          case KC_O:    VIM_TO_INSERT(VIM_o);           break;
          case S(KC_O): VIM_TO_INSERT(VIM_0);           break;
          case KC_P:    if (vim_put_inline) {
                          VIM_p_INLINE;
                        } else {
                          VIM_p_NEWLINE;
                        }                               break;
          case S(KC_P): if (vim_put_inline) {
                          VIM_P_INLINE;
                        } else {
                          VIM_P_NEWLINE;
                        }                               break;
          case KC_R:    vim_mode = NORM_r;              break;
          case S(KC_R): vim_mode = REPLACE;             break;
          case C(KC_R): VIM_CTRLr;                      break; // moves back one space if nothing to redo
          case KC_S:    VIM_TO_INSERT(VIM_s);
                        vim_put_inline = true;          break;
          case S(KC_S): VIM_TO_INSERT(VIM_S);
                        vim_put_inline = true;          break;
          case KC_T:    vim_mode = NORM_t;              break;
          case S(KC_T): vim_mode = NORM_T;              break;
          case KC_U:    VIM_u;                          break; // moves back one space if nothing to undo
        //case KC_V     vim_mode = VISUAL;              break;
        //case S(KC_V)  vim_mode = VIS_LN:              break;
          case KC_W:    VIM_w;                          break;
          case KC_X:    VIM_x;
                        vim_put_inline = true;          break;
          case S(KC_X): VIM_X;
                        vim_put_inline = true;          break;
          case KC_Y:    vim_mode = NORM_y;              break;
          case S(KC_Y): VIM_Y;                          break;
                        vim_put_inline = false;         break;
          case KC_0:    VIM_0;                          break;
          case KC_DLR:  VIM_DOLLAR;                     break;
          case KC_HASH: VIM_HASH;
                        vim_find_forward = false;       break;
          case KC_ASTR: VIM_ASTERISK;
                        vim_find_forward = true;        break;
          case KC_MINS: VIM_MINUS;                      break;
          case KC_PLUS: VIM_PLUS;                       break;
          case KC_BSPC: VIM_BSPC;                       break;
          case KC_DEL:  VIM_DELETE;                     break;
          case KC_ENT:  VIM_ENTER;                      break;
        //case KC_SCLN
        //case KC_COLN
        //case KC_COMM
        //case KC_LT
        //case KC_RT
          case KC_SLSH:
                        VIM_TO_INSERT(VIM_SLASH);
                        vim_find_forward = true;        break;
          case KC_QUES: VIM_TO_INSERT(VIM_QUES);
                        vim_find_forward = false;       break;
          case KC_SPC:  VIM_SPACE;                      break;
          case KC_LEFT: VIM_LEFT;                       break;
          case KC_UP:   VIM_UP;                         break;
          case KC_DOWN: VIM_DOWN;                       break;
          case KC_RGHT: VIM_RIGHT;                      break;
          default:      vim_mode = NORMAL;
        }
        break;


      // Change states
      case NORM_c:
        switch (kc) {
          case KC_A:    vim_mode = NORM_ca;         break;
          case KC_B:    VIM_TO_INSERT(VIM_cb);      break;
          case KC_C:    VIM_TO_INSERT(VIM_cc);      break; // does not preserve indent
          case KC_E:    VIM_TO_INSERT(VIM_ce);      break;
          case KC_G:    vim_mode = NORM_cg;         break;
          case S(KC_G): VIM_TO_INSERT(VIM_cG);      break;
          case KC_H:    VIM_TO_INSERT(VIM_ch);      break;
          case KC_I:    vim_mode = NORM_ci;         break;
          case KC_J:    VIM_TO_INSERT(VIM_cj);      break;
          case KC_K:    VIM_TO_INSERT(VIM_ck);      break;
          case KC_L:    VIM_TO_INSERT(VIM_cj);      break;
          case KC_W:    VIM_TO_INSERT(VIM_cw);      break;
          case KC_0:    VIM_TO_INSERT(VIM_c0);      break;
          case KC_DLR:  VIM_TO_INSERT(VIM_cDOLLAR); break;
          default:      vim_mode = NORMAL;
        }
        break;
                                                    break;
      case NORM_ca:
        switch (kc) {
          case KC_W:    VIM_TO_INSERT(VIM_caw);     break;
          default:      vim_mode = NORMAL;
        }
        break;
                                                    break;
      case NORM_cg:
        switch (kc) {
          case KC_G:    VIM_TO_INSERT(VIM_cgg);     break;
          default:      vim_mode = NORMAL;
        }
        break;

      case NORM_ci:
        switch (kc) {
          case KC_W:    VIM_TO_INSERT(VIM_ciw);     break;
          default:      vim_mode = NORMAL;
        }
        break;


      // Delete states
      case NORM_d:
        switch (kc) {
          case KC_A:    vim_mode = NORM_da;         break;
          case KC_B:    VIM_TO_NORMAL(VIM_db);      break;
          case KC_D:    VIM_TO_NORMAL(VIM_dd);      break;
          case KC_E:    VIM_TO_NORMAL(VIM_de);      break;
          case KC_G:    vim_mode = NORM_dg;         break;
          case S(KC_G): VIM_TO_NORMAL(VIM_dG);      break;
          case KC_H:    VIM_TO_NORMAL(VIM_dh);      break;
          case KC_I:    vim_mode = NORM_di;         break;
          case KC_J:    VIM_TO_NORMAL(VIM_dj);      break;
          case KC_K:    VIM_TO_NORMAL(VIM_dk);      break;
          case KC_L:    VIM_TO_NORMAL(VIM_dl);      break;
          case KC_W:    VIM_TO_NORMAL(VIM_dw);      break;
          case KC_0:    VIM_TO_NORMAL(VIM_d0);      break;
          case KC_DLR:  VIM_TO_NORMAL(VIM_dDOLLAR); break;
          default:      vim_mode = NORMAL;
        }
        break;
                                                    break;
      case NORM_da:
        switch (kc) {
          case KC_W:    VIM_TO_NORMAL(VIM_daw);     break;
          default:      vim_mode = NORMAL;
        }
        break;

      case NORM_dg:
        switch (kc) {
          case KC_G:    VIM_TO_NORMAL(VIM_dgg);     break;
          default:      vim_mode = NORMAL;
        }
        break;
                                                    break;
      case NORM_di:
        switch (kc) {
          case KC_W:    VIM_TO_NORMAL(VIM_diw);     break;
          default:      vim_mode = NORMAL;
        }
        break;


      // Yank states
      case NORM_y:
        switch (kc) {
          case KC_A:    vim_mode = NORM_ya;         break;
          case KC_B:    VIM_TO_NORMAL(VIM_yb);      break;
          case KC_E:    VIM_TO_NORMAL(VIM_ye);      break;
          case KC_G:    vim_mode = NORM_yg;         break;
          case S(KC_G): VIM_TO_NORMAL(VIM_yG);      break;
          case KC_H:    VIM_TO_NORMAL(VIM_yh);      break;
          case KC_I:    vim_mode = NORM_yi;         break;
          case KC_J:    VIM_TO_NORMAL(VIM_yj);      break;
          case KC_K:    VIM_TO_NORMAL(VIM_yk);      break;
          case KC_L:    VIM_TO_NORMAL(VIM_yl);      break;
          case KC_W:    VIM_TO_NORMAL(VIM_yw);      break;
          case KC_Y:    VIM_TO_NORMAL(VIM_yy);      break;
          case KC_0:    VIM_TO_NORMAL(VIM_y0);      break;
          case KC_DLR:  VIM_TO_NORMAL(VIM_yDOLLAR); break;
          default:      vim_mode = NORMAL;
        }
        break;

      case NORM_ya:
        switch (kc) {
          case KC_W:    VIM_TO_NORMAL(VIM_yaw); break;
          default:      vim_mode = NORMAL;
        }
        break;

      case NORM_yg:
        switch (kc) {
          case KC_G:    VIM_TO_NORMAL(VIM_ygg); break;
          default:      vim_mode = NORMAL;
        }
        break;

      case NORM_yi:
        switch (kc) {
          case KC_W:    VIM_TO_NORMAL(VIM_yiw); break;
          default:      vim_mode = NORMAL;
        }
        break;


      // To char states
      // TODO: reimplement
      // case NORM_f:
      //   vim_to_char(kc); vim_mode = NORMAL ;
      //   return false;
      // case NORM_F:
      //   vim_to_char_back(kc); vim_mode = NORMAL ;
      //   return false;
      // case NORM_t:
      //   vim_till_char(kc); vim_mode = NORMAL ;
      //   return false;
      // case NORM_T:
      //   vim_till_char_back(kc); vim_mode = NORMAL ;
      //   return false;


      // Goto states
      case NORM_g:
        switch (kc) {
          case KC_E: VIM_TO_NORMAL(VIM_ge); break;
          case KC_G: VIM_TO_NORMAL(VIM_gg); break;
          case KC_J: VIM_TO_NORMAL(VIM_gj); break;
          case KC_K: VIM_TO_NORMAL(VIM_gk); break;
          default:   vim_mode = NORMAL;
        }
        break;


      // Replace states
      case NORM_r:
        switch (kc) {
          case KC_ESC: vim_mode = NORMAL;    break;
          default: VIM_REPLACE(kc);
                   VIM_TO_NORMAL(VIM_h);
        }
        break;

      case REPLACE:
        switch (kc) {
          case KC_ESC: VIM_TO_NORMAL(VIM_h); break;
          case KC_ENT: TAP(KC_ENTER);        break;
          default:     VIM_REPLACE(kc);

        }
        break;


      // this is never get triggered unless someone sets the state to something invalid
      default: vim_mode = NORMAL ;
    }

    // Select character right of cursor to simulate Vim cursor
    VIM_SELECT_MOVE(VIM_l);

    // Re-enable mods
    set_mods(current_mods);

    return false;
  }

  // Need to return true so we capture mods and layer keys being pressed etc.
  return true;
}

// void vim_till_to_find(uint16_t kc) {
//   vim_find();
//   tap(kc);
//   wait_ms(VIM_TO_TILL_WAIT*2);
// };
//
// void vim_till_char(uint16_t kc) {
//   vim_to_char(kc);
//   vim_left();
// }
//
// void vim_till_char_back(uint16_t kc) {
//   vim_to_char_back(kc);
//   vim_right();
// }
//
// void vim_to_char(uint16_t kc) {
//   vim_till_to_find(kc);
//   tap(KC_ESCAPE);
//   vim_left();
// }
//
// void vim_to_char_back(uint16_t kc) {
//   vim_till_to_find(kc);
//   vim_find_prev();
//   tap(KC_ESCAPE);
//   vim_left();
// }
