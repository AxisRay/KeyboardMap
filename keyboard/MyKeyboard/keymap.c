/*
Copyright 2014 Ralf Schmitt <ralf@bunkertor.net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "keycode.h"
#include "action.h"
#include "action_macro.h"
#include "report.h"
#include "host.h"
#include "debug.h"
#include "keymap.h"

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
/*
 Matrix col/row mapping

 ,----.    ,-------------------. ,-------------------. ,-------------------. ,--------------.
 |06/6|    |07/4|08/4|08/2|08/6| |15/5|11/6|12/2|12/4| |14/4|14/5|14/6|14/0| |13/5|13/7|15/7|
 `----'    `-------------------' `-------------------' `-------------------' `--------------'
 ,-------------------------------------------------------------------------. ,--------------. ,-------------------.
 |06/4|06/5|07/5|08/5|09/5|09/4|10/4|10/5|11/5|12/5|05/5|05/4|11/4|    14/2| |17/4|02/4|04/4| |16/1|17/1|04/1|04/0|
 |-------------------------------------------------------------------------| |--------------| |-------------------|
 |06/2  |06/7|07/7|08/7|09/7|09/2|10/2|10/7|11/7|12/7|05/7|05/2|11/2|  14/3| |16/4|02/5|04/5| |16/7|17/7|04/7|    |
 |-------------------------------------------------------------------------| '--------------' |-------------- 02/7|
 |02/7   |06/3|07/3|08/3|09/3|09/6|10/6|10/3|11/3|12/3|05/3|05/6|      14/1|                  |16/2|17/2|04/2|    |
 |-------------------------------------------------------------------------|      ,----.      |-------------------|
 |01/2     |06/1|07/1|08/1|09/1|09/0|10/0|10/1|11/1|12/1|05/0|         01/3|      |02/6|      |16/3|17/3|04/3|    |
 |-------------------------------------------------------------------------| ,--------------. |-------------- 02/3|
 |15/4|03/2|13/6|                 16/6                  |13/0|0/3|12/0|15/1| |02/0|16/0|17/0| | 17/6    |04/6|    |
 `-------------------------------------------------------------------------' `--------------' `-------------------'
*/

#define KEYMAP( \
    KB3, KC6, KD6, KD1, KD3, KA7, KG3, KH1, KH6, KK6, KK7, KK3, KK5, KP3, KP4, KQ6, KS6, KS3, \
    KB6, KB7, KC7, KD7, KE7, KE6, KF6, KF7, KG7, KH7, KI7, KI6, KG6,    KK1  , KJ7, KJ0, KA0, \
     KB1  , KB0, KC0, KD0, KE0, KE1, KF1, KF0, KG0, KH0, KI0, KI1, KG1,  KK2 , KM6, KO6, KN6, \
     KC1    , KB2, KC2, KD2, KE2, KE3, KF3, KF2, KG2, KH2, KI2, KI3,   KK4   , KL6, KO7, KN7, \
       KP1     , KB4, KC4, KD4, KE4, KE5, KF5, KF4, KG4, KH4, KI5,    KP2    ,      KO3,      \
    KA6, KQ1 , KJ3 ,                KL3,               KJ5 , KR2 , KH5 , KA4 , KO5, KL5, KM5  \
) { \
/*            0         1         2         3         4         5         6         7   */  \
/* A  0 */ {KC_##KA0, KC_NO,    KC_NO,    KC_NO,    KC_##KA4, KC_NO,    KC_##KA6, KC_##KA7},\
/* B  1 */ {KC_##KB0, KC_##KB1, KC_##KB2, KC_##KB3, KC_##KB4, KC_NO,    KC_##KB6, KC_##KB7},\
/* C  2 */ {KC_##KC0, KC_##KC1, KC_##KC2, KC_NO,    KC_##KC4, KC_NO,    KC_##KC6, KC_##KC7},\
/* D  3 */ {KC_##KD0, KC_##KD1, KC_##KD2, KC_##KD3, KC_##KD4, KC_NO,    KC_##KD6, KC_##KD7},\
/* E  4 */ {KC_##KE0, KC_##KE1, KC_##KE2, KC_##KE3, KC_##KE4, KC_##KE5, KC_##KE6, KC_##KE7},\
/* F  5 */ {KC_##KF0, KC_##KF1, KC_##KF2, KC_##KF3, KC_##KF4, KC_##KF5, KC_##KF6, KC_##KF7},\
/* G  6 */ {KC_##KG0, KC_##KG1, KC_##KG2, KC_##KG3, KC_##KG4, KC_NO,    KC_##KG6, KC_##KG7},\
/* H  7 */ {KC_##KH0, KC_##KH1, KC_##KH2, KC_NO,    KC_##KH4, KC_##KH5, KC_##KH6, KC_##KH7},\
/* I  8 */ {KC_##KI0, KC_##KI1, KC_##KI2, KC_##KI3, KC_NO,    KC_##KI5, KC_##KI6, KC_##KI7},\
/* J  9 */ {KC_##KJ0, KC_NO,    KC_NO,    KC_##KJ3, KC_NO,    KC_##KJ5, KC_NO,    KC_##KJ7},\
/* K 10 */ {KC_NO,    KC_##KK1,    KC_##KK2, KC_##KK3, KC_##KK4, KC_##KK5, KC_##KK6, KC_##KK7},\
/* L 11 */ {KC_NO,    KC_NO,    KC_NO,    KC_##KL3, KC_NO,    KC_##KL5, KC_##KL6, KC_NO   },\
/* M 12 */ {KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##KM5, KC_##KM6, KC_NO   },\
/* N 13 */ {KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##KN6, KC_##KN7},\
/* O 14 */ {KC_NO,    KC_NO,    KC_NO,    KC_##KO3, KC_NO,    KC_##KO5, KC_##KO6, KC_##KO7},\
/* P 15 */ {KC_NO,    KC_##KP1, KC_##KP2, KC_##KP3, KC_##KP4, KC_NO,    KC_NO,    KC_NO   },\
/* Q 16 */ {KC_NO,    KC_##KQ1, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##KQ6, KC_NO   },\
/* R 17 */ {KC_NO,    KC_NO,    KC_##KR2, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO   },\
/* S 18 */ {KC_NO,    KC_NO,    KC_NO,    KC_##KS3, KC_NO,    KC_NO,    KC_##KS6, KC_NO   } \
}

#include "keymap_ansi.h"

#define KEYMAPS_SIZE    (sizeof(keymaps) / sizeof(keymaps[0]))
#define FN_ACTIONS_SIZE (sizeof(fn_actions) / sizeof(fn_actions[0]))

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer < KEYMAPS_SIZE) {
        return pgm_read_byte(&keymaps[(layer)][(key.col)][(key.row)]);
    } else {
        return pgm_read_byte(&keymaps[0][(key.col)][(key.row)]);
    }
}

/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    action_t action;
    if (FN_INDEX(keycode) < FN_ACTIONS_SIZE) {
        action.code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]);
    } else {
        action.code = ACTION_NO;
    }
    return action;
}

void action_function(keyrecord_t *event, uint8_t id, uint8_t opt)
{
    if (id == TEENSY_KEY) {
        clear_keyboard();
        
        print("\n\nJump to bootloader..");
        wdt_disable();
        _delay_ms(100);

        cli();
        UDCON = 1;
        USBCON = (1<<FRZCLK);  // disable USB
        UCSR1B = 0;
        _delay_ms(5);

        EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0;
        TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0; UCSR1B = 0; TWCR = 0;
        DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
        PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;
        asm volatile("jmp 0xFC00");

        print("not supported.\n");

    }
}
