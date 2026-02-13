/* Copyright 2023 RephlexZero (@RephlexZero)
SPDX-License-Identifier: GPL-2.0-or-later */
#include QMK_KEYBOARD_H
#include "scanfunctions.h"
#include "moonboard.h"
#include "custom_midi.h"
#include "custom_velocity.h"
#include "custom_oled.h"
#include "custom_print.h"

// Layer names; using underscore to differentiate between layers and other variables
enum layers {
  _QWERTY,
  _FUNC,
  _MIDI,
  _EXTRA
};

enum custom_keycodes {
    C_CALIB = SAFE_RANGE,
    C_HEAL,
    C_SONG1,
    C_SONG2,
    C_SONG3,
    C_SONG4,
    SS_MAIL,
    M_PAD_1,
    M_PAD_2
};

#ifdef RGB_MATRIX_ENABLE
    // https://colordesigner.io/convert/rgbtohsv
    #define RGB_ORANGE_48      122, 43, 0
    #define RGB_BLUE_48        34, 36, 122
    #define RGB_CYAN_48        34, 122, 122
    #define RGB_GREEN_48       34, 122, 46          // Looks like Cyan!
    #define RGB_RED_48         122, 34, 34
    #define RGB_WHITE_48       122, 122, 122
    #define RGB_YELLOW_48      122, 122, 7
    #define RGB_PINK_48        122, 0, 122

    #define RGB_ORANGE_32      82, 29, 0
    #define RGB_BLUE_32        23, 24, 82
    #define RGB_CYAN_32        23, 82, 82
    #define RGB_GREEN_32       23, 82, 31
    #define RGB_RED_32         82, 23, 23
    #define RGB_WHITE_32       82, 82, 82
    #define RGB_YELLOW_32      82, 82, 4
    #define RGB_PINK_32        82, 0, 82

    #define RGB_YELLOW_24      61, 61, 2
    #define RGB_YELLOW_12      30, 30, 1

    #define RGB_MY_ORANGE      RGB_ORANGE_32
    #define RGB_MY_BLUE        RGB_BLUE_32
    #define RGB_MY_CYAN        RGB_CYAN_32
    #define RGB_MY_GREEN       RGB_GREEN_32
    #define RGB_MY_RED         RGB_RED_32
    #define RGB_MY_WHITE       RGB_WHITE_32
    #define RGB_MY_YELLOW      RGB_YELLOW_32
    #define RGB_MY_PINK        RGB_PINK_32
#endif

#ifdef DEBUG_ENABLE
    extern bool debug_calibrate_data(void);
#endif

#ifdef AUDIO_ENABLE
    float song_1[][2] = SONG(ZELDA_PUZZLE);
    float song_2[][2] = SONG(MUSIC_SCALE_SOUND);
    float song_3[][2] = SONG(ONE_UP_SOUND);
    float song_4[][2] = SONG(COIN_SOUND);
#endif

// US International ANSI Keyboard equivalents
#define ES_N RALT(KC_N)
#define ES_Q RALT(KC_SLSH)
#define ES_C RALT(KC_COMM)
#define ES_I RALT(KC_1)
#define EURO RALT(KC_5)
#define DE_SZ RALT(KC_S)

#ifdef  KEY_OVERRIDE_ENABLE
    const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
    const key_override_t de_sz_key_override = ko_make_basic(MOD_BIT_RALT, KC_S, KC_DEL);
    const key_override_t home_key_override = ko_make_basic(MOD_BIT_LALT, KC_UP, KC_HOME);
    const key_override_t end_key_override = ko_make_basic(MOD_BIT_LALT, KC_DOWN, KC_END);
    const key_override_t pgup_key_override = ko_make_basic(MOD_BIT_LALT, KC_LEFT, KC_PGUP);
    const key_override_t pgdn_key_override = ko_make_basic(MOD_BIT_LALT, KC_RGHT, KC_PGDN);

    // This globally defines all key overrides to be used
    const key_override_t *key_overrides[] = {
        &delete_key_override,
        &de_sz_key_override,
        &home_key_override,
        &end_key_override,
        &pgup_key_override,
        &pgdn_key_override
    };
#endif

