
static const uint8_t PROGMEM keymaps[][MATRIX_COLS][MATRIX_ROWS] = {
    /* Layer 0: Standard ANSI layer */
    KEYMAP(\
         ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, VOLD, VOLU ,  MUTE,CALC,MAIL, \
         GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS, EQL,   BSPC    ,  PSCR,SLCK,PAUS, \
          TAB,   Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,  BSLS  ,  INS, HOME,PGUP, \
          CAPS ,   A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN, QUOT,     ENT  ,  DEL, END, PGDN, \
           LSFT  ,   Z,   X,   C,   V,   B,   N,   M,   COMM, DOT, SLSH,     RSFT    ,        UP,       \
         LCTL,FN1,LALT,               SPC,                RALT , RGUI , FN0  , RCTL  ,  LEFT,DOWN,RGHT),\
    /* Layer 1: Function layer */
    KEYMAP(\
         TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS, TRNS ,  WAKE, PWR,SLEP, \
         TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,   TRNS    ,  TRNS,TRNS,TRNS, \
          TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS, TRNS,   TRNS  ,  TRNS,TRNS,TRNS, \
          TRNS ,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,        TRNS  ,  TRNS,TRNS,TRNS, \
           TRNS  ,TRNS,TRNS,TRNS,TRNS, FN2,TRNS,TRNS,   TRNS, TRNS, TRNS,    TRNS    ,       MSTP,      \
         TRNS,TRNS,TRNS,              TRNS,               TRNS , TRNS , TRNS , TRNS  ,  MPRV,MPLY,MNXT  )
};

enum function_id {
    TEENSY_KEY,
};

static const uint16_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_MOMENTARY(1),
    [2] = ACTION_FUNCTION(TEENSY_KEY)

};
