/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#include "quantum.h"
#include "analogkeys.h"
#include "custom_midi.h"
#include "process_midi.h"
#include "print.h"

#ifdef MIDI_ENABLE
#include "qmk_midi.h"

static uint8_t tone_status[MIDI_TONE_COUNT];
#endif

uint8_t last_midi_note;
uint8_t last_velocity;

void midi_init_user(void) {
    #ifdef MIDI_ENABLE
        for (uint8_t i = 0; i < MIDI_TONE_COUNT; i++) {
            tone_status[i] = MIDI_INVALID_NOTE;
        }
    #endif
}

bool is_note_played(void) {
    return (last_midi_note != 0);
}

void note_to_string(char *note_string) {
    char *note[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    uint8_t octv;
    uint8_t nt;
    octv = last_midi_note / 12 - 1;
    nt = (last_midi_note % 12);
    sprintf(note_string, "%s%d V%d", note[nt], octv, last_velocity);
}

void process_midi_user(uint16_t keycode, uint8_t row, uint8_t col, bool pressed) {
    #ifdef MIDI_ENABLE
        uint8_t channel  = midi_config.channel;
        uint8_t tone     = keycode - MIDI_TONE_MIN;
        uint8_t velocity = 64;

        #ifdef VELOCITY_ENABLE
            analog_key_t *key = &keys[row][col];
            velocity = key->velocity;
        #endif

        if (pressed) {
            if (tone_status[tone] == MIDI_INVALID_NOTE) {
                uint8_t note = midi_compute_note(keycode);
                last_midi_note = note;
                last_velocity = velocity;
                midi_send_noteon(&midi_device, channel, note, velocity);
                char note_str[20];
                memset(note_str, 0x00, 20);
                note_to_string(note_str);
                uprintf("MIDI noteon  CH%d note:%d %s\n", channel, note, note_str);
                tone_status[tone] = note;
            }
        } else {
            uint8_t note = tone_status[tone];
            if (note != MIDI_INVALID_NOTE) {
                midi_send_noteoff(&midi_device, channel, note, velocity);
                uprintf("MIDI noteoff CH%d note:%d\n", channel, note);
            }
            tone_status[tone] = MIDI_INVALID_NOTE;
        }
    #endif
}

#ifdef DEFERRED_EXEC_ENABLE
deferred_token deferred_midi_token;


uint32_t note_off_callback(uint32_t trigger_time, void *cb_arg) {
    uint8_t channel  = GENERAL_MIDI_PERCUSSION_CHANNEL;
    uint8_t velocity = 64;
    uint8_t *param_note;
    param_note = (uint8_t *)cb_arg;
    midi_send_noteoff(&midi_device, channel, *param_note, velocity);
    free(cb_arg);
    return 0;
}
#endif

void midi_send_percussion(uint8_t note) {
    uint8_t channel  = GENERAL_MIDI_PERCUSSION_CHANNEL;
    uint8_t velocity = 64;

    last_midi_note = note;
    last_velocity = velocity;
    midi_send_noteon(&midi_device, channel, note, velocity);

    #ifdef DEFERRED_EXEC_ENABLE
        uint8_t *param_note;
        param_note = malloc(sizeof(uint8_t));
        *param_note = note;
        deferred_midi_token = defer_exec(TIMEOUT_NOTE_OFF, note_off_callback, param_note);
    #endif
}