#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

#ifdef  TAP_DANCE_ENABLE
    // Comment for VIAL - Interferes with VIAL tap dance
    enum {
        T_LALT,
        T_RALT,
        T_TAB,
        T_MIDI,
        T_RSFT
    };

    tap_dance_action_t tap_dance_actions[] = {
        // Tap once for first key, twice for second
        [T_LALT] = ACTION_TAP_DANCE_DOUBLE(KC_LALT, KC_CAPS),
        // [T_RALT] = ACTION_TAP_DANCE_DOUBLE(KC_RALT, KC_LSFT),
        [T_TAB] = ACTION_TAP_DANCE_DOUBLE(KC_CAPS, KC_TAB),
        [T_MIDI] = ACTION_TAP_DANCE_LAYER_MOVE(KC_LGUI, _MIDI),
        // [T_RSFT] = ACTION_TAP_DANCE_DOUBLE(KC_RSFT, KC_CAPS),
    };

    #define TD_LALT TD(T_LALT)
    #define TD_RALT TD(T_RALT)
    #define TD_TAB TD(T_TAB)
    #define TD_MIDI TD(T_MIDI)
    #define TD_RSFT TD(T_RSFT)
#else
    #define TD_LALT KC_LALT
    #define TD_RALT KC_RALT
    #define TD_TAB KC_TAB
    // #define TD_MIDI TD(T_MIDI)
    #define TD_RSFT KC_RSFT
    #define TD_RSFT KC_RSFT
#endif

