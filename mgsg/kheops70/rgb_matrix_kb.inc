RGB_MATRIX_EFFECT(QWERTY_MODE)
RGB_MATRIX_EFFECT(MIDI_MODE)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

/* Specific keys position */
#define LSHIFT_LED_INDEX            40          // LEFT SHIFT KEY, USED TO SIGNAL CAPS LOCK
#define CAPS_LOCK_LED_INDEX         39          // CAPS LOCK KEY, USED AS TAB
// #define CAPS_LOCK_LED_INDEX         40
#define ENTER_LED_INDEX             27
#define BACKSPACE_LED_INDEX         26
#define DELETE_LED_INDEX            54
#define PGDN_LED_INDEX              55
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
#define RGB_GREEN_24       17, 61, 23

#define RGB_YELLOW_12      30, 30, 1
#define RGB_RED_12         30, 8, 8
#define RGB_PINK_12        30, 0, 30
#define RGB_GREEN_12       8, 30, 12

#define RGB_MY_ORANGE      RGB_ORANGE_32
#define RGB_MY_BLUE        RGB_BLUE_32
#define RGB_MY_CYAN        RGB_CYAN_32
#define RGB_MY_GREEN       RGB_GREEN_32
#define RGB_MY_RED         RGB_RED_32
#define RGB_MY_WHITE       RGB_WHITE_32
#define RGB_MY_YELLOW      RGB_YELLOW_32
#define RGB_MY_PINK        RGB_PINK_32

// typedef union {
//     uint8_t raw;
//     struct {
//         bool    num_lock : 1;
//         bool    caps_lock : 1;
//         bool    scroll_lock : 1;
//         bool    compose : 1;
//         bool    kana : 1;
//         uint8_t reserved : 3;
//     };
// } led_t;
// extern led_t host_keyboard_led_state(void);
extern bool caps_lock_on;

// e.g: A simple effect, self-contained within a single method
static bool QWERTY_MODE(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);
//   for (uint8_t i = led_min; i < led_max; i++) {
//     rgb_matrix_set_color(i, 0xff, 0xff, 0x00);
//   }

    //hsv_t hsv = {0, rgb_matrix_config.hsv.s, rgb_matrix_config.hsv.v};
    //rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
    //rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);

    // if (host_keyboard_led_state().caps_lock) {
    if (caps_lock_on) {
        rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, RGB_GREEN_24);
    } else {
        rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, 0, 0, 0);
    }

    rgb_matrix_set_color(NUM1_LED_INDEX, RGB_MY_YELLOW);
    rgb_matrix_set_color(ENTER_LED_INDEX, RGB_YELLOW_12);
    //rgb_matrix_set_color(DELETE_LED_INDEX, RGB_MY_RED);
    rgb_matrix_set_color(BACKSPACE_LED_INDEX,RGB_RED_12);
    //rgb_matrix_set_color(RALT_LED_INDEX, RGB_MY_GREEN);
    //rgb_matrix_set_color(LALT_LED_INDEX, RGB_MY_GREEN);
    rgb_matrix_set_color(RCTRL_LED_INDEX, RGB_PINK_12);
    rgb_matrix_set_color(LCTRL_LED_INDEX, RGB_PINK_12);
    //rgb_matrix_set_color(LFN_LED_INDEX, RGB_MY_YELLOW);
    //rgb_matrix_set_color(RFN_LED_INDEX, RGB_MY_YELLOW);


  return rgb_matrix_check_finished_leds(led_max);
}

static bool MIDI_MODE(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    rgb_matrix_set_color(NUM3_LED_INDEX, RGB_YELLOW_12);
    rgb_matrix_set_color(PGDN_LED_INDEX, RGB_RED_12);

    // for(int i=0;i<=39;i++) {
    //     rgb_matrix_set_color(i, RGB_BLACK);
    // }
    // for(int i=40;i<=54;i++) {
    //     rgb_matrix_set_color(i, RGB_WHITE);
    // }
    // for(int i=56;i<=69;i++) {
    //     rgb_matrix_set_color(i, RGB_WHITE);
    // }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
