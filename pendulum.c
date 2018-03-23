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
}

uint16_t position_lut_30[] = {
1391,
2789,
4200,
5631,
7089,
8584,
10128,
11734,
13423,
15222,
17172,
19344,
21872,
25107,
32768,
40429,
43664,
46192,
48364,
50314,
52113,
53802,
55408,
56952,
58447,
59905,
61336,
62747,
64144,
65535
}

uint16_t alt_lut[] = {
0,
179,
716,
1603,
2832,
4390,
6258,
8416,
10841,
13507,
16383,
19440,
22642,
25955,
29342,
36194,
39581,
42894,
46096,
49153,
52029,
54695,
57120,
59278,
61146,
62704,
63933,
64820,
65357,
65535
}

int get_position(uint16_t time) {
    for( int i = 0; i < 28; i++ ) {
        if( time <= position_lut[i] ) {
            return i;
        }
    }
    return 28; // this will not happen
}