/** QWERTY LAYER
   ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
   │Esc│ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ OLED  │
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┬───┤
   │ ` │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ | ] │ P1│ P2|
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
   │Cps│ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │Ent│Bck│Enc│
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
   │Sh │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │Sh │ \ │Up │Del│
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
   │Ctl│Win│Alt│Tab│   │   │   │   │Alt│Fn │Ctl│Fx │Lft│Dwn│Rgt│
   └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
 [_QWERTY] = LAYOUT_ortho_5x15(
    QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,  KC_EQL,
    KC_GRAVE,KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,  KC_RBRC, M_PAD_1, M_PAD_2,
    TD_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,  KC_ENT,  KC_BSPC, KC_MUTE,
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,  KC_NUBS, KC_UP,   KC_DEL,
    KC_LCTL, TD_MIDI, KC_LALT, KC_TAB,  KC_SPC,  KC_SPC,  KC_SPC,  KC_RCTL, KC_RALT, MO(1),   KC_HOME, KC_END,   KC_LEFT, KC_DOWN, KC_RGHT
 ),
 [_FUNC] = LAYOUT_ortho_5x15(
    KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,   KC_F12,
    RM_PREV, RM_NEXT, MU_TOGG, EE_CLR,  QK_RBT,  EURO,    _______, KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP, KC_PSCR,  _______, KC_U,    KC_D,
    RM_HUEU, RM_HUED, DE_SZ,   RM_VALU, RM_VALD, _______, CK_ON,   CK_OFF,  C_SONG1, C_SONG2, C_SONG3, C_SONG4,  _______, _______, _______,
    _______, RM_SATU, RM_TOGG, C_CALIB, _______, QK_BOOT, ES_N,    SS_MAIL, ES_C,    ES_I,    ES_Q,    _______,  _______, KC_HOME, KC_DEL,
    RM_VALD, RM_SATD, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  KC_PGUP, KC_END,  KC_PGDN
  ),
 [_MIDI] = LAYOUT_ortho_5x15(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, MI_Cs,   MI_Ds,   KC_NO,   MI_Fs,   MI_Gs,   MI_As,   KC_NO,   MI_Cs1,  MI_Ds1,  KC_NO,   MI_Fs1,  M_PAD_1, M_PAD_2,
    _______, MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C1,   MI_D1,   MI_E1,   MI_F1,   MI_G1,   MI_A1,   MI_B1,
    MI_C,    MI_Cs,   MI_Ds,   KC_NO,   MI_Fs,   MI_Gs,   MI_As,   KC_NO,   MI_Cs1,  MI_Ds1,  KC_NO,   MI_Fs1,  MI_Gs1,  MI_As1,  KC_NO,
    MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C1,   MI_D1,   MI_E1,   MI_F1,   MI_G1,   MI_A1,   MI_B1,   TO(_QWERTY)
  ),
 [_EXTRA] = LAYOUT_ortho_5x15(
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
    _______, _______, MI_Cs,   MI_Ds,   KC_NO,   MI_Fs,   MI_Gs,   MI_As,   KC_NO,   MI_Cs1,  MI_Ds1,  KC_NO,   MI_Fs1,  KC_NO,   KC_NO,
    _______, MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C1,   MI_D1,   MI_E1,   MI_F1,   MI_G1,   MI_A1,   MI_B1,
    _______, MI_Cs,   MI_Ds,   KC_NO,   MI_Fs,   MI_Gs,   MI_As,   KC_NO,   MI_Cs1,  MI_Ds1,  KC_NO,   MI_Fs1,  MI_Gs1,  MI_As1,  KC_NO,
    MI_C,    MI_D,    MI_E,    MI_F,    MI_G,    MI_A,    MI_B,    MI_C1,   MI_D1,   MI_E1,   MI_F1,   MI_G1,   MI_A1,   MI_B1,   TO(_QWERTY)
  ),
};

#if defined(ENCODER_MAP_ENABLE)
    const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
        [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD) },
        [1] = { ENCODER_CCW_CW(KC_U, KC_D) },
        [2] = { ENCODER_CCW_CW(KC_NO, KC_NO) },
        [3] = { ENCODER_CCW_CW(KC_NO, KC_NO) }
    };
#endif

void keyboard_post_init_user(void) {
    debug_print_add("+---------------+\n| KHEops70 init |\n+---------------+\n");
    debug_print_mem();
    //debug_print_threads();

    // Customise these values to desired behaviour
    // debug_enable=true;
    // debug_matrix=true;
    // debug_keyboard=true;
    // debug_mouse=true;

    debug_config.matrix = 0;
    debug_config.keyboard = 0;
    debug_config.mouse = 0;         // Trick: used for periodic ADC value check
    debug_config.enable = 1;

#ifdef MIDI_CUSTOM_ENABLE
    midi_init_user();
#endif

#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_enable();
    // rgb_matrix_mode(RGB_MATRIX_CUSTOM_QWERTY_MODE);
#endif

    // caps_word_off();
}

#ifdef RGB_MATRIX_ENABLE
/** RGB Matrix Indices
   ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
   │ 12│ 11│ 10│ 9 │ 8 │ 7 │ 6 │ 5 │ 4 │ 3 │ 2 │ 1 │ 0 │
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
   │ 13│ 14│ 15│ 16│ 17│ 18│ 19│ 20│ 21│ 22│ 23│ 24│ 25|
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┐
   │ 39│ 38│ 37│ 36│ 35│ 34│ 33│ 32│ 31│ 30│ 29│ 28│ 27│ 26│
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┐
   │ 40│ 41│ 42│ 43│ 44│ 45│ 46│ 47│ 48│ 49│ 50│ 51│ 52│ 53│ 54│
   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤
   │ 69│ 68│ 67│ 66│ 65│ 64│ 63│ 62│ 61│ 60│ 59│ 58│ 57│ 56│ 55│
   └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
 */

void set_caps_lock_rgb_indicator(void) {
    rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, RGB_GREEN_32);
}

/**
    * This runs after another backlight effect and replaces
    * colors already set.
    * The set color sets the leds for one frame.
    */
