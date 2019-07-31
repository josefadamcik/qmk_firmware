// WIP implementation of Vim commands using QMK macros on macOS
#include <stdbool.h>
#include <stdint.h>
#include "quantum.h"


enum vim_modes {
  INSERT,
  NORMAL,
  NORM_c,
  NORM_ca,
  NORM_cg,
  NORM_ci,
  NORM_d,
  NORM_da,
  NORM_dg,
  NORM_di,
  NORM_y,
  NORM_ya,
  NORM_yg,
  NORM_yi,
  NORM_f,
  NORM_F,
  NORM_g,
  NORM_t,
  NORM_T,
  NORM_r,
  REPLACE,
};


/** Macro to include externs user might want to read */
#define VIM_EXTERNS() \
  extern uint8_t vim_mode; \
  extern uint8_t vim_find_direction


// Functions to add mods pressed to keycode
uint16_t add_mods_pressed(uint16_t kc);
uint16_t add_shift_if_pressed(uint16_t kc);
uint16_t add_ctlr_if_pressed(uint16_t kc);
uint16_t add_gui_if_pressed(uint16_t kc);
uint16_t add_alt_if_pressed(uint16_t kc);

// Function that does all the work
bool process_record_vim(uint16_t keycode, keyrecord_t *record);


// Vim command helper macros
#define TAP(keycode)     tap_code16(keycode)
#define RELEASE(keycode) unregister_code16(keycode)
#define PRESS(keycode)   register_code16(keycode)

#define VIM_SELECT_MOVE(macro) PRESS(KC_LSHIFT); macro; RELEASE(KC_LSHIFT)
#define VIM_CUT_MOVE(macro)    VIM_SELECT_MOVE(macro); TAP(LGUI(KC_X))
#define VIM_YANK_MOVE(macro)   VIM_SELECT_MOVE(macro); TAP(LGUI(KC_C)); TAP(KC_LEFT)

#define VIM_REPLACE(kc) TAP(KC_DELETE); TAP(kc) // behaves badly at end of line

#define VIM_TO_INSERT(macro) macro; vim_mode = INSERT
#define VIM_TO_NORMAL(macro) macro; vim_mode = NORMAL

#define KC_EOL TAP(LCTL(KC_E))


// Vim command macros
// TODO: actually test all of these and make sure they work as expected

// movement primitives
#define VIM_h     TAP(KC_LEFT)
#define VIM_LEFT  VIM_h
#define VIM_BSPC  VIM_h
#define VIM_l     TAP(KC_RIGHT)
#define VIM_RIGHT VIM_l
#define VIM_SPACE VIM_l
#define VIM_k     TAP(KC_UP)
#define VIM_UP    VIM_k
#define VIM_gk    VIM_k
#define VIM_j     TAP(KC_DOWN)
#define VIM_gj    VIM_j
#define VIM_DOWN  VIM_j
#define VIM_b     TAP(LALT(KC_LEFT))
#define VIM_e     TAP(LALT(KC_RIGHT))
#define VIM_0     TAP(LCTL(KC_A))
#define VIM_gg    TAP(LGUI(KC_UP))

// movement build on primitives
#define VIM_G      TAP(LGUI(KC_DOWN)); VIM_0
#define VIM_ge     VIM_b;              VIM_b; VIM_e
#define VIM_w      VIM_e;              VIM_e; VIM_b
#define VIM_MINUS  VIM_0;              VIM_k; VIM_0
#define VIM_PLUS   KC_EOL;             VIM_j; VIM_0
#define VIM_ENTER  VIM_PLUS
#define VIM_DOLLAR KC_EOL;             VIM_h

// insertion (not pure because mode change is also needed)
#define VIM_a VIM_l
#define VIM_A KC_EOL
#define VIM_i TAP(KC_NO)
#define VIM_I VIM_0
#define VIM_o KC_EOL; TAP(KC_ENTER)
#define VIM_O VIM_0;  TAP(KC_ENTER); VIM_k

// change (not pure because mode change is also needed)
#define VIM_ch      VIM_CUT_MOVE(VIM_h)
#define VIM_cl      VIM_CUT_MOVE(VIM_l)
#define VIM_ck      KC_EOL;              VIM_CUT_MOVE(VIM_MINUS)
#define VIM_cj      VIM_0;               VIM_CUT_MOVE(VIM_PLUS)
#define VIM_cb      VIM_CUT_MOVE(VIM_b)
#define VIM_ce      VIM_CUT_MOVE(VIM_e)
#define VIM_cw      VIM_ce                                        // this is not a mistake, silly Vim
#define VIM_caw     VIM_b;               VIM_CUT_MOVE(VIM_w)
#define VIM_ciw     VIM_b;               VIM_ce;
#define VIM_c0      VIM_CUT_MOVE(VIM_0)
#define VIM_C       VIM_CUT_MOVE(KC_EOL)
#define VIM_cDOLLAR VIM_C
#define VIM_cc      VIM_0;               VIM_C
#define VIM_cgg     KC_EOL;              VIM_CUT_MOVE(VIM_gg)
#define VIM_cG      VIM_0;               VIM_CUT_MOVE(TAP(G(KC_DOWN)))

// substitute (not pure because mode change is also needed)
#define VIM_s VIM_cl
#define VIM_S VIM_cc

