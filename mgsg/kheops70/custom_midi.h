/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define GENERAL_MIDI_PERCUSSION_CHANNEL 9
#define GENERAL_MIDI_CRASH_CYMBAL_1     49
#define GENERAL_MIDI_LOW_BONGO          61
#define GENERAL_MIDI_HI_BONGO           60

#define TIMEOUT_NOTE_OFF                1000

void process_midi_user(uint16_t keycode, uint8_t row, uint8_t col, bool pressed);
void midi_init_user(void);
void note_to_string(char *note_string);
bool is_note_played(void);
void midi_send_percussion(uint8_t note);
