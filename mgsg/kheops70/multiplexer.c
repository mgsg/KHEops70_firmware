/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#include "quantum.h"
#include "multiplexer.h"

const pin_t mux_pins[MUXES] = MUX_PINS;
const pin_t mux_selector_pins[MUX_SELECTOR_BITS] = MUX_SELECTOR_PINS;

// Compile-time sanity checks
_Static_assert(MUX_CHANNELS == (1U << MUX_SELECTOR_BITS), "MUX_CHANNELS must equal (1 << MUX_SELECTOR_BITS)");
_Static_assert((sizeof mux_index / sizeof mux_index[0]) == MUXES, "mux_index first dimension must equal MUXES");
_Static_assert((sizeof mux_index[0] / sizeof mux_index[0][0]) == MUX_CHANNELS, "mux_index second dimension must equal MUX_CHANNELS");

void multiplexer_init(void) {
    // Initialize selector pins to known state (channel 0)
    for (uint8_t i = 0; i < MUX_SELECTOR_BITS; i++) {
        pin_t pin = mux_selector_pins[i];
        setPinOutput(pin);
        writePin(pin, 0); // Set all selector bits to 0 (channel 0)
    }

    // Initialize current channel to match hardware state
    current_channel = 0;

    // Small delay to ensure multiplexer settles
    wait_us(10);
}

bool select_mux(uint8_t channel) {
    // Bounds check with tighter constraint (channel must be strictly less than MUX_CHANNELS)
    if (channel >= MUX_CHANNELS) {
        return false;
    }

    // Binary channel selection using bit manipulation
    for (uint8_t i = 0; i < MUX_SELECTOR_BITS; i++) {
        writePin(mux_selector_pins[i], (channel >> i) & 1);
    }
    current_channel = channel;
    return true;
}

const mux_t NC = {255,255}; // A coord with a Null pin (from JSON)

#ifdef KHEOPS70_ENABLE
/* KHEops70 */

#ifndef KEY_PADS_ENABLE
    #define PAD1 NC
    #define PAD2 NC
#else
    #define PAD1 {1, 13}
    #define PAD2 {1, 14}
    //#define PAD2 NC
#endif
const mux_t mux_index[MUXES][MUX_CHANNELS] = {
    // I0      I1     I2      I3     I4      I5      I6      I7      I8      I9     I10     I11     I12    I13     I14     I15
    { {2,1},  NC,     {0,1},  NC,    {1,1},  {1,0},  {0,0},  {2,0},  {3,0},  {4,0}, NC,     NC,     NC,    {4,1},  {3,1},  NC},    // MUX0
    { {2,3},  NC,     {0,3},  NC,    {1,3},  {1,2},  {0,2},  {2,2},  NC,     {3,2}, NC,     {4,2},  NC,    NC,     {3,3},  {4,3}}, // MUX1
    { {2,5},  {1,6},  {0,5},  {2,6}, {1,5},  {1,4},  {0,4},  {2,4},  {3,4},  {4,4}, NC,     {4,5},  NC,    NC,     {3,5},  {4,6}}, // MUX2
    { {2,8},  {1,9},  {0,8},  {2,9}, {0,7},  {0,6},  {1,8},  {1,7},  {3,6},  {3,7}, {2,7},  {4,7},  NC,    {4,8},  {3,8},  {4,9}}, // MUX3
    { {2,11}, {0,12}, {1,11}, {0,9}, {0,11}, {2,10}, {0,10}, {1,10}, {3,10}, NC,    {4,10}, NC,     {3,9}, {4,11}, {3,11}, NC},    // MUX4
    {  NC,    {3,14}, {3,13}, NC,    {1,12}, {2,12}, {2,13}, NC,     {3,12}, NC,    {4,12}, {4,13}, NC,    PAD1,   {4,14}, PAD2}   // MUX5
};
#else
/* MoonBoard */
const mux_t mux_index[MUXES][MUX_CHANNELS] = {
    {{2,1},NC,{0,1},NC,{1,1},{1,0},{0,0},{2,0},{3,0},{4,0},NC,{5,0},NC,{4,1},{3,1},{5,1}},
    {{2,3},NC,{0,3},NC,{1,3},{1,2},{0,2},{2,2},NC,{3,2},NC,{4,2},NC,{5,2},{3,3},{4,3}},
    {{2,5},{1,6},{0,5},{2,6},{1,5},{1,4},{0,4},{2,4},{3,4},{4,4},NC,{4,5},NC,{5,3},{3,5},{4,6}},
    {{2,8},{1,9},{0,8},{2,9},{0,7},{0,6},{1,8},{1,7},{3,6},{3,7},{2,7},{4,7},NC,{4,8},{3,8},{4,9}},
    {{2,11},{1,12},{0,11},{0,9},{0,10},{1,10},{1,11},{2,10},{3,10},{5,4},{4,10},{5,5},{3,9},{4,11},{3,11},{5,6}},
    {{2,14},{3,13},NC,NC,{0,12},{2,12},{1,13},{2,13},{3,12},{5,7},{4,12},{4,13},NC,{5,8},{4,14},{5,9}}
};
#endif