// deletion
#define VIM_x       VIM_cl;
#define VIM_X       VIM_ch;
#define VIM_DELETE  VIM_cl;
#define VIM_dl      VIM_cl
#define VIM_dh      VIM_ch
#define VIM_dk      VIM_ck;  TAP(KC_DELETE)
#define VIM_dj      VIM_cj;  TAP(KC_DELETE)
#define VIM_db      VIM_cb
#define VIM_de      VIM_ce
#define VIM_dw      VIM_cw
#define VIM_daw     VIM_caw
#define VIM_diw     VIM_ciw
#define VIM_d0      VIM_c0
#define VIM_D       VIM_C;   VIM_h
#define VIM_dDOLLAR VIM_D
#define VIM_dd      VIM_cc;  TAP(KC_DELETE)
#define VIM_dgg     VIM_cgg; TAP(KC_DELETE)
#define VIM_dG      VIM_cG;  TAP(KC_BSPC)

// find (not pure because of direction changes etc.)
#define VIM_SLASH    TAP(LGUI(KC_F))
#define VIM_QUES     VIM_SLASH
#define VIM_n        TAP(LGUI(KC_G))
#define VIM_N        TAP(LSFT(LGUI(KC_G)))
#define VIM_ASTERISK VIM_e; VIM_yiw; VIM_SLASH; TAP(LGUI(KC_V))
#define VIM_HASH     VIM_ASTERISK

// till/to char
// TODO: reimplement

// put
#define VIM_p_INLINE  VIM_l;          TAP(LGUI(KC_V)); VIM_h             // doesn't work right a beginning or end of line
#define VIM_p_NEWLINE VIM_o;          TAP(LGUI(KC_V)); VIM_u; VIM_CTRLr
#define VIM_P_INLINE  TAP(LGUI(KC_V))
#define VIM_P_NEWLINE VIM_O;          TAP(LGUI(KC_V)); VIM_u; VIM_CTRLr

// yank
#define VIM_yh      VIM_YANK_MOVE(VIM_h)
#define VIM_yl      VIM_YANK_MOVE(VIM_l)
#define VIM_yk      KC_EOL;                   VIM_YANK_MOVE(VIM_MINUS)
#define VIM_yj      VIM_0;                    VIM_YANK_MOVE(VIM_PLUS)
#define VIM_yb      VIM_YANK_MOVE(VIM_b)
#define VIM_ye      VIM_YANK_MOVE(VIM_e)
#define VIM_yw      VIM_YANK_MOVE(VIM_w)
#define VIM_yaw     VIM_b; VIM_yw
#define VIM_yiw     VIM_b; VIM_ye
#define VIM_y0      VIM_YANK_MOVE(VIM_0)
#define VIM_yDOLLAR VIM_YANK_MOVE(KC_EOL)
#define VIM_Y       VIM_0;                    VIM_yDOLLAR
#define VIM_yy      VIM_Y
#define VIM_ygg     KC_EOL;                   VIM_YANK_MOVE(VIM_gg)
#define VIM_yG      VIM_0;                    VIM_YANK_MOVE(TAP(G(KC_DOWN)))

// other
//#define VIM_COLON
#define VIM_K     TAP(LCTL(LGUI(KC_D)))        // experimental, need a way to exit popup
#define VIM_CTRLr TAP(LSFT(LGUI(KC_Z))); VIM_h
#define VIM_u     TAP(LGUI(KC_Z));       VIM_h // undo history will have a bunch of extra stuff compared to Vim
#define VIM_J     KC_EOL;                TAP(KC_DELETE); TAP(KC_SPACE); VIM_h


// Not possible because we can't do Word movements in macOS
// #define VIM_B
// #define VIM_E
// #define VIM_W

// Not possible because we can't move relative to what's visible in the window
// #define VIM_H
// #define VIM_L
// #define VIM_M

// Not possible because no way to do mark stuff
// #define VIM_m
// #define VIM_GRAVE
// #define VIM_QUOTE

// Not possible for other reasons
// #define VIM_Q no Ex mode
// #define VIM_U
// #define VIM_TILDE no way to toggle case
// #define VIM_PERCENT can't know enough context of surrounding delimiters
// #define VIM_CIRC can't can only go to beginning of line inclusive of whitespace
// #define VIM_AMPR can't guarantee that last substitution will rememberable
// #define VIM_BSLS not a thing
// #define VIM_DQUO no concept of registers
// #define VIM_DOT no way to reliably track previous insertion
// #define VIM_gw/VIM_gww etc. which formats
// #define VIM_gh/VIM_gH enters select mode which is not implemented
// #define VIM_ga to print Ascii value of character under cursor
// #define VIM_gO to show file specific outline

// Would be pretty buggy
// #define VIM_RPRN/VIM_LPRN could probably hack it with find macro like t/T and f/F
// #define VIM_RCBR/VIM_RCBR ditto

// Really ambitious so won't work on any time soon
// #define VIM_Q could track be used to create QMK dynamic macros
// #define VIM_Z can't to a lot of that stuff but might be able to do some
// #define VIM_EXLM probably something possible but would depend on system configuration
// #define VIM_AT macros again
// #define VIM_UNDS needs count movements to be useful
// #define VIM_PIPE needs count to be useful
