/*
 * led_strip.c
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


#include <avr/io.h>

#include "led_strip.h"

void led_strip_init() {
 DDRD |= _BV(PD5);
}

inline void high() {
 PORTD |= _BV(PD5);
}

inline void low() {
 PORTD &= ~_BV(PD5);
}

inline void NRZ_PWM_bit_high() {
 high();
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
 low();
}

inline void NRZ_PWM_bit_low() {
 high();
 low();
 asm volatile ("nop");
 asm volatile ("nop");
 asm volatile ("nop");
}

inline void NRZ_PWM_bit( int bit ) {
 if( bit ) {
  NRZ_PWM_bit_high();
 } else {
  NRZ_PWM_bit_low();
 }
}

inline void NRZ_PWM_byte( uint8_t value ) {
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
	value <<= 1;
	NRZ_PWM_bit(value & 128);
 /*int8_t i;
 i=8;
 do{
  NRZ_PWM_bit(value & 1);
  value >>=1;
  i--;
 }while(i);*/
}

int mirror(int value, int upper) {
    upper -= 1;
    value = value % (2*upper);
    if( value >= upper ) {
        value = upper - (value%upper);
    }
    return value;
}

uint8_t leds[MAX_COLOR];

void scan_strip() {
	int count = MAX_COLOR - 1;
	while(1) {
		NRZ_PWM_byte(leds[count]);
		if (count == 0) break;
		count--;
	}
}

void led_strip_set_color(int pos, uint8_t color) {
	leds[pos] = color>>2; //not so bright
}

int write_color_pos;

void leds_write_color(uint8_t color) {
	led_strip_set_color(write_color_pos, color);
	write_color_pos = (write_color_pos + 1) % MAX_COLOR;
}

void led_strip_write_colors(uint8_t green, uint8_t red, uint8_t blue) {
	write_color_pos -= write_color_pos%3;
	leds_write_color(blue);
	leds_write_color(red);
	leds_write_color(green);
}

void led_strip_set_colors(int pos, uint8_t green, uint8_t red, uint8_t blue) {
	led_strip_set_color(pos*3 + 2, green);
	led_strip_set_color(pos*3 + 1, red);
	led_strip_set_color(pos*3, blue);
}


void led_strip_set(int pos, struct Led *led_color) {
	led_strip_set_color(pos*3 + 2, led_color->g);
	led_strip_set_color(pos*3 + 1, led_color->r);
	led_strip_set_color(pos*3,     led_color->b);
//	led_strip_set_color(pos*3 + 2, led_color.g);
//	led_strip_set_color(pos*3 + 1, led_color.r);
//	led_strip_set_color(pos*3,     led_color.b);
}


void clear_strip(uint8_t g, uint8_t r, uint8_t b) {
	for (int i = 0; i < STRIP_LENGTH; i++) {
		led_strip_set_colors(i, g, r, b);
	}
}


