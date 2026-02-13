/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#include "matrix.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "quantum.h"
#include "custom_analog.h"
#include "lut.h"
#include "multiplexer.h"
#include "scanfunctions.h"
#include <ch.h>
#include <hal.h>
#include "gpio.h"

#include "custom_print.h"
#include "qmk_midi.h"
#ifdef VELOCITY_ENABLE
#include "custom_velocity.h"
#endif

//#define CCM_DATA __attribute__((section(".ram4")))
extern void debug_log_key(void *mux_idx);

// External definitions
// Remove duplicate externs if already declared via other headers:
// extern const mux_t mux_index[MUXES][MUX_CHANNELS];
// extern ADCManager  adcManager;

// In memory structure that holds a 2D matrix with data for all keys
analog_key_t    keys[MATRIX_ROWS][MATRIX_COLS] = {0};
static uint16_t pressedAdcValue                = 0;
static uint16_t restAdcValue                   = 0;

void matrix_init_custom(void) {
    gpio_set_pin_input_high(ENCODER_BUTTON_PIN);
    generate_lut();
    pressedAdcValue = distance_to_adc(255);
    restAdcValue = distance_to_adc(0);
    multiplexer_init();
    initADCGroups();
    wait_ms(100);
    get_sensor_offsets();
}

static inline bool is_potentiometer(const mux_t *mux_idx) {
    return ((mux_idx->col == 13 || mux_idx->col == 14) && mux_idx->row == 1) ? true : false;
}

matrix_row_t previous_matrix[MATRIX_ROWS];

// Modify process_adc_readings to accept a snapshot pointer.
static void process_adc_readings(matrix_row_t current_matrix[], uint8_t ch, const ADCManager *snapshot) {
    for (uint8_t mux = 0; mux < MUXES; ++mux) {
        const mux_t *mux_idx = &mux_index[mux][ch];
        if (mux_idx->row == 255 && mux_idx->col == 255) continue; // Skip unconnected mux pin.

        analog_key_t *key = &keys[mux_idx->row][mux_idx->col];
        key->raw          = getADCSample(snapshot, mux);

        // KHEOPS70
        if (key->raw < RAW_VALUE_FOCUS_THRESHOLD) {
            key->value = 0;
            continue;
        }

        // 1. Pads in specific col/row 13 & 14 / 1
        // if (key->value > 15 && (mux_idx->col == 13 || mux_idx->col == 14) && mux_idx->row == 1) {
        //     key->value        = key->value - 15;
        // }
        // 2. Potentiometers in specific col/row 13 & 14 / 1
        if (is_potentiometer(mux_idx)) {
            // Calculate value
            key->value = (key->raw >> 4) & 0xFF;    // Convert to 8-bit
            if (key->value == 0) key->value = 1;    // TODO: Dirty trick: never accept 0 as converted value, at least 1
            if ((key->previous == 0) || (key->value > (key->previous + 4)) || (key->value < (key->previous - 4))) {
                // MIDI CC
                uint8_t chan=0;
                uint8_t cc=mux_idx->col+100;
                midi_send_cc(&midi_device, chan, cc, key->value);
                uprintf("MIDI: CC: %d value: %d raw:%04d\n", cc, key->value, key->raw);
                key->previous = key->value;
            }
            continue;
        }

        // 3. Analog key
        key->value = lut[key->raw + key->offset];

        #ifdef VELOCITY_ENABLE
            store_key_velocity(mux_idx->row, mux_idx->col, key);
        #endif

        switch (g_config.mode) {
            case dynamic_actuation:
                matrix_read_cols_dynamic_actuation(&current_matrix[mux_idx->row], mux_idx->row, mux_idx->col, key);
                break;
            case continuous_dynamic_actuation:
                matrix_read_cols_continuous_dynamic_actuation(&current_matrix[mux_idx->row], mux_idx->row, mux_idx->col, key);
                break;
            case static_actuation:
                matrix_read_cols_static_actuation(&current_matrix[mux_idx->row], mux_idx->row, mux_idx->col, key);
                break;
            case flashing:
            default:
                bootloader_jump();
                break;
        }

        #ifdef DEBUG_ENABLE
            debug_log_key((void *)mux_idx);
        #endif
    }
}

// Add the greycode conversion function
static inline uint8_t greycode(uint8_t channel) {
    return (channel >> 1) ^ channel;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    memcpy(previous_matrix, current_matrix, sizeof(previous_matrix));

    scanActive = true;

    // Kick off the first conversion on channel 0 (binary order).
    uint8_t     current       = 0;
    adcStartAllConversions(current);

    for (uint8_t ch = 1; ch < MUX_CHANNELS; ch++) {
        // Wait for the current conversion to finish
        waitForAdcConversion();

        // Snapshot results and process the current channel
        ADCManager curr_snapshot = *getAdcManagerSnapshot();
        process_adc_readings(current_matrix, current, &curr_snapshot);

        // Start the next channel conversion
        current = ch; // binary order
        adcStartAllConversions(current);
    }

    // Final channel: wait, snapshot, process
    waitForAdcConversion();
    ADCManager final_snapshot = *getAdcManagerSnapshot();
    process_adc_readings(current_matrix, current, &final_snapshot);

    // Return MUX to channel 0 (idle)
    select_mux(0);
#ifdef ENCODER_ENABLE
    bool encoder_button_pressed = !gpio_read_pin(ENCODER_BUTTON_PIN);
    if (current_matrix[ENCODER_ROW] & (1 << ENCODER_COL)) {
        if (!encoder_button_pressed) {
            deregister_key(&current_matrix[ENCODER_ROW], ENCODER_ROW, ENCODER_COL);
        }
    } else {
        if (encoder_button_pressed) {
            register_key(&current_matrix[ENCODER_ROW], ENCODER_ROW, ENCODER_COL);
        }
    }
#endif
    scanActive = false;
    return memcmp(previous_matrix, current_matrix, sizeof(previous_matrix)) != 0;
}
