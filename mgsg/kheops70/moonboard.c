/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "moonboard.h"
#include "quantum.h"
#include "eeprom.h"
#include "scanfunctions.h"
#include "print.h"
#include "multiplexer.h"
#include "lut.h"

#include "config.h"
#include "custom_print.h"
#ifdef OLED_ENABLE
#include "custom_oled.h"
#endif

analog_config g_config = {.mode = static_actuation, .actuation_point = KEY_ACTUATION_POINT, .press_sensitivity = 32, .release_sensitivity = 32, .press_hysteresis = KEY_PRESS_HYSTERESIS, .release_hysteresis = KEY_RELEASE_HYSTERESIS};

#ifdef BOOTMAGIC_ENABLE
void bootmagic_scan(void) {
    matrix_scan();

    uint16_t threshold = distance_to_adc(CALIBRATION_RANGE / 2);
    uint16_t raw_value = keys[BOOTMAGIC_ROW][BOOTMAGIC_COLUMN].raw;

    if ((lut_b > 0 && raw_value > threshold) || (lut_b < 0 && raw_value < threshold)) {
        bootloader_jump();
    }
}
#endif

#ifdef DEFERRED_EXEC_ENABLE

#    ifdef DEBUG_ENABLE
#        ifdef DEBUG_PRINT
deferred_token debug_token;

bool debug_print(void) {
    static char rowBuffer[MATRIX_COLS * 8]; // 8: for 7 characters (" null  " or " 12345  ") + '\0'
    static uint8_t currentRow = 0;
    char *bufferPtr = rowBuffer;

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        analog_key_t *key = &keys[currentRow][col];
        if (key->raw) {
            bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "%5u  ", key->value);
        } else {
            bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), " null   ");
        }
    }

    uprintf("%s\n", rowBuffer);
    currentRow++;

    if (currentRow >= MATRIX_ROWS) {
        currentRow = 0;
        uprintf("\n");
        return false;
    }

    return true;
}

uint32_t debug_print_callback(uint32_t trigger_time, void *cb_arg) {
    // debug_print();
    debug_print_flush();        // Print other messages
    debug_print_matrix();
    return TIMEOUT_PRINT; // Assuming this is in milliseconds
}
#        endif
#    endif

deferred_token idle_recalibrate_token;
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    static uint16_t reset_timer;
    extend_deferred_exec(idle_recalibrate_token, TIMEOUT_RECALIBRATE_INIT);

    switch (keycode) {
        case QK_BOOT:
            if (record->event.pressed) {
                print("Continue pressing for bootloader DFU\n");
                reset_timer = timer_read();
                #if defined(RGB_MATRIX_ENABLE)
                    rgb_matrix_disable();
                #endif
                #ifdef OLED_ENABLE
                    oled_print_text("Continue to DFU mode");
                #endif
            } else {
                if (timer_elapsed(reset_timer) >= 200) {
                    #ifdef OLED_ENABLE
                        oled_print_text("DFU mode ON");
                        wait_ms(50);
                    #endif
                    print("Entering DFU\n");
                    reset_keyboard();
                }
            }
            return false;
        default:
            break;
    }

    return process_record_user(keycode, record);
}

uint32_t idle_recalibrate_callback(uint32_t trigger_time, void *cb_arg) {
    get_sensor_offsets();
    #ifdef DEBUG_ENABLE
        debug_calibrate_data();
    #endif
    return TIMEOUT_RECALIBRATE;
}
#endif

void values_load(void) {
    eeconfig_read_kb_datablock(&g_config, 0, sizeof(g_config));

    // Validate loaded configuration and set safe defaults if invalid
    if (g_config.mode > 3) {
        g_config.mode = static_actuation;
    }

    // Ensure actuation values are in reasonable ranges
    if (g_config.actuation_point == 0) {
        g_config.actuation_point = KEY_ACTUATION_POINT;
    }
    if (g_config.press_sensitivity == 0) {
        g_config.press_sensitivity = 32;
    }
    if (g_config.release_sensitivity == 0) {
        g_config.release_sensitivity = 32;
    }
}

void values_save(void) {
    eeconfig_update_kb_datablock(&g_config, 0, sizeof(g_config));
}