bool caps_lock_on;
bool rgb_matrix_indicators_user(void) {
    // static uint8_t rgb_effect_mode = _QWERTY;

    if (IS_LAYER_ON(_QWERTY)) {
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_LOGO);
        #endif

        // rgb_matrix_mode(RGB_MATRIX_CUSTOM_QWERTY_MODE);
        // if (rgb_effect_mode != _QWERTY) {
        //     rgb_effect_mode = _QWERTY;
        //     rgb_matrix_mode(RGB_MATRIX_CUSTOM_QWERTY_MODE);
        // }

        caps_lock_on = host_keyboard_led_state().caps_lock;
        if (caps_lock_on) {
            //set_caps_lock_rgb_indicator();

        }

        #ifdef CAPS_WORD_ENABLE
        if (is_caps_word_on()) {
            set_caps_lock_rgb_indicator();
        }
        #endif

        rgb_matrix_set_color(NUM1_LED_INDEX, RGB_MY_YELLOW);
        rgb_matrix_set_color(ENTER_LED_INDEX, RGB_MY_YELLOW);
        // rgb_matrix_set_color(DELETE_LED_INDEX, RGB_MY_RED);
        rgb_matrix_set_color(BACKSPACE_LED_INDEX,RGB_MY_RED);
        rgb_matrix_set_color(RALT_LED_INDEX, RGB_MY_GREEN);
        rgb_matrix_set_color(LALT_LED_INDEX, RGB_MY_GREEN);
        rgb_matrix_set_color(RCTRL_LED_INDEX, RGB_MY_PINK);
        rgb_matrix_set_color(LCTRL_LED_INDEX, RGB_MY_PINK);
        // //rgb_matrix_set_color(LFN_LED_INDEX, RGB_MY_YELLOW);
        // //rgb_matrix_set_color(RFN_LED_INDEX, RGB_MY_YELLOW);
    } else if (IS_LAYER_ON(_FUNC)) {
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_FUNC);
        #endif
        rgb_matrix_set_color(NUM2_LED_INDEX, RGB_MY_YELLOW);
    } else if (IS_LAYER_ON(_MIDI)) {
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_FUNC);
        #endif

        // if (rgb_effect_mode != _MIDI) {
        //     rgb_effect_mode = _MIDI;
        //     rgb_matrix_mode(RGB_MATRIX_CUSTOM_MIDI_MODE);
        // }

        rgb_matrix_set_color(NUM3_LED_INDEX, RGB_MY_YELLOW);
        // for(int i=0;i<=39;i++) {
        //     rgb_matrix_set_color(i, RGB_BLACK);
        // }
        // for(int i=40;i<=54;i++) {
        //     rgb_matrix_set_color(i, RGB_WHITE);
        // }
        // rgb_matrix_set_color(55, RGB_RED);
        // for(int i=56;i<=69;i++) {
        //     rgb_matrix_set_color(i, RGB_WHITE);
        // }
    }

    return false;
}

void caps_word_set_user(bool active) {
    uprintf("CAPS WORD %x\n", active);
    if (active) {
        set_caps_lock_rgb_indicator();
    } else {
        rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, 0, 0, 0);
    }
}
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t previous_layer = 255;
    uint8_t layer = biton32(state);
    switch(layer) {
    case _QWERTY:
        uprintf("QWERTY layer ON %x\n", layer);
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_LOGO);
            // oled_print_screen(OLED_STATUS);
        #endif
        break;
    case _FUNC:
        uprintf("FUNC layer ON %x\n", layer);
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_STATUS);
        #endif
        break;
    case _MIDI:
        uprintf("MIDI layer ON %x\n", layer);
        #ifdef OLED_ENABLE
            oled_print_screen(OLED_MIDI);        // Check https://gist.github.com/fdidron/ca9f4a70089ce4f15753fac54be59c44
        #endif
        break;
    default:
        if (previous_layer == _MIDI) {
            uprintf("MIDI layer OFF %x\n", layer);
        } else {
            uprintf("Change Layer from %x to %x\n", previous_layer, layer);
        }
        break;
    }
    previous_layer = layer;
    return state;
}

