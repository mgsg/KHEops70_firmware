/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#ifdef VELOCITY_ENABLE
    #include "analogkeys.h"

    void store_key_velocity(uint8_t row, uint8_t col, analog_key_t *key);
    uint8_t compute_key_velocity(analog_key_t *key, uint16_t elapsed, bool reset);
    void reset_key_velocity(analog_key_t *key);
    bool supported_key_velocity(uint8_t row, uint8_t col);
    void register_key_velocity(uint8_t row, uint8_t col);
    void deregister_key_velocity(uint8_t row, uint8_t col);
#endif

bool process_hard_key(uint8_t row, uint8_t col, uint16_t soft_keycode);