void eeconfig_init_kb() {
    values_save();

    #ifdef RGB_MATRIX_ENABLE
        // use the non noeeprom versions, to write these values to EEPROM too
        rgblight_disable(); // Enable RGB by default
        rgblight_sethsv(85,64,30);  // Set it to CYAN by default
        rgblight_mode(1); // set to solid by default
    #endif
}

void keyboard_post_init_kb(void) {
    #ifdef DEFERRED_EXEC_ENABLE
        #ifdef DEBUG_ENABLE
            #ifdef DEBUG_PRINT
                debug_token = defer_exec(TIMEOUT_PRINT_INIT, debug_print_callback, NULL);
            #endif
        #endif
        idle_recalibrate_token = defer_exec(TIMEOUT_RECALIBRATE_INIT, idle_recalibrate_callback, NULL);
    #endif
    values_load();

    keyboard_post_init_user();
}

#ifdef VIA_ENABLE
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    /* data = [ command_id, channel_id, value_id, value_data ] */
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                via_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                values_save();
                break;
            }
            default: {
                /* Unhandled message */
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    /* Return the unhandled state */
    *command_id = id_unhandled;

    /* DO NOT call raw_hid_send(data,length) here, let caller do this */
}

enum via_dynamic_actuation {
    id_mode = 1,
    id_actuation_point,
    id_press_sensitivity,
    id_release_sensitivity,
    id_press_hysteresis,
    id_release_hysteresis,
};

void via_config_set_value(uint8_t *data) {
    /* data = [ value_id, value_data ] */
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_mode:
            // Validate mode value (assume valid modes are 0-3)
            if (*value_data <= 3) {
                g_config.mode = *value_data;
            }
            break;
        case id_actuation_point:
            // Clamp value to valid range (0-40 maps to 0-255)
            if (*value_data <= 40) {
                g_config.actuation_point = *value_data * 255 / 40;
            }
            break;
        case id_press_sensitivity:
            // Clamp value to valid range
            if (*value_data <= 40) {
                g_config.press_sensitivity = *value_data * 255 / 40;
            }
            break;
        case id_release_sensitivity:
            // Clamp value to valid range
            if (*value_data <= 40) {
                g_config.release_sensitivity = *value_data * 255 / 40;
            }
            break;
        case id_press_hysteresis:
            // Clamp value to valid range
            if (*value_data <= 40) {
                g_config.press_hysteresis = *value_data * 255 / 40;
            }
            break;
        case id_release_hysteresis:
            // Clamp value to valid range
            if (*value_data <= 40) {
                g_config.release_hysteresis = *value_data * 255 / 40;
            }
            break;
    }
}

void via_config_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_mode:
            *value_data = g_config.mode;
            break;
        case id_actuation_point:
            *value_data = g_config.actuation_point * 40 / 255;
            break;
        case id_press_sensitivity:
            *value_data = g_config.press_sensitivity * 40 / 255;
            break;
        case id_release_sensitivity:
            *value_data = g_config.release_sensitivity * 40 / 255;
            break;
        case id_press_hysteresis:
            *value_data = g_config.press_hysteresis * 40 / 255;
            break;
        case id_release_hysteresis:
            *value_data = g_config.release_hysteresis * 40 / 255;
            break;
    }
}
#endif

#ifdef DEBUG_ENABLE
bool debug_calibrate_data(void) {
    static char rowBuffer2[(MATRIX_ROWS * MATRIX_COLS * 13) + 30];
    char *bufferPtr2 = rowBuffer2;

    bufferPtr2 += snprintf(bufferPtr2, sizeof(rowBuffer2) - (bufferPtr2 - rowBuffer2), "\nCalibrate Offset/Extremum\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            analog_key_t *key = &keys[row][col];
            if (key->raw) {
                bufferPtr2 += snprintf(bufferPtr2, sizeof(rowBuffer2) - (bufferPtr2 - rowBuffer2), "%5d/%4u ", key->offset, key->extremum);
            } else {
                bufferPtr2 += snprintf(bufferPtr2, sizeof(rowBuffer2) - (bufferPtr2 - rowBuffer2), " null/null ");
            }
        }
        bufferPtr2 += snprintf(bufferPtr2, sizeof(rowBuffer2) - (bufferPtr2 - rowBuffer2), "\n");
    }
    uprintf("%s\n", rowBuffer2);

    return true;
}
#endif
