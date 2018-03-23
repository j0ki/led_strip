/*
 * led_strip.h
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

#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__


#include <stdint.h>


#define STRIP_LENGTH (30)

#define MAX_COLOR (STRIP_LENGTH*3)

struct Led {
    uint8_t g;
    uint8_t r;
    uint8_t b;
};

void led_strip_init();

int mirror(int value, int upper);

void scan_strip();

void leds_write_color(uint8_t color);
void led_strip_write_colors(uint8_t green, uint8_t red, uint8_t blue);
void led_strip_set_colors(int pos, uint8_t green, uint8_t red, uint8_t blue);
void led_strip_set(int pos, struct Led *led_color);

//void set_led(int pos, struct Led color);
void set_led_colors(int pos, uint8_t g, uint8_t r, uint8_t b);
void clear_strip(uint8_t g, uint8_t r, uint8_t b);
//void clear_strip(struct Led color);

#endif // __LED_STRIP_H__