bool caps_word_press_user(uint16_t keycode) {
  switch (keycode) {
    // Keycodes that continue Caps Word, with shift applied.
    case KC_A ... KC_Z:
    case KC_MINS:
        add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
        return true;

    // Keycodes that continue Caps Word, without shifting.
    case KC_1 ... KC_0:
    case KC_BSPC:
    case KC_DEL:
    case KC_UNDS:
    case KC_SCLN:     // For spanish Ñ
        return true;

    default:
        return false;  // Deactivate Caps Word.
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool continue_processing = true;
    switch (keycode) {
        case KC_LSFT:
            if (record->event.pressed) {
                uprintf("LSHIFT pressed\n");
            } else {
                uprintf("LSHIFT released\n");
            }
            break;
        case KC_RSFT:
            if (get_mods() & MOD_BIT(KC_LSFT)) {
                // unregister left shift because it's stuck!!
                print("Heal LSHIFT 1\n");
                unregister_code(KC_LSFT);
            }
            break;
        case KC_CAPS:
            if (get_mods() & MOD_BIT(KC_LSFT)) {
                // unregister left shift because it's stuck!!
                print("Heal LSHIFT 2\n");
                unregister_code(KC_LSFT);
            }
            if (record->event.pressed) uprintf("CAPS\n");
            break;
        case KC_TAB:
            if (record->event.pressed) uprintf("TAB\n");
            break;
        case KC_VOLU:
            uprintf("KL: kc: 0x%04X encoder rotated right\n", keycode);
            return true;
        case KC_VOLD:
            uprintf("KL: kc: 0x%04X encoder rotated left\n", keycode);
            return true;
        case QK_CLEAR_EEPROM:
            print("Erase EEPROM\n");
            return true;
        case C_HEAL:
            print("Heal LSHIFT 3\n");
            clear_mods();
            return true;
        case C_CALIB:
            if (record->event.pressed) {
                print("Calibration...\n");
                get_sensor_offsets();
                #ifdef DEBUG_ENABLE
                    debug_calibrate_data();
                #endif
            }
            return true;
    #ifdef AUDIO_ENABLE
        case C_SONG1:
            PLAY_SONG(song_1);
            return false;
        case C_SONG2:
            PLAY_SONG(song_2);
            return false;
         case C_SONG3:
            PLAY_SONG(song_3);
            return false;
        case C_SONG4:
            PLAY_SONG(song_4);
            return false;
    #endif
        case M_PAD_1:
            midi_send_percussion(GENERAL_MIDI_CRASH_CYMBAL_1);
            break;
        case M_PAD_2:
            midi_send_percussion(GENERAL_MIDI_LOW_BONGO);
            break;
        case SS_MAIL:
            if (record->event.pressed) {
                SEND_STRING("@gmail.com");
            }
            return false;
        case MIDI_TONE_MIN ... MIDI_TONE_MAX:
            #ifdef MIDI_CUSTOM_ENABLE
                process_midi_user(keycode, record->event.key.row, record->event.key.col, record->event.pressed);
                return false;   // skip the normal key handling
            #endif
            break;
        case KC_UP:
            //rc = process_hard_key(record->event.key.row, record->event.key.col, KC_HOME)
            break;
        case KC_DOWN:
            //rc = process_hard_key(record->event.key.row, record->event.key.col, KC_END);
            break;
        case KC_LEFT:
            //rc = process_hard_key(record->event.key.row, record->event.key.col, KC_PGUP);
            break;
        case KC_RGHT:
            //rc = process_hard_key(record->event.key.row, record->event.key.col, KC_PGDN);
            break;
        default:
            break;
    }

    // If console is enabled, it will print the matrix position and status of each key pressed
    #ifdef CONSOLE_ENABLE
        uint8_t row = record->event.key.row;
        uint8_t col = record->event.key.col;
        analog_key_t *key = &keys[row][col];
        #ifdef KEYCODE_STRING_ENABLE
        const char* key_name = get_keycode_string(keycode);
        #else
        const char* key_name = "KL";
        #endif

        // if (keycode == KC_LEFT_SHIFT || keycode == KC_RIGHT_SHIFT || keycode == KC_CAPS) {
            if (!supported_key_velocity(row, col)) {
                uprintf("%-7s: kc: 0x%04X row: %2u col: %2u pressed: %u time: %5u int: %u count: %u\n",
                    key_name, keycode, row, col, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
            } else {
                char debugText[30];
                sprintf(debugText, "%s [%d]\n", key_name, key->velocity);
                oled_write(debugText, false);
                uprintf("%-7s: kc: 0x%04X row: %2u col: %2u pressed: %u time: %5u int: %u count: %u raw: %4u dist: %3u vel: %3u\n",
                    key_name, keycode, row, col, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count,
                    key->raw, key->value, key->velocity);
            }
        // }
        reset_key_velocity(key);
    #endif
    return continue_processing;
}
