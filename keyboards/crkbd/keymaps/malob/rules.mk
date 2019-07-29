# General features
BLUETOOTH          = no  # Current options are AdafruitEzKey, AdafruitBLE, RN42
BLUETOOTH_ENABLE   = no  # Legacy option to Enable Bluetooth with the Adafruit EZ-Key HID.
BOOTMAGIC_ENABLE   = no  # Virtual DIP switch configuration (+1000)
COMMAND_ENABLE     = no  # Commands for debug and configuration
CONSOLE_ENABLE     = no  # Console for debug (+400)
EXTRAKEY_ENABLE    = yes # Audio control and System control(+450)
LEADER_ENABLE      = no  # Leader key functionality
MOUSEKEY_ENABLE    = no  # Mouse keys (+4700)
NKRO_ENABLE        = no  # USB N-Key Rollover
OLED_DRIVER_ENABLE = yes # Use QMK OLED driver
UNICODE_ENABLE     = no  # Unicode

# Audio features
AUDIO_ENABLE = no  # Enable the audio subsystem
MIDI_ENABLE  = no  # MIDI controls

# LED features
# TODO: Understand what this stuff does.
BACKLIGHT_ENABLE = no  # Enable keyboard backlight functionality
RGBLIGHT_ENABLE  = no # Enable WS2812 RGB underlight.
SLEEP_LED_ENABLE = no  # Breathing sleep LED during USB suspend
# Do not enable SLEEP_LED_ENABLE, it uses the same timer as BACKLIGHT_ENABLE

# Not explicitly needed but nice to include since I'm using Elite-Cs not Pro Micros
BOOTLOADER = atmel-dfu

# Settings to use QMK code rather than project custom code
CUSTOM_MATRIX  = no  # Don't use project custom matrix.c
SPLIT_KEYBOARD = yes # Use QMK split library rather can code in project
SRC = 				 			 # Remove all custom project code


# TODO: Figure out why LINK_TIME_OPTIMIZATION_ENABLE makes the slave half unresponsive.
