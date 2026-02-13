/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

#define ENCODER_BUTTON_PIN C15
#define ENCODER_ROW 2
#define ENCODER_COL 14

#define MUXES 6
#define MUX_PINS { A2, A3, A6, A7, B12, B14 }
#define MUX_SELECTOR_BITS 4
#define MUX_SELECTOR_PINS { B11, B10, B1, B2 }

#define MUX_CHANNELS ( 1 << MUX_SELECTOR_BITS )

#define EECONFIG_KB_DATA_SIZE 6

#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
#define DEBUG_PRINT
//#define DEBUG_MATRIX_SCAN_RATE
#endif

#define ADC_RESOLUTION      ADC_CFGR_RES_12BITS
#define ADC_SAMPLING_TIME  ADC_SMPR_SMP_2P5
#define ADC_RESOLUTION_MAX  1 << 12

#define CALIBRATION_RANGE 255

// Optional: enable a tiny moving average for raw ADC to reduce noise (0=disabled)
#ifndef ANALOG_FILTER_TAPS
#define ANALOG_FILTER_TAPS 0
#endif

// Validate filter configuration
#if (ANALOG_FILTER_TAPS < 0) || (ANALOG_FILTER_TAPS > 32)
#error "ANALOG_FILTER_TAPS must be between 0 and 32"
#endif

// ADC timeout for safety (microseconds)
#ifndef ADC_TIMEOUT_US
#define ADC_TIMEOUT_US 1000
#endif

#ifdef RGB_MATRIX_ENABLE
    #define WS2812_PWM_TARGET_PERIOD 800000
    #define WS2812_PWM_DRIVER PWMD16
    #define WS2812_PWM_CHANNEL 1
    #define WS2812_PWM_PAL_MODE 1
    #define WS2812_DMA_STREAM STM32_DMA1_STREAM3
#endif

#ifdef OLED_ENABLE
    #define I2C1_SCL_PIN B6
    #define I2C1_SDA_PIN B7
    #define I2C1_CLOCK_SPEED 400000
    #define I2C1_DUTY_CYCLE FAST_DUTY_CYCLE_16_9
    #define OLED_BRIGHTNESS 64
    #define OLED_UPDATE_INTERVAL 1000
    #define OLED_DISPLAY_128X32
#endif

#ifdef AUDIO_ENABLE
    #define AUDIO_PIN A4
    #define AUDIO_PIN_ALT_AS_NEGATIVE
    #define AUDIO_PIN_ALT A5
    #define AUDIO_INIT_DELAY
    #define AUDIO_DAC_SAMPLE_MAX 4095U
    #define TEMPO_DEFAULT 70
    //#define AUDIO_DAC_OFF_VALUE AUDIO_DAC_SAMPLE_MAX

    //#define AUDIO_DAC_QUALITY_VERY_LOW
    //#define AUDIO_CLICKY

    #define AUDIO_DAC_SAMPLE_RATE 32768U
    #define AUDIO_MAX_SIMULTANEOUS_TONES 8

    #define STARTUP_SONG SONG()
    #define GOODBYE_SONG SONG()
#endif

/* KHEops70 */
/* New features VELOCITY_ENABLE and KEY_PADS_ENABLE supported in keyboard.json using /data/mappings/info_rules.hjson
    // KHEOPS70
    "VELOCITY_ENABLE": {"info_key": "velocity.enabled", "value_type": "bool"},
    "KEY_PADS_ENABLE": {"info_key": "key_pads.enabled", "value_type": "bool"},
    "KHEOPS70_ENABLE": {"info_key": "kheops70.enabled", "value_type": "bool"},
*/
#ifndef VELOCITY_ENABLE
#define VELOCITY_ENABLE                 true
#endif
#ifndef KHEOPS70_ENABLE
#define KHEOPS70_ENABLE                 true
#endif
#ifndef KEY_PADS_ENABLE
#define KEY_PADS_ENABLE                 true
#endif
#define DEBOUNCE                        15
#define DEBUG_KEY_THRESHOLD             80      /* Print debug message for the debug key if distance shorter */
#define SOFT_KEY_THRESHOLD              80      /* Threshold for "hard pressed key" */
#define RAW_VALUE_FOCUS_THRESHOLD       2000

/* key combination for commands */
#define IS_COMMAND() ( \
    get_mods() == MOD_MASK_CTRL)

#ifdef CAPS_WORD_ENABLE
    #define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD
#endif

#define TIMEOUT_PRINT_INIT              5000
#define TIMEOUT_RECALIBRATE_INIT        300000
#define TIMEOUT_PRINT                   5000
#define TIMEOUT_RECALIBRATE             60000

// Actuation point
#define KEY_ACTUATION_POINT             100     /* 48 Moonboard original */
#define KEY_PRESS_HYSTERESIS            0
#define KEY_RELEASE_HYSTERESIS          5       /* Original: 30 */
#define KEY_VELOCITY_TRIGGER_POINT      50      /* Trigger point to calculate velocity */
//  #define DEBUG_KEY_ROW                   3   // 0,3 KC_LSFT
#define DEBUG_KEY_ROW                   11      // 0,11 KC_RSFT
// #define DEBUG_KEY_COL                   99
#define DEBUG_KEY_COL                   0
// #define PREVENT_STUCK_MODIFIERS         true     // deprecated

#ifdef KEY_PADS_ENABLE
    #define KEY_PAD_1_ROW               1
    #define KEY_PAD_1_COL               13
    #define KEY_PAD_2_ROW               1
    #define KEY_PAD_2_COL               14
#endif

#ifdef MIDI_ENABLE
    /* #define MIDI_BASIC */
    #define MIDI_ADVANCED
    #define MIDI_CUSTOM_ENABLE
    #define VELOCITY_BUFFER_DEPTH       3

    /* Avoid 127 since it is used as a special number in some sound sources. */
    #define MIDI_INITIAL_VELOCITY       126
#endif

#ifdef RGB_MATRIX_ENABLE
    #if !defined(IOS_DEVICE_ENABLE)
        #define USB_MAX_POWER_CONSUMPTION 450
    #else
        /* fix iPhone and iPad power adapter issue - iOS device need lessthan 100 */
        #define USB_MAX_POWER_CONSUMPTION 100
    #endif

    /* Specific keys position */
    #define LSHIFT_LED_INDEX            40          // LEFT SHIFT KEY, USED TO SIGNAL CAPS LOCK
    #define CAPS_LOCK_LED_INDEX         39          // CAPS LOCK KEY, USED AS TAB
    // #define CAPS_LOCK_LED_INDEX         40
    #define ENTER_LED_INDEX             27
    #define BACKSPACE_LED_INDEX         26
    #define DELETE_LED_INDEX            54
    #define RALT_LED_INDEX              61
    #define LALT_LED_INDEX              67
    #define RCTRL_LED_INDEX             62
    #define LCTRL_LED_INDEX             69
    #define RFN_LED_INDEX               62
    #define LFN_LED_INDEX               66

    #define NUM1_LED_INDEX              11
    #define NUM2_LED_INDEX              10
    #define NUM3_LED_INDEX              9
    #define NUM4_LED_INDEX              8
#endif
