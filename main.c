/*
 * main.c
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

#define F_CPU 8000000UL // 8 MHz
//#define F_CPU 14.7456E6
#include <util/delay.h>

#include <avr/interrupt.h>

#include "led_strip.h"
#include "snake.h"

#include "adc.h"

#include "pendulum.h"

#include "timer_and_watchdog.h"

#include <avr/io.h>

void debug_led_init()
{
    DDRD |= _BV(PD1);
}

void debug_led_on()
{
    PORTD &= ~_BV(PD1);
}

void debug_led_off()
{
    PORTD |= _BV(PD1);
}

void process_debug_led()
{
    if (get_error_level() == 0) {
        debug_led_off();
    } else {
        debug_led_on();
    }
}

void coil_init()
{
    //coil actor output pin
    DDRD |= _BV(PD0);

    //coil sensor int0 input pin
    DDRD &= ~_BV(PD2);

    //maybe use int0 later for finer tuning
    //general interupt control register:
    //enable int0
    //GICR |= _BV(INT0);

    //rising edge triggers int0
    //MCUCR |= _BV(ISC00);
    //MCUCR |= _BV(ISC01);
}

void coil_on()
{
    PORTD |= _BV(PD0);
}

void coil_off()
{
    PORTD &= ~_BV(PD0);
}

#define DEFAULT_TIME (34000)

uint16_t measured_time_left;
uint16_t measured_time_right;
int side;
#define LEFT (1) // left and right really depends on perspective.....
#define RIGHT (!LEFT)

uint16_t get_target_time()
{
    if (LEFT == side) {
        if (measured_time_left > 1000) {
            return measured_time_left;
        } else if (measured_time_right > 1000) {
            return measured_time_right;
        } else {
            return DEFAULT_TIME;
        }
    } else {
        if (measured_time_right > 1000) {
            return measured_time_right;
        } else if (measured_time_left > 1000) {
            return measured_time_left;
        } else {
            return DEFAULT_TIME;
        }
    }
}

void process_pendulum(uint16_t time)
{
    side = !side;
    if (side == RIGHT)
        measured_time_left = time;
    else
        measured_time_right = time;
}

struct Snake snake;

void init_snake()
{
    snake_init(&snake);
    for (int i = 0; i < 30; i++) {
        snake_set_position(&snake, i);
        snake_draw(&snake);
        cli();
        scan_strip();
        sei();
        _delay_ms(10);
    }
}

uint16_t sensor_time;

// improve timing and coordination between pendulum and ledsnake
void process_coil_sensed(uint16_t coil_sensed_time)
{
    sensor_time = coil_sensed_time; //TODO: fine tune
}

int main(int argc, char** argv)
{
    debug_led_init();
    ADC_Init();
    coil_init();
    timer_and_watchdog_init();
    led_strip_init();

    init_snake();

    int p_position = 0;

    int speed = 0;

    int coil_sensed = 0;

    timer_reset();
    while (1) {
        process_debug_led();

        uint16_t time = get_time();

        if (get_error_level() > 0) {
            p_position = 0;
            speed = 0;
            snake_set_position(&snake, 0);
        }

        if (get_error_level() >= 4) {
            if (time > 35000)
                timer_reset();
            if (time < 10000) {
                coil_on();
            } else {
                coil_off();
            }
        }

        //if ( !(PIND & _BV(PD2)) ) {
        if (ADC_Read() < 200) {
            if (0 == coil_sensed) {
                timer_reset();
                coil_sensed = 1;
                speed += 1;
            } else {
                coil_sensed = (0 < time) ? time : 1;
            }

            set_error_level(0);

            process_pendulum(time);

            p_position = 0;

        } else {
            if (0 < coil_sensed) {
                process_coil_sensed(coil_sensed);
            }
            coil_sensed = 0;
        }

        uint16_t target_time = get_target_time() + sensor_time;

        unsigned int snake_offset = (RIGHT == side) ? 29 : 58;

        if (time < sensor_time / 2) {
            time = 0;
            if (time < sensor_time/4) {
                snake_offset -= 1;
            }
        } else {
            time = time - sensor_time / 2;
        }

        //~ uint16_t current_position = ((uint32_t)time) * 30 / ((uint32_t)target_time);
        //~ uint16_t current_position = time / (target_time / 30);
        //this should be quicker, but it's 2 extra steps:
        //~ uint16_t current_position = time / (target_time/32);

        // the result is a fixed point number between 0 and 65536
        uint32_t relative_time = (((uint32_t)time) << 16) / target_time;
        // this can happen, when the pendulum period changes.
        if ((1 << 16) >= relative_time) {
            relative_time = (1 << 16) - 1;
        }
        unsigned int current_position = get_position(relative_time);

        if (1 < current_position && current_position < 10) {
            coil_on();
        } else {
            coil_off();
        }

        snake_set_position(&snake, snake_offset + current_position);

        if (current_position > p_position) {
            snake_draw(&snake);
            cli();
            scan_strip();
            sei();
        }
        p_position = current_position;
    }
    return 0;
}
