/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#include "analogkeys.h"
#include "print.h"
#include "config.h"
#include "quantum.h"

#ifdef VELOCITY_ENABLE
#include "lut.h"
#endif

#ifdef VELOCITY_ENABLE

    bool supported_key_velocity(uint8_t row, uint8_t col) {
        if (row == ENCODER_ROW && col == ENCODER_COL) {
            return false;
        #ifdef KEY_PADS_ENABLED
        } else if (row == KEY_PAD_1_ROW  && col == KEY_PAD_1_COL) {
            //return false;
        #endif
        }

        return true;
    }

    void store_key_velocity(uint8_t row, uint8_t col, analog_key_t *key_vel) {
        #ifdef KEY_PADS_ENABLE
            // Velocity not supported for this switch
            if (!supported_key_velocity(row, col)) {
                key_vel->vel_timer = 0;
                return;
            }
        #endif
        if (key_vel->value > KEY_VELOCITY_TRIGGER_POINT) {
            if (key_vel->vel_timer == 0) {
                key_vel->vel_timer = timer_read();
            }
        } else {
            key_vel->vel_timer = 0;
        }
    }

    /**
     * Calculates the press velocity and depress velocity.
     * For different approaches, check https://www.cs.cmu.edu/~rbd/papers/velocity-icmc2006.pdf
     */
    uint8_t compute_key_velocity(analog_key_t *key, uint16_t elapsed, bool reset) {
        uint8_t velocity = 64;

        if (elapsed == 0) {
            velocity = 0;
        } else if (elapsed > VELOCITY_ELAPSED_MAX) {
            velocity = 0;
        } else {
            // Too much memory: not needed since we only compute when key pressed
            // velocity = velocity_lut[vel_elapsed];
            velocity = elapsed_time_to_velocity(elapsed);
        }

        if (reset) {
            key->velocity = (uint8_t)velocity;
            #if defined(MIDI_ENABLE)
                if (velocity != 0) {
                    midi_config.velocity =  velocity;
                }
            #endif
            key->vel_timer = 0;
        }

        return velocity;
    }

    void reset_key_velocity(analog_key_t *key) {
        if (key != NULL) {
            key->vel_timer = 0;
            #if defined(MIDI_ENABLE)
                midi_config.velocity = 64;
            #endif
        }
    }

    void register_key_velocity(uint8_t row, uint8_t col) {
        if (supported_key_velocity(row,col)) {
            analog_key_t *key = &keys[row][col];
            if (key->vel_timer) {
                uint16_t elapsed = timer_elapsed(key->vel_timer);
                compute_key_velocity(key, elapsed, true);
                // reset_key_velocity(key);
            }
        }
    }

    void deregister_key_velocity(uint8_t row, uint8_t col) {
        //if (current_row_index != ENCODER_ROW || current_col_index != ENCODER_COL) {
            // analog_key_t *key = &keys[current_row_index][current_col_index];
            // uint8_t key_velocity = compute_key_velocity(key, elapsed, true);
            // reset_key_velocity(key);
        //}
    }
#endif

bool process_hard_key(uint8_t row, uint8_t col, uint16_t soft_keycode) {
    #ifdef VELOCITY_ENABLE
        analog_key_t *key = &keys[row][col];
        if (key->velocity > SOFT_KEY_THRESHOLD) {
            return true;
        } else {
            tap_code16(soft_keycode); // Intercept gentle press to send Home
            return false;
        }
    #else
        return true; // this allows for normal processing of key release!
    #endif
}
