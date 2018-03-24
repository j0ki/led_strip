/*
 * pendulum.c
 *
 * Copyright 2018 Lukas Fischer <joki@joki-laptop>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

// 28 positions
uint16_t position_lut[] = {
    1491,
    2990,
    4505,
    6044,
    7618,
    9239,
    10922,
    12688,
    14565,
    16596,
    18854,
    21480,
    24835,
    32768,
    40700,
    44055,
    46681,
    48939,
    50970,
    52847,
    54613,
    56296,
    57917,
    59491,
    61030,
    62545,
    64044,
    65535
};

unsigned int get_position(uint16_t time)
{
    for (int i = 0; i < 28; i++) {
        if (time <= position_lut[i]) {
            return i;
        }
    }
    return 28; // this will not happen
}
