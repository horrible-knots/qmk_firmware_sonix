# project specific files
SRC += config_led.c
SRC += command_extra.c
SRC += openrgb_eeprom.c

# MCU name - Engraved with VS11K09A-1.
MCU = SN32F248BF

# Build Options
#   comment out to disable the options.
#
LTO_ENABLE = no
BACKLIGHT_ENABLE = no
MOUSEKEY_ENABLE = no    # Mouse keys
EXTRAKEY_ENABLE = no  # Audio control and System control
SLEEP_LED_ENABLE = no   # Breathing sleep LED during USB suspend
VIRTSER_ENABLE = no
NKRO_ENABLE = yes       # USB Nkey Rollover
AUDIO_ENABLE = no
WAIT_FOR_USB = no
RAW_ENABLE = no
TAP_DANCE_ENABLE = no
KEYBOARD_SHARED_EP = yes
NO_USB_STARTUP_CHECK = yes
#DEBOUNCE_TYPE = sym_eager_pk
# Debug
MAGIC_KEYCODE_ENABLE = yes # See quantum/command.c and quantum/command.h for available commands
CONSOLE_ENABLE = yes         # Console for debug.
COMMAND_ENABLE = yes       # Commands for debug and configuration.
BOOTMAGIC_ENABLE = yes     # Hold down escape while powering up to jump to bootloader.  Seems broken currently.
# Choose which key to start up in bootloader by matrix grid.
BOOTMAGIC_LITE_COLUMN = 0
BOOTMAGIC_LITE_ROW    = 0
# Print out uS per matrix scan when CONSOLE_ENABLE = yes on the HID listener.
DEBUG_MATRIX_SCAN_RATE = yes
# OpenRGB
OPENRGB_ENABLE = no
OPENRGB_DIRECT_MODE_UNBUFFERED = no
# Custom effects
RGB_MATRIX_CUSTOM_KB = yes

# Custom RGB matrix handling
CUSTOM_MATRIX = shared
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = SN32F24xB
WEAR_LEVELING_DRIVER = sn32_flash
# Debugging ram usage
TOP_SYMBOLS = yes

# Debugging
OPT_DEFS += -g
OPT = 0
BOOTLOADER = sn32-dfu
