// vim: foldmethod=marker
#include "vim.h"

// Vim helpers {{{

uint8_t _vim_state = INSERT;
uint8_t _vim_find_direction = FORWARD;

/**
 * Retrives the currents `vim_state`.
 * @return `vim_state`: a `uint8_t` defined in `vim_states enum`
 * @see `vim_set_state()`
 */
uint8_t vim_current_state(void)  { return _vim_state; }

/**
 * Sets `vim_state`.
 * @param `vim_state`: a `uint8_t` defined in `vim_states enum`
 * @see `vim_current_state()`
 */
void vim_set_state(uint8_t s) { _vim_state = s; }

uint8_t vim_find_direction(void) { return _vim_find_direction; }

void vim_set_find_direction(uint8_t d) { _vim_find_direction = d; }

uint16_t add_shift_if_pressed(uint16_t kc) { return (get_mods() & MOD_MASK_SHIFT) ? S(kc) : kc; }
uint16_t add_ctlr_if_pressed(uint16_t kc) { return (get_mods() & MOD_MASK_CTRL) ? C(kc) : kc; }
uint16_t add_gui_if_pressed(uint16_t kc) { return (get_mods() & MOD_MASK_GUI) ? G(kc) : kc; }
uint16_t add_alt_if_pressed(uint16_t kc) { return (get_mods() & MOD_MASK_ALT) ? A(kc) : kc; }
uint16_t add_mods_pressed(uint16_t kc) {
  return add_shift_if_pressed(add_ctlr_if_pressed(add_gui_if_pressed(add_alt_if_pressed(kc))));
}

#ifndef VIM_TO_TILL_WAIT
  #define VIM_TO_TILL_WAIT 1000
#endif
// }}}

// Vim Mode keycode processing {{{

/**
 * This function should be called from `process_record_user()`, to enable simunating Vim commands.
 * It's return value follows QMKs convention for `proces_record_*` functions, i.e., it returns
 * `true` signal that QMK should continue to process the key event as normal, and returns
 * `false` otherwise.
 *
 * @param `kc`: a QMK keycode (uint16_t)
 * @param `*record`: a pointer to the `keyrecord_t` struct.
 * @return `bool`
 * @see `vim_exec_macro()`
 */
bool process_record_vim(uint16_t kc, keyrecord_t *record) {
  if (vim_current_state() == INSERT) { return true; }

  switch (kc) {
    case KC_A ... KC_SLASH:
      if (record->event.pressed) { return vim_exec_macro(kc); }
      return true;

    case KC_HOME ... KC_UP:
      if (record->event.pressed) { return vim_exec_macro(kc); }
      return true;

    default:
      return true;
  }
}

/**
 * Manages `_vim_state` and executes Vim command macros based on normal keycode.
 *
 * If the state is `INSERT` then function returns `true` indicating that QMK should process the
 * keycode event as is normally would, unless the keycode was `VIM` in which case the state is
 * changed to normal.
 *
 * Otherwise, if keycode does not have a corresponding macro in the current state, the state is set
 * to `NORMAL` and no action is performed.
 *
 * @param `kc`: a QMK keycode (uint16_t)
 * @return `bool`
 * @see `vim_cmd()` for details on how macro functions are executed.
 */
