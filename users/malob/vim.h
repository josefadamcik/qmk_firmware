// WIP implementation of Vim commands using QMK macros
#include <stdbool.h>
#include <stdint.h>
#include "quantum.h"

#define press(keycode)   register_code16(keycode)
#define release(keycode) unregister_code16(keycode)
#define tap(keycode)     tap_code16(keycode)

// Vim state
enum vim_states {
  INSERT,
  NORMAL,
  NORM_c,
  NORM_ca,
  NORM_ci,
  NORM_d,
  NORM_da,
  NORM_di,
  NORM_f,
  NORM_F,
  NORM_g,
  NORM_r,
  NORM_t,
  NORM_T,
  NORM_y,
  NORM_ya,
  NORM_yi,
  REPLACE,
};

uint8_t vim_current_state(void);
void    vim_set_state(uint8_t state);

enum find_directions {
  FORWARD,
  BACKWARD,
};

uint8_t vim_find_direction(void);
void    vim_set_find_direction(uint8_t direction);

// Helper functions
void     vim_select_move(void (*cmd)(void));
void     vim_cut_move(void (*cmd)(void));
void     vim_yank_move(void (*cmd)(void));
uint16_t add_shift_if_pressed(uint16_t kc);
uint16_t add_ctlr_if_pressed(uint16_t kc);
uint16_t add_gui_if_pressed(uint16_t kc);
uint16_t add_alt_if_pressed(uint16_t kc);
uint16_t add_mods_pressed(uint16_t kc);

// Vim command execution management functions
bool process_record_vim(uint16_t keycode, keyrecord_t *record);
bool vim_exec_macro(uint16_t keycode);
void vim_cmd(void (*cmd)(void), uint8_t new_state);

// Vim command functions
// movement
void vim_left(void);
void vim_right(void);
void vim_up(void);
void vim_down(void);
void vim_word_back(void);
void vim_word_end(void);
void vim_word_back_end(void);
void vim_word(void);
void vim_line_first(void);
void vim_line_end(void);
void vim_line_up(void);
void vim_line_down(void);
void vim_goto_line_first(void);
void vim_goto_line_last(void);
// insertion
void vim_open(void);
void vim_open_above(void);
// change
void vim_chg_left(void);
void vim_chg_right(void);
void vim_chg_up(void);
void vim_chg_down(void);
void vim_chg_word_back(void);
void vim_chg_word_end(void);
void vim_chg_word(void);
void vim_chg_a_word(void);
void vim_chg_inner_word(void);
void vim_chg_to_line_fst(void);
void vim_chg_to_line_end(void);
void vim_chg_line(void);
// deletion
void vim_del_up(void);
void vim_del_down(void);
void vim_del_to_line_end(void);
void vim_del_line(void);
// find
void vim_find(void);
void vim_find_next(void);
void vim_find_prev(void);
void vim_find_word(void);
// till/to char
void vim_till_to_find(uint16_t kc);
void vim_till_char(uint16_t kc);
void vim_till_char_back(uint16_t kc);
void vim_to_char(uint16_t kc);
void vim_to_char_back(uint16_t kc);
// put
void vim_put(void);
// replace
void vim_replace_start(void);
void vim_replace_end(void);
void vim_replace_enter(void);
void vim_replace(uint16_t);
// yank
void vim_yank_left(void);
void vim_yank_right(void);
void vim_yank_up(void);
void vim_yank_down(void);
void vim_yank_word_back(void);
void vim_yank_word_end(void);
void vim_yank_word(void);
void vim_yank_a_word(void);
void vim_yank_inner_word(void);
void vim_yank_to_line_fst(void);
void vim_yank_to_line_end(void);
void vim_yank_line(void);
// other
void vim_command(void);
void vim_lookup(void);
void vim_redo(void);
void vim_undo(void);
