#pragma once

// Hand configuration, either side can be master.
// Docs: https://docs.qmk.fm/#/config_options?id=setting-handedness
#define EE_HANDS

// Setting to adjust of mod tap and layer tab keys respond.
// Docs: https://docs.qmk.fm/#/feature_advanced_keycodes?id=tap-hold-configuration-options
#define TAPPING_TERM 200
#define TAPPING_FORCE_HOLD
#define PERMISSIVE_HOLD

// Configuration needed to use QMK's built in split code instead of custom code
#define SOFT_SERIAL_PIN D2
#undef USE_I2C
