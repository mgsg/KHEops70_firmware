/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>

#ifdef OLED_ENABLE
enum oled_screens {
    OLED_LOGO,
    OLED_STATUS,
    OLED_MIDI,
    OLED_FUNC,
    OLED_TEXT
};

void oled_print_screen(uint8_t);
void oled_print_text(char *);
#endif
