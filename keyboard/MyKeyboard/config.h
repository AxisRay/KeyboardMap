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

#ifndef CONFIG_H
#define CONFIG_H

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED // 0x2516 (original CM XT value)
#define PRODUCT_ID      0x001A
#define DEVICE_VER      0x0000
#define MANUFACTURER    Cooler Master
#define PRODUCT         Cooler Master QuickFire XT

/* message strings */
#define DESCRIPTION     t.m.k. keyboard firmware for Cooler Master QuickFire XT

/* matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 19

/* Set 0 if need no debouncing */
#define DEBOUNCE    5

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_RGUI) | MOD_BIT(KC_RCTRL)) \
)

#endif