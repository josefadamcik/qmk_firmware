#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "quantum.h"

bool check_leaders(void);
bool process_leader(uint16_t keycode, keyrecord_t *record);

// Generates code that equals true if lead_seq is equal to a specific leader sequence
#define LEAD_CHECK(x1, x2, x3, x4, x5) \
  (lead_seq[0] == (x1) && lead_seq[1] == (x2) && lead_seq[2] == (x3) && lead_seq[3] == (x4) && lead_seq[4] == (x5))

// Generates code that check whether lead_seq so far is a prefix of a specific leader sequence
#define LEAD_VALID_CHECK(x1, x2, x3, x4, x5, lead_size) \
  uint16_t L##x1##x2##x3##x4##x5[5] = {x1, x2, x3, x4, x5}; \
  if (lead_seq_size < lead_size && L##x1##x2##x3##x4##x5[lead_seq_size] == lead_seq[lead_seq_size]) { \
    leads_defined[lead_seq_size+1] = true; \
  }

// Generates code that executes a keycode if a specific leader sequence is equal to lead_seq
#define LEAD_TAP_IF(x1, x2, x3, x4, x5, keycode) \
  if LEAD_CHECK(x1, x2, x3, x4, x5) { tap_code16(keycode); return false; }

// Generates code that does LEAD_VALID_CHECK then, if the lead_seq_size is the same size as a specific
// leader sequence, checks if lead_seq is equal to that sequence and if so executes a keycode
#define LEAD_TAP_FILTER(x1, x2, x3, x4, x5, keycode, lead_size) \
  LEAD_VALID_CHECK(x1, x2, x3, x4, x5, lead_size) \
  else if (lead_seq_size == lead_size) { LEAD_TAP_IF(x1, x2, x3, x4, x5, keycode) }

// Generates code for leader sequences that execute keycodes
#define LEAD1_TAP(kc1, keycode) if (lead_seq_size == 0) { LEAD_TAP_IF(kc1, 0 , 0, 0, 0, keycode) }
#define LEAD2_TAP(kc1, kc2, keycode)                LEAD_TAP_FILTER(kc1, kc2, 0,   0,   0,   keycode, 1)
#define LEAD3_TAP(kc1, kc2, kc3, keycode)           LEAD_TAP_FILTER(kc1, kc2, kc3, 0,   0,   keycode, 2)
#define LEAD4_TAP(kc1, kc2, kc3, kc4, keycode)      LEAD_TAP_FILTER(kc1, kc2, kc3, kc4, 0,   keycode, 3)
#define LEAD5_TAP(kc1, kc2, kc3, kc4, kc5, keycode) LEAD_TAP_FILTER(kc1, kc2, kc3, kc4, kc5, keycode, 4)

// Generates code that does LEAD_VALID_CHECK then, if the lead_seq_size is the same size as a specific
// leader sequence, checks if lead_seq is equal to that sequence and if so executes the arbitrary code
#define LEAD_FILTER(x1, x2, x3, x4, x5, lead_size) \
  LEAD_VALID_CHECK(x1, x2, x3, x4, x5, lead_size) \
  else if (lead_seq_size == lead_size && LEAD_CHECK(x1, x2, x3, x4, x5))

// Generates code for leader sequences that execute arbitary code
#define LEAD1(kc1) if (lead_seq_size == 0 && LEAD_CHECK(kc1, 0, 0, 0, 0))
#define LEAD2(kc1, kc2)                LEAD_FILTER(kc1, kc2, 0,   0,   0,   1)
#define LEAD3(kc1, kc2, kc3)           LEAD_FILTER(kc1, kc2, kc3, 0,   0,   2)
#define LEAD4(kc1, kc2, kc3, kc4)      LEAD_FILTER(kc1, kc2, kc3, kc4, 0,   3)
#define LEAD5(kc1, kc2, kc3, kc4, kc5) LEAD_FILTER(kc1, kc2, kc3, kc4, kc5, 4)

// Externs keymap needs to have
#define LEAD_EXTERNS() \
  extern bool leading; \
  extern uint8_t lead_seq_size; \
  extern uint16_t lead_seq[5]; \
  extern bool leads_defined[5]
