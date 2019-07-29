#include "leader.h"

// State variables
bool     leading          = false;                              // whether currently in a leader sequence
uint16_t lead_seq[5]      = {0, 0, 0, 0, 0};                    // stores leader sequence keycodes
uint8_t  lead_seq_size    = 0;                                  // number of keycodes in sequence so far
bool     leads_defined[5] = {true, false, false, false, false}; // whether there are longer valid leaders then current sequence

// Leader keycode provided by user
extern uint16_t leader_keycode;

// Function user defines in their keymap.c that includes leader sequence definition macros
__attribute__((weak)) bool check_leaders(void) { return false; }

// Function called from process_record_user to integrate leader functionality
bool process_leader(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    if (leading) {
      lead_seq[lead_seq_size] = keycode;
      leading = (check_leaders()) ? leads_defined[lead_seq_size+1] : false;
      lead_seq_size++;
      return false;
    }
    else if (keycode == leader_keycode) {
      leading = true;
      lead_seq_size = 0;
      memset(lead_seq, 0, sizeof(lead_seq));
      return false;
    }
  }
  return true;
}
