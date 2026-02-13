/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdint.h>
#include <stdbool.h>

void debug_print_add(const char* data);
void debug_print_timestamp(const char* data, int8_t id, bool condition);
bool debug_print_is_empty(void);
void debug_print_flush(void);
void debug_print_mem(void);
void debug_print_threads(void);
bool debug_print_matrix(void);
void debug_log_key(void *mux_idx);
