/* Copyright 2025 mgsg (@mgsg)
SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdlib.h>

#include "debug.h"
#include "print.h"
#include "timer.h"
#include "analogkeys.h"
#include "multiplexer.h"

// Debug init messages buffer
#define DBG_BUF_LEN     1000
char *dbgPrBuf = NULL;
char *dbgPrBufPtr;
uint16_t debugTimer = 0xffff;

void debug_print_init(void) {
    if (dbgPrBuf == NULL) {
        dbgPrBuf = malloc(DBG_BUF_LEN);
        memset(dbgPrBuf, 0, DBG_BUF_LEN);
        dbgPrBufPtr = dbgPrBuf;
    }
}

void debug_print_add(const char* data) {
    debug_print_init();
    dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf), "%s", data);
}

void debug_print_timestamp(const char* data, int8_t id, bool condition) {
    if (condition) {
        uint16_t elapsed = 0;
        debug_print_init();

        if (debugTimer == 0xffff) {
            debugTimer = timer_read();
        } else {
            elapsed = timer_elapsed(debugTimer);
        }
        dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf), "##%s/%d: %d\n", data, id, elapsed);
    }
}

bool debug_print_is_empty(void) {
    return (dbgPrBufPtr == NULL);
}

void debug_print_flush(void) {
    static uint16_t counter = 0;

    // For first iterations the console is usually still not ready
    if (counter++ > 3 && dbgPrBufPtr != dbgPrBuf) {
        print(dbgPrBuf);
        memset(dbgPrBuf, 0, DBG_BUF_LEN);
        dbgPrBufPtr = dbgPrBuf;
        debugTimer = 0;
    }
}

void debug_print_mem(void) {
    size_t n, size;

    n = chHeapStatus(NULL, &size, NULL);
    dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf),"core free memory : %u bytes\r\n", chCoreGetStatusX());
    dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf),"heap fragments   : %u\r\n", n);
    dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf),"heap free total  : %u bytes\r\n", size);
}

void debug_print_threads(void) {
static const char *states[] = {CH_STATE_NAMES};
thread_t *tp;

dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf),"    addr    stack prio refs     state time\r\n");
tp = chRegFirstThread();
do {
    dbgPrBufPtr += snprintf(dbgPrBufPtr, DBG_BUF_LEN - (dbgPrBufPtr - dbgPrBuf),"%08lx %08lx %4lu %4lu %9s\r\n",
            (uint32_t)tp,
            0L, // (uint32_t)tp->ctx.r13,
            (uint32_t)tp->realprio,
            (uint32_t)(tp->refs - 1),
            states[tp->state]);
    tp = chRegNextThread(tp);
} while (tp != NULL);
}

bool debug_print_matrix(void) {
    // Modified to print the whole matrix at once
    static char rowBuffer[(MATRIX_ROWS * MATRIX_COLS * 12)+10];
    static uint8_t mode = 0;
    static const char* title[] =  { "RAW\n", "VALUE\n", "RAW + OFFSET\n" };
    char *bufferPtr = rowBuffer;

    if (debug_config.mouse) {
        bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), title[mode]);
        for (uint8_t currentRow = 0; currentRow < MATRIX_ROWS; currentRow++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                analog_key_t *key = &keys[currentRow][col];
                if (!key->raw) {
                    bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "     ");
                } else if (mode == 0) {
                    bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "%4u ", key->raw);
                } else if (mode == 1) {
                    bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "%4u ", key->value);
                } else if (mode == 2) {
                    bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "%4u ", key->raw + key->offset);
                }
            }
            bufferPtr += snprintf(bufferPtr, sizeof(rowBuffer) - (bufferPtr - rowBuffer), "\n");
        }
        print(rowBuffer);
        print("\n");
        mode++;
        if (mode > 2) {
            mode = 0;
        }
    }
    return true;
}

/**
 * For a specific key DEBUG_KEY_ROW, DEBUG_KEY_COL (default "F")
 * print all values above specific point
 */
 void debug_log_key(void *vmux) {
    mux_t *mux_idx = (mux_t *)vmux;
    uint8_t row = mux_idx->row;
    uint8_t col = mux_idx->col;

    if (row == DEBUG_KEY_ROW && col == DEBUG_KEY_COL) {
        analog_key_t *key = &keys[row][col];
        if (key->value > DEBUG_KEY_THRESHOLD) {
            #ifdef VELOCITY_ENABLED
                uint16_t elapsed = (key_vel->vel_timer != 0) ? timer_elapsed(key_vel->vel_timer) : 0;
                uint8_t vel = (key_vel->vel_timer != 0) ? compute_key_velocity(key_vel, elapsed, false) : 0;
                uprintf("%u,%u,%u,%u,%u\n", col, key->raw, key->value, vel, elapsed);
            #else
                uprintf("%u,%u,%u\n", col, key->raw, key->value);
            #endif
        }
    }
}
