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
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"

#ifndef DEBOUNCE
#   define DEBOUNCE 0
#endif
static uint8_t debouncing = DEBOUNCE;

static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static uint8_t read_rows(void);
static void init_rows(void);
static void unselect_cols(void);
static void select_col(uint8_t col);

inline uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    unselect_cols();
    init_rows();
    for (uint8_t i=0; i < MATRIX_ROWS; i++)  {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }
}

uint8_t matrix_scan(void)
{
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        select_col(col);
        _delay_us(3);
        uint8_t rows = read_rows();
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            bool prev_bit = matrix_debouncing[row] & ((matrix_row_t)1<<col);
            bool curr_bit = rows & (1<<row);
            if (prev_bit != curr_bit) {
                matrix_debouncing[row] ^= ((matrix_row_t)1<<col);
                debouncing = DEBOUNCE;
            }
        }
        unselect_cols();
    }

    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        xprintf("%02X: %032lb\n", row, bitrev32(matrix_get_row(row)));
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop32(matrix[i]);
    }
    return count;
}

/* Row pin configuration
 *
 * row: 0    1    2    3    4    5    6    7
 * pin: PB1  PC2  PB6  PB4  PB3  PB5  PB0  PB2
 *
 */
static void init_rows(void)
{
    DDRF &= ~0b11111111;
    PORTF |= 0b11111111;
}

static uint8_t read_rows(void)
{
    return (PINF&(1<<0) ? 0 : (1<<0)) |
           (PINF&(1<<1) ? 0 : (1<<1)) |
           (PINF&(1<<2) ? 0 : (1<<2)) |
           (PINF&(1<<3) ? 0 : (1<<3)) |
           (PINF&(1<<4) ? 0 : (1<<4)) |
           (PINF&(1<<5) ? 0 : (1<<5)) |
           (PINF&(1<<6) ? 0 : (1<<6)) |
           (PINF&(1<<7) ? 0 : (1<<7));
}

/*  These columns uses two 74HC42 4 to 10 bit demultiplexers (low active).
 *
 *   COL PD6 PD5 PD4 PD3 PD2 PD1
 *   10   1   1   0   0   0   0
 *   15   1   1   0   0   0   1
 *    8   1   1   0   0   1   0
 *   14   1   1   0   1   0   0
 *    6   1   1   0   1   0   1
 *   13   1   1   0   1   1   0
 *   12   1   1   1   0   0   0
 *    9   1   1   1   0   1   0
 *   11   1   1   1   1   0   0
 *    7   1   1   1   1   1   0
 *
 *   COL PD1 PD2 PD3 PD4 PD5 PD6
 *    3   1   1   0   0   0   1
 *    4   1   1   0   0   1   0
 *   17   1   1   0   1   0   0
 *   16   1   1   0   1   1   0
 *    0   1   1   1   0   0   1
 *    5   1   1   1   0   1   0
 *    2   1   1   1   1   0   0
 *    1   1   1   1   1   1   0
 */
static void unselect_cols(void)
{
    DDRD &= ~0b11111111;
    PORTD &= ~0b11111111;
    DDRC &= ~0b11111111;
    PORTC &= ~0b11111111;
    DDRE &= ~0b00000011;
    PORTE &= ~0b00000011;
    DDRB &= ~0b10000000;
    PORTB &= ~0b10000000;
}

static void select_col(uint8_t col)
{
    switch (col) {
        case 0:
            DDRB  |= (1<<7);
            PORTB &= ~(1<<7);
            break;
        case 1:
            DDRD  |= (1<<0);
            PORTD &= ~(1<<0);
            break;
        case 2:
            DDRD  |= (1<<1);
            PORTD &= ~(1<<1);
            break;
        case 3:
            DDRD  |= (1<<2);
            PORTD &= ~(1<<2);
            break;
        case 4:
            DDRD  |= (1<<3);
            PORTD &= ~(1<<3);
            break;
        case 5:
            DDRD  |= (1<<4);
            PORTD &= ~(1<<4);
            break;
        case 6:
            DDRD  |= (1<<5);
            PORTD &= ~(1<<5);
            break;
        case 7:
            DDRD  |= (1<<6);
            PORTD &= ~(1<<6);
            break;
        case 8:
            DDRD  |= (1<<7);
            PORTD &= ~(1<<7);
            break;
        case 9:
            DDRE  |= (1<<0);
            PORTE &= ~(1<<0);
            break;
        case 10:
            DDRE  |= (1<<1);
            PORTE &= ~(1<<1);
            break;
        case 11:
            DDRC  |= (1<<0);
            PORTC &= ~(1<<0);
            break;
        case 12:
            DDRC  |= (1<<1);
            PORTC &= ~(1<<1);
            break;
        case 13:
            DDRC  |= (1<<2);
            PORTC &= ~(1<<2);
            break;
        case 14:
            DDRC  |= (1<<3);
            PORTC &= ~(1<<3);
            break;
        case 15:
            DDRC  |= (1<<4);
            PORTC &= ~(1<<4);
            break;
        case 16:
            DDRC  |= (1<<5);
            PORTC &= ~(1<<5);
            break;
        case 17:
            DDRC  |= (1<<6);
            PORTC &= ~(1<<6);
            break;
        case 18:
            DDRC  |= (1<<7);
            PORTC &= ~(1<<7);
            break;
    }
}
