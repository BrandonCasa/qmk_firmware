VIA_ENABLE = yes
ENCODER_MAP_ENABLE = yes
ENCODER_ENABLE = yes
DEBOUNCE_TYPE = custom
# USB_POLLING_INTERVAL_MS is a C define, not a rules.mk setting.
# The USB descriptor already defaults to 1 ms; verify bInterval on the host.
SRC += $(KEYMAP_PATH)/debounce.c $(KEYMAP_PATH)/socd.c
CONSOLE_ENABLE  = yes
DEBUG_ENABLE    = yes
EXTRAKEY_ENABLE = yes