bool vim_exec_macro(uint16_t keycode) {

  uint16_t kc = add_mods_pressed(keycode);

  switch (vim_current_state()) {

    case INSERT:
    // Normal state
    case NORMAL:
      switch (kc) {
        case KC_A:    vim_cmd(vim_right           , INSERT); break;
        case S(KC_A): vim_cmd(vim_line_end        , INSERT); break;
        case KC_B:    vim_cmd(vim_word_back       , NORMAL); break;
        case KC_C:    vim_set_state(                NORM_c); break;
        case S(KC_C): vim_cmd(vim_chg_line        , INSERT); break;
        case KC_D:    vim_set_state(                NORM_d); break;
        case S(KC_D): vim_cmd(vim_del_to_line_end , NORMAL); break;
        case KC_E:    vim_cmd(vim_word_end        , NORMAL); break;
        case KC_F:    vim_set_state(                NORM_f); break;
        case S(KC_F): vim_set_state(                NORM_F); break;
        case KC_G:    vim_set_state(                NORM_g); break;
        case S(KC_G): vim_cmd(vim_goto_line_last  , NORMAL); break;
        case KC_H:    vim_cmd(vim_left            , NORMAL); break;
        case KC_I:    vim_set_state(                INSERT); break;
        case S(KC_I): vim_cmd(vim_line_first      , INSERT); break;
        case KC_J:    vim_cmd(vim_down            , NORMAL); break; // actually behaves like `gj`
      //case S(KC_J): vim_cmd(vim_join_line       , NORMAL); break;
        case KC_K:    vim_cmd(vim_up              , NORMAL); break; // actually behaves like `gk`
        case S(KC_K): vim_cmd(vim_lookup          , NORMAL); break;
        case KC_L:    vim_cmd(vim_right           , NORMAL); break;
        case KC_N:    (vim_find_direction() ==      FORWARD)
                    ? vim_cmd(vim_find_next       , NORMAL)
                    : vim_cmd(vim_find_prev       , NORMAL); break;
        case S(KC_N): (vim_find_direction() ==      BACKWARD)
                    ? vim_cmd(vim_find_prev       , NORMAL)
                    : vim_cmd(vim_find_next       , NORMAL); break;
        case KC_O:    vim_cmd(vim_open            , INSERT); break;
        case S(KC_O): vim_cmd(vim_open_above      , INSERT); break;
        case KC_P:    vim_cmd(vim_put             , NORMAL); break;
      //case S(KC_P)  vim_cmd(vim_put_above       , NORMAL); break;
        case KC_R:    vim_cmd(vim_replace_start   , NORM_r); break;
        case S(KC_R): vim_cmd(vim_replace_start   , REPLACE);break;
        case C(KC_R): vim_cmd(vim_redo            , NORMAL); break;
        case KC_S:    vim_cmd(vim_chg_right       , INSERT); break;
        case S(KC_S): vim_cmd(vim_chg_line        , INSERT); break;
        case KC_T:    vim_set_state(                NORM_t); break;
        case S(KC_T): vim_set_state(                NORM_T); break;
        case KC_U:    vim_cmd(vim_undo            , NORMAL); break;
      //case KC_V     vim_set_state(                VISUAL); break;
      //case S(KC_V)  vim_set_state(                VIS_LN): break;
        case KC_W:    vim_cmd(vim_word            , NORMAL); break;
        case KC_X:    vim_cmd(vim_chg_right       , NORMAL); break;
        case S(KC_X): vim_cmd(vim_chg_left        , NORMAL); break;
        case KC_Y:    vim_set_state(                NORM_y); break;
        case S(KC_Y): vim_cmd(vim_yank_line       , NORMAL); break;
        case KC_0:    vim_cmd(vim_line_first      , NORMAL); break;
        case KC_DLR:  vim_cmd(vim_line_end        , NORMAL); break;
        case KC_HASH: vim_set_find_direction(       BACKWARD);
                      vim_cmd(vim_find_word       , NORMAL); break;
        case KC_ASTR: vim_set_find_direction(       FORWARD);
                      vim_cmd(vim_find_word       , NORMAL); break;
        case KC_MINS: vim_cmd(vim_line_up         , NORMAL); break;
        case KC_PLUS: vim_cmd(vim_line_down       , NORMAL); break;
        case KC_BSPC: vim_cmd(vim_left            , NORMAL); break;
        case KC_DEL:  vim_cmd(vim_chg_right       , NORMAL); break;
        case KC_ENT:  vim_cmd(vim_line_down       , NORMAL); break;
      //case KC_SCLN
        case KC_COLN: vim_cmd(vim_command         , INSERT); break;
      //case KC_COMM
      //case KC_LT
      //case KC_RT
        case KC_SLSH: vim_set_find_direction(       FORWARD);
                      vim_cmd(vim_find            , INSERT); break;
        case KC_QUES: vim_set_find_direction(       BACKWARD);
                      vim_cmd(vim_find            , INSERT); break;
        case KC_SPC:  vim_cmd(vim_right           , NORMAL); break;
        case KC_LEFT: vim_cmd(vim_left            , NORMAL); break;
        case KC_UP:   vim_cmd(vim_up              , NORMAL); break;
        case KC_DOWN: vim_cmd(vim_down            , NORMAL); break;
        case KC_RGHT: vim_cmd(vim_right           , NORMAL); break;
        default:      vim_set_state(                NORMAL);
        // Not possible because we can't do Word movements in macOS
        // case S(KC_B)
        // case S(KC_E)
        // case S(KC_W)
        // Not possible because we can't move relative to what's vibible in the window
        // case S(KC_H)
        // case S(KS_L)
        // case S(KC_M)
        // Not possible because no way to do mark stuff
        // case KC_M
        // case KC_GRV
        // case KC_QUOT
        // Not possible for other reasons
        // case S(KC_Q) no Ex mode
        // case S(KC_U)
        // case KC_TILD no way to toggle case
        // case KC_PERC can't know enough context of surounding delimeters
        // case KC_CIRC can't can only go to begining of line inclusive of whitespace
        // case KC_AMPR can't guarentee that last substitution will rememberable
        // case KC_BSLS not a thing
        // case KC_DQUO no concept of registers
        // case KC_DOT no way to reliably track previous insertion
        // Would be pretty buggy
        // case KC_RPRN/KC_LPRN could probably hack it with find macro like t/T and f/F
        // case KC_RCBR/KC_RCBR ditto
        // Really ambitious so won't work on any time soon
        // case KC_Q could track be used to create QMK dynamic macros
        // case KC_Z can't to a lot of that stuff but might be able to do some
        // case KC_EXLM probably somesthing possible but would depend on system configuration
        // case KC_AT macros again
        // case KC_UNDS needs count movements to be useful
        // case KC_RBRC/KC_LBRC needs a whole new mode with lots of hard commands
        // case KC_PIPE needs count to be useful
      }
      return false;

    // Change states
    case NORM_c:
      switch (kc) {
        case KC_A:    vim_set_state(                NORM_ca);break;
        case KC_B:    vim_cmd(vim_chg_word_back   , INSERT); break;
        case KC_C:    vim_cmd(vim_chg_line        , INSERT); break; // does not preserve indent
        case KC_E:    vim_cmd(vim_chg_word_end    , INSERT); break;
        case KC_H:    vim_cmd(vim_chg_left        , INSERT); break;
        case KC_I:    vim_set_state(                NORM_ci);break;
        case KC_J:    vim_cmd(vim_chg_down        , INSERT); break;
        case KC_K:    vim_cmd(vim_chg_up          , INSERT); break;
        case KC_L:    vim_cmd(vim_chg_right       , INSERT); break;
        case KC_W:    vim_cmd(vim_chg_word        , INSERT); break;
        case KC_0:    vim_cmd(vim_chg_to_line_fst , INSERT); break;
        case KC_DLR:  vim_cmd(vim_chg_to_line_end , INSERT); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_ca:
      switch (kc) {
        case KC_W:    vim_cmd(vim_chg_a_word,       INSERT); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_ci:
      switch (kc) {
        case KC_W:    vim_cmd(vim_chg_inner_word  , INSERT); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;

    // Delete states
    case NORM_d:
      switch (kc) {
        case KC_A:    vim_set_state(                NORM_da);break;
        case KC_B:    vim_cmd(vim_chg_word_back   , NORMAL); break;
        case KC_D:    vim_cmd(vim_del_line        , NORMAL); break;
        case KC_E:    vim_cmd(vim_chg_word_end    , NORMAL); break;
        case KC_H:    vim_cmd(vim_chg_left        , NORMAL); break;
        case KC_I:    vim_set_state(                NORM_di);break;
        case KC_J:    vim_cmd(vim_del_down        , NORMAL); break;
        case KC_K:    vim_cmd(vim_del_up          , NORMAL); break;
        case KC_L:    vim_cmd(vim_chg_right       , NORMAL); break;
        case KC_W:    vim_cmd(vim_chg_word        , NORMAL); break;
        case KC_0:    vim_cmd(vim_chg_to_line_fst , NORMAL); break;
        case KC_DLR:  vim_cmd(vim_del_to_line_end , NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_da:
      switch (kc) {
        case KC_W:    vim_cmd(vim_chg_a_word      , NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_di:
      switch (kc) {
        case KC_W:    vim_cmd(vim_chg_inner_word  , NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;

    // To char states
    case NORM_f:
      vim_to_char(kc); vim_set_state(               NORMAL);
      return false;
    case NORM_F:
      vim_to_char_back(kc); vim_set_state(          NORMAL);
      return false;
    case NORM_t:
      vim_till_char(kc); vim_set_state(             NORMAL);
      return false;
    case NORM_T:
      vim_till_char_back(kc); vim_set_state(        NORMAL);
      return false;

    // Goto states
    case NORM_g:
      switch (kc) {
        case KC_E:    vim_cmd(vim_word_back_end   , NORMAL); break;
        case KC_G:    vim_cmd(vim_goto_line_first , NORMAL); break;
        case KC_J:    vim_cmd(vim_down            , NORMAL); break;
        case KC_K:    vim_cmd(vim_up              , NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    // Not implemented
    // Cannot implement
    // `gw` `gww` etc. which formats
    // `gh` `gH` enters select mode which is not implemented
    // `ga` to print Ascii value of character under cursor
    // `gO` to show file specific outline

    // Yank states
    case NORM_y:
      switch (kc) {
        case KC_A:    vim_set_state(                NORM_ya);break;
        case KC_B:    vim_cmd(vim_yank_word_back  , NORMAL); break;
        case KC_E:    vim_cmd(vim_yank_word_end   , NORMAL); break;
        case KC_H:    vim_cmd(vim_yank_left       , NORMAL); break;
        case KC_I:    vim_set_state(                NORM_yi);break;
        case KC_J:    vim_cmd(vim_yank_down       , NORMAL); break;
        case KC_K:    vim_cmd(vim_yank_up         , NORMAL); break;
        case KC_L:    vim_cmd(vim_yank_right      , NORMAL); break;
        case KC_W:    vim_cmd(vim_yank_word       , NORMAL); break;
        case KC_Y:    vim_cmd(vim_yank_line       , NORMAL); break;
        case KC_0:    vim_cmd(vim_yank_to_line_fst, NORMAL); break;
        case KC_DLR:  vim_cmd(vim_yank_to_line_end, NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_ya:
      switch (kc) {
        case KC_W:    vim_cmd(vim_yank_a_word,      NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;
    case NORM_yi:
      switch (kc) {
        case KC_W:    vim_cmd(vim_yank_inner_word , NORMAL); break;
        default:      vim_set_state(                NORMAL);
      }
      return false;

    // Replace states
    case NORM_r:
      switch (kc) {
        case KC_ESC:  vim_cmd(vim_replace_end     , NORMAL); break;
        default:
          tap(kc);    vim_cmd(vim_replace_end     , NORMAL);
      }
      return false;
    case REPLACE:
      switch (kc) {
        case KC_ESC:  vim_cmd(vim_replace_end     , NORMAL); break;
        case KC_ENT:  vim_cmd(vim_replace_enter   , REPLACE);break;
        default:
          vim_replace(kc);
      }
      return false;

    // this is never get triggered unless someone sets the state to something invalid
    default:
      vim_set_state(NORMAL);
      return false;
  }
}

/**
 * Executes the a provided marco function and updates `vim_state`.
 *
 * Given that mods might be pressed that we don't want active while the macro is running, current
 * active mods are saved, then cleared while the macro is run, and then reset.
 *
 * @param `cmd`: a pointer to a macro function to execute
 * @param `new_state`: a `uint8_t` value defined in `vim_states enum`
 */
void vim_cmd(void (*cmd)(void), uint8_t new_state) {
  uint8_t current_mods = get_mods();
  clear_mods();
  (*cmd)();
  set_mods(current_mods);
  vim_set_state(new_state);
}
// }}}

// Vim macro helpers {{{

/**
 * Executes a Vim movement macro while holding shift to select text.
 * @param `*cmd`: pointer to macro function to execute
 */
void vim_select_move(void (*cmd)(void)) {
  press(KC_LSHIFT);
  (*cmd)();
  release(KC_LSHIFT);
}

/**
 * Executes a Vim movement macro,
 *   * while holding shift to select text, then
 *   * sending `⌘x` to cut the select text.
 * @param `*cmd`: pointer to macro function to execute
 */
void vim_cut_move(void (*cmd)(void)) {
  vim_select_move(cmd);
  tap(G(KC_X));
}

/**
 * Executes a Vim movement macro,
 *   * while holding shift to select text, then
 *   * sending `⌘c` to cut the select text.
 * @param `*cmd`: pointer to macro function to execute
 */
void vim_yank_move(void (*cmd)(void)) {
  vim_select_move(cmd);
  tap(G(KC_C));
}

// }}}

// Vim movement macros functions {{{

/**
 * Simulates Vim's `h` command by sending `←` to move the cursor left by one character.
 */
void vim_left(void) { tap(KC_LEFT); }

/**
 * Simulates Vim's `l` command by sending `→` to move the cursor right by one character.
 */
void vim_right(void) { tap(KC_RIGHT); }

/**
 * Simulates Vim's `k` and `gk` command by sending `↑` to move the cursor up one line.
 * (Where a line is a soft wrapped line, so it actually just behaves like `gk`.)
 */
void vim_up(void) { tap(KC_UP); }

/**
 * Simulates Vim's `j` and `gj` command by sending `↓` to move the cursor down one line.
 * (Where a line is a soft wrapped line, so it actually just behaves like `gj`.)
 */
void vim_down(void) { tap(KC_DOWN); }

/**
 * Simulates Vim's `b` command by sending `⌥←` to move the cursor to the beginning of the current
 * word.
 */
void vim_word_back(void) { tap( LALT(KC_LEFT) ); }

/**
 * Simulates Vim's `e` command by sending `⌥→` to move the cursor to the end of the current word.
 */
void vim_word_end(void) { tap( LALT(KC_RIGHT) ); }

/**
 * Simulates Vim's `ge` command by,
 *   * sending `⌥←` to move the cursor to the beginning of the current word, then
 *   * sending `⌥←` to move the cursor to the beginning of the previous work, and finally
 *   * sending `⌥→` to move the cursor to the end of that word.
 */
void vim_word_back_end(void) {
  vim_word_back();
  vim_word_back();
  vim_word_end();
}

/**
 * Simulates Vim's `w` command by,
 *   * sending `⌥→` to move the cursor to the end of the current word, then
 *   * sending `⌥→` to move the cursor to the end of the next word, and finally
 *   * sending `⌥←` to move the cursor to the beginning of that word.
 */
void vim_word(void) {
  vim_word_end();
  vim_word_end();
  vim_word_back();
}

/**
 * Simulates Vim's `0` command by sending `⌃a` to move the cursor to the beginning of the line.
 */
void vim_line_first(void) { tap( LCTL(KC_A) ); }

/**
 * Simulates Vim's `$` command by sending `⌃e` to move the cursor to the end of the line.
 */
void vim_line_end(void) { tap( LCTL(KC_DOWN) ); }

/**
 * Simulates Vim's `-` command by,
 *   * sending `⌃a` to move the cursor to the beginning of the line, then
 *   * sending `↑` to move the cursor up onto the line above, and finally
 *   * sending `⌃e` to move ensure the cursor to the beginning of that line.
 */
void vim_line_up(void) {
  vim_line_end();
  vim_down();
  vim_line_first();
}

/**
 * Simulates Vim's `+` and `⏎` commands by,
 *   * sending `⌃e` to move the cursor to the end of the line, then
 *   * sending `↑` to move up cursor onto the line above, and finally
 *   * sending `⌃a` to move ensure the cursor is at the beginning of that line.
 */
void vim_line_down(void) {
  vim_line_end();
  vim_down();
  vim_line_first();
}

/**
 * Simulates Vim's `gg` command by sending `⌘↑` to move the cursor the beginning of the first line.
 */
void vim_goto_line_first(void) { tap( LGUI(KC_UP) ); }

/**
 * Simulates Vim's `G` command by,
 * * sending `⌘↓` to move the cursor the end of the last line, then
 * * sending `⌃a` to move the cursor beginning of that line.
 */
void vim_goto_line_last(void) {
  tap( LGUI(KC_UP) );
  vim_line_first();
}
// }}}

// Vim insertion macro functions {{{

/**
 * Simulates Vim's `o` command by,
 *   * sending `⌃e` to move the cursor to the end of the line, then
 *   * sending `⏎` to insert a new line.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_open(void) {
  vim_line_end();
  tap(KC_ENTER);
}

/**
 * Simulates Vim's `O` command by,
 *   * sending `⌃a` to move the cursor to the start of the line, then
 *   * sending `⏎` to insert a new line, and finally
 *   * sending `↑` to move up to the newly create line.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_open_above(void) {
  vim_line_first();
  tap(KC_ENTER);
  tap(KC_UP);
}
// }}}

// Vim change macro functions {{{

/**
 * Simulates Vim's `ch` command by,
 *   * sending `⇧←` to select the character left of the cursor, then
 *   * sending `⌘x` to cut the select text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_left(void) { vim_cut_move(vim_left); }

/**
 * Simulates Vim's `cl` `s` command by,
 *   * sending `⇧→` to select the character right of the cursor, then
 *   * sending `⌘x` to cut the select text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_right(void) { vim_cut_move(vim_right); }

/**
 * Simulates Vim's `ck` command by,
 *   * sending `⌥↑` twice to move the cursor to the start of the line above, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the starting line, and finally
 *   * sending `⌘x` to cut the select text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_up(void) {
  vim_line_up();
  vim_select_move(vim_line_end);
  vim_line_down();
  vim_cut_move(vim_line_end);
}

/**
 * Simulates Vim's `cj` command by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the line below, and finally
 *   * sending `⌘x` to cut the select text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_down(void) {
  vim_line_first();
  vim_select_move(vim_line_end);
  vim_cut_move(vim_line_end);
}

/**
 * Simulates Vim's `cb` command by,
 *   * sending `⇧⌥←` to select the text until the beginning of the current word, then
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_word_back(void) { vim_cut_move(vim_word_back); }

/**
 * Simulates Vim's `ce` command by,
 *   * sending `⇧⌥→` to select the text until the end of current word, then
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_word_end(void) { vim_cut_move(vim_word_end); }

/**
 * Simulates Vim's `cw` command by,
 *   * pressing `⇧` to enable text selection, then
 *     * sending `⌥→` to cursor to the end of the current word, then
 *     * sending `⌥→` to move the cursor to the end of the next word, then
 *     * sending `⌥←` to move the cursor to the beginning of that word, then
 *   * releasing `⇧` to end text selection, and finally
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_word(void) { vim_cut_move(vim_word); }

/**
 * Simulates Vim's `caw` command by,
 *   * sending `⌥←` to move the cursor to the beginning of the word, then
 *   * pressing `⇧` to enable text selection, then
 *     * sending `⌥→` to cursor to the end of the current word, then
 *     * sending `⌥→` to move the cursor to the end of the next word, then
 *     * sending `⌥←` to move the cursor to the beginning of that word, then
 *   * releasing `⇧` to end text selection, and finally
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_a_word(void) {
  vim_word_back();
  vim_chg_word();
}

/**
 * Simulates Vim's `ciw` command by,
 *   * sending `⌥←` to move the cursor to the beginning of the word, then
 *   * sending `⇧⌥→` to select the text until the end of current word, and finally
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_inner_word(void) {
  vim_word_back();
  vim_chg_word_end();
}

/**
 * Simulates Vim's `c0` command by,
 *   * sending `⇧⌥↑` to select the text until the beginning of the line, then
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_to_line_fst(void) { vim_cut_move(vim_line_first); }

/**
 * Simulates Vim's `C` and `c$`command by,
 *   * sending `⇧⌥↓` to select the text until the end of the line, then
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_to_line_end(void) { vim_cut_move(vim_line_end); }

/**
 * Simulates Vim's `cc` and `S` commands by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` to select the text until the end of the line, and finally
 *   * sending `⌘x` to cut the selected text.
 * (Transition to insert mode handled by `vim_process_keycode()` function.)
 */
void vim_chg_line(void) {
  vim_line_first();
  vim_chg_to_line_end();
}
// }}}

// Vim find macro functions {{{

void vim_find(void) { tap( G(KC_F) ); }

void vim_find_next(void) { tap( G(KC_G) ); }

void vim_find_prev(void) { tap( S(G(KC_G)) ); }

void vim_find_word(void) {
  vim_word_end();
  vim_yank_inner_word();
  vim_find();
  tap( G(KC_V) );
}
// }}}

// Vim till/to macro functions {{{

void vim_till_to_find(uint16_t kc) {
  vim_find();
  tap(kc);
  wait_ms(VIM_TO_TILL_WAIT*2);
};

void vim_till_char(uint16_t kc) {
  vim_to_char(kc);
  vim_left();
}

void vim_till_char_back(uint16_t kc) {
  vim_to_char_back(kc);
  vim_right();
}

void vim_to_char(uint16_t kc) {
  vim_till_to_find(kc);
  tap(KC_ESCAPE);
  vim_left();
}

void vim_to_char_back(uint16_t kc) {
  vim_till_to_find(kc);
  vim_find_prev();
  tap(KC_ESCAPE);
  vim_left();
}

// }}}

// Vim deletion macro functions {{{

/**
 * Simulates Vim's `dk` command by,
 *   * sending `⌥↑` twice to move the cursor to the start of the line above, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the starting line, then
 *   * sending `⌘x` to cut the select text, and finally
 *   * sending `⌦` to delete the empty line.
 */
void vim_del_up(void) {
  vim_chg_up();
  tap(KC_DELETE);
}

/**
 * Simulates Vim's `dj` command by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the line below, then
 *   * sending `⌘x` to cut the select text, and finally
 *   * sending `⌦` to delete the empty line.
 */
void vim_del_down(void) {
  vim_chg_down();
  tap(KC_DELETE);
}

/**
 * Simulates Vim's `D` and `d$` command by,
 *   * sending `⇧⌥↓` to select the text until the end of the line, then
 *   * sending `⌘x` to cut the selected text, and finally
 *   * sending `←` to move the cursor one character to the left.
 */
void vim_del_to_line_end(void) {
  vim_chg_to_line_end();
  tap(KC_LEFT);
}

/**
 * Simulates Vim's `dd` command by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` to select the text until the end of the line, then
 *   * sending `⌘x` to cut the selected text, and finally
 *   * sending `⌦` to delete the now empty line.
 */
void vim_del_line(void) {
  vim_chg_line();
  tap(KC_DELETE);
}
// }}}

// Vim put macro functions {{{

/**
 * Simulates Vim's `p` command by sending `⌘v`.
 */
void vim_put(void) { tap(G(KC_V)); }
// }}}

// Vim replace macro functions {{{

/**
 * Simulates starting Vim's `r` command or entering REPLACE mode by,
 *   * sending `⇧→` to select the character right of the cursor.
 */
void vim_replace_start(void) { vim_select_move(vim_right); }

/** Simulates ending Vim's `r` command or exiting REPLACE mode by,
 *   * sending `←` to move the cursor back to it's orginal position.
 */
void vim_replace_end(void) { vim_left(); }

/**
 * Simulates pressing `⏎` while in Vim's REPLACE mode by,
 *   * sending `←` to deselect the currently selected character, then
 *   * sending `⏎` to create a new line, and finally
 *   * sending `⇧→` to select the character right of the cursor.
 */
void vim_replace_enter(void) {
  vim_replace_end();
  tap(KC_ENTER);
  vim_replace_start();
}

/**
 * Simulates typing a character while after starting Vim's `r` or while in REPLACE mode by,
 *   * sending `kc` to insert the typed chararter, then
 *   * sending `⇧→` to select the character right of the cursor.
 * @param `kc`: the QMK keycode to insert (`uint16_t`)
 */
void vim_replace(uint16_t kc) {
  tap(kc);
  vim_replace_start();
}
// }}}

// Vim yank macro functions {{{

/**
 * Simulates Vim's `yh` command by,
 *   * sending `⇧←` to select the character left of the cursor, then
 *   * sending `⌘c` to copy the select text.
 */
void vim_yank_left(void) { vim_yank_move(vim_left); }

/**
 * Simulates Vim's `yl` command by,
 *   * sending `⇧→` to select the character right of the cursor, then
 *   * sending `⌘c` to copy the select text.
 */
void vim_yank_right(void) { vim_yank_move(vim_right); }

/**
 * Simulates Vim's `yk` command by,
 *   * sending `⌥↑` twice to move the cursor to the start of the line above, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the starting line, and finally
 *   * sending `⌘c` to copy the select text.
 */
void vim_yank_up(void) {
  vim_line_first();
  vim_line_first();
  vim_select_move(vim_line_end);
  vim_yank_move(vim_line_end);
}

/**
 * Simulates Vim's `yj` command by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` twice to select the text until the end of the line below, and finally
 *   * sending `⌘c` to copy the select text.
 */
void vim_yank_down(void) {
  vim_line_first();
  vim_select_move(vim_line_end);
  vim_cut_move(vim_line_end);
}

/**
 * Simulates Vim's `yb` command by,
 *   * sending `⇧⌥←` to select the text until the beginning of the current word, then
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_word_back(void) { vim_yank_move(vim_word_back); }

/**
 * Simulates Vim's `ye` command by,
 *   * sending `⇧⌥→` to select the text until the end of current word, then
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_word_end(void) { vim_yank_move(vim_word_end); }

/**
 * Simulates Vim's `yw` command by,
 *   * pressing `⇧` to enable text selection, then
 *     * sending `⌥→` to cursor to the end of the current word, then
 *     * sending `⌥→` to move the cursor to the end of the next word, then
 *     * sending `⌥←` to move the cursor to the beginning of that word, then
 *   * releasing `⇧` to end text selection, and finally
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_word(void) { vim_yank_move(vim_word); }

/**
 * Simulates Vim's `yaw` command by,
 *   * sending `⌥←` to move the cursor to the beginning of the word, then
 *   * pressing `⇧` to enable text selection, then
 *     * sending `⌥→` to cursor to the end of the current word, then
 *     * sending `⌥→` to move the cursor to the end of the next word, then
 *     * sending `⌥←` to move the cursor to the beginning of that word, then
 *   * releasing `⇧` to end text selection, and finally
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_a_word(void) {
  vim_word_back();
  vim_yank_word();
}

/**
 * Simulates Vim's `yiw` command by,
 *   * sending `⌥←` to move the cursor to the beginning of the word, then
 *   * sending `⇧⌥→` to select the text until the end of current word, and finally
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_inner_word(void) {
  vim_word_back();
  vim_yank_word_end();
}

/**
 * Simulates Vim's `y0` command by,
 *   * sending `⇧⌥↑` to select the text until the beginning of the line, then
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_to_line_fst(void) { vim_yank_move(vim_line_first); }

/**
 * Simulates Vim's `y$` command by,
 *   * sending `⇧⌥↓` to select the text until the end of the line, then
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_to_line_end(void) { vim_yank_move(vim_line_end); }

/**
 * Simulates Vim's `yy` and `Y` commands by,
 *   * sending `⌥↑` to move the cursor to the start of the line, then
 *   * sending `⇧⌥↓` to select the text until the end of the line, and finally
 *   * sending `⌘c` to copy the selected text.
 */
void vim_yank_line(void) {
  vim_line_first();
  vim_yank_to_line_end();
}
// }}}

// Vim other macro functions {{{

/**
 * Simulates Vim's `:` command to enter COMMAND mode by sending,
 *   * sending `⌘␣` by default to open Spotlight if `VIM_COMM_KC` isn't defined, or
 *   * sending `VIM_COMM_KC` if it is defined.
 */
void vim_command(void) {
  #ifndef VIM_COMM_KC
    #define VIM_COMM_KC G(KC_SPC)
  #endif
  tap(VIM_COMM_KC);
}

void vim_lookup(void) { tap( C(G(KC_D)) ); }

/**
 * Simulates Vim's `⌃r` command by sending `⇧⌘z`.
 */
void vim_redo(void) { tap( S(G(KC_X)) ); }

/**
 * Simulates Vim's `u` command by sending `⌘z`.
 */
void vim_undo(void) { tap(G(KC_Z)); }
// }}}
