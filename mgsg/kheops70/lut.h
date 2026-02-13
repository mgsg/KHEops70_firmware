/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>

extern uint8_t lut[ADC_RESOLUTION_MAX];

const double lut_a;
const double lut_b;
const double lut_c;
const double lut_d;

uint16_t distance_to_adc(uint8_t distance);

uint8_t adc_to_distance(uint16_t adc);

void generate_lut(void);

#ifdef VELOCITY_ENABLE
    #ifndef VELOCITY_ELAPSED_MAX
        #define VELOCITY_ELAPSED_MAX    1024
    #endif

    // LUT to get velocity from elapsed time
    //extern uint8_t velocity_lut[1024];
    
    //void generate_velocity_lut(void);
    uint8_t elapsed_time_to_velocity(uint16_t elapsed);
#endif