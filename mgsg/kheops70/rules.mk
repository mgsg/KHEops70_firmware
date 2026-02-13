SRC += matrix.c lut.c scanfunctions.c multiplexer.c custom_analog.c moonboard.c custom_midi.c custom_velocity.c custom_oled.c custom_print.c

MCU_LDSCRIPT = STM32F303xB

CUSTOM_MATRIX = lite

OPT = 2

#AUDIO_DRIVER = dac_basic
AUDIO_DRIVER = dac_additive

WEAR_LEVELING_DRIVER = embedded_flash

RGB_MATRIX_CUSTOM_KB = yes
