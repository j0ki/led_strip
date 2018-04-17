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

#include "time_defs.h"

#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "led_strip.h"
#include "snake.h"

#include "coil.h"

#include "pendulum.h"

#include "timer_and_watchdog.h"

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

void process_debug_led(uint16_t current, uint16_t mod)
{
    current %= mod;
    if (current < mod / 2) {
        debug_led_off();
    } else {
        debug_led_on();
    }
}

#define PUSH_TIME (1250) // in milliseconds
#define DEFAULT_TIME (1250) // in milliseconds
#define MINIMUM_TIME (100) //in milliseconds
#define MAXIMUM_TIME (1300) //in milliseconds

uint16_t measured_time_left;
uint16_t measured_time_right;
int side;
#define LEFT (1) // left and right really depends on perspective.....
#define RIGHT (!LEFT)

uint16_t get_target_time()
{
	uint16_t target_time = DEFAULT_TIME;
    if (LEFT == side) {
        if (measured_time_left > MINIMUM_TIME) {
            target_time = measured_time_left;
        } else if (measured_time_right > MINIMUM_TIME) {
            target_time = measured_time_right;
        }
    } else {
        if (measured_time_right > MINIMUM_TIME) {
            target_time = measured_time_right;
        } else if (measured_time_left > MINIMUM_TIME) {
            target_time = measured_time_left;
        }
    }
	if (MAXIMUM_TIME < target_time) {
		target_time = MAXIMUM_TIME;
	}
	return target_time;
}

void process_pendulum(uint16_t time)
{
    side = !side;
    if (side == RIGHT) {
        measured_time_left = time;
    } else {
        measured_time_right = time;
    }
}

struct Snake snake;

void init_snake()
{
    snake_init(&snake);
    snake_color_mode(0);
    for (int i = 0; i < 30; i++) {
        snake_set_position(&snake, i);
        snake_draw(&snake);
        cli();
        scan_strip();
        sei();
        _delay_ms(10);
    }
}

uint16_t sensor_last_edge;
int last_sensor = 0;

// improve timing and coordination between pendulum and ledsnake
void process_coil_sensed()
{
    if (side == RIGHT) {
        snake_color_mode(0);
    } else {
        snake_color_mode(1);
    }
}

#define P_RESET (0)
#define P_TESTING (1)
#define P_SWINGING (2)

int main(int argc, char** argv)
{
    debug_led_init();
    coil_init();
    timer_init();
    led_strip_init();

    init_snake();

	int pendulum_state = P_TESTING;

    int p_position = 0;

    uint16_t coil_active = 0;

    timer_reset();
    while (1) {
        uint16_t milliseconds = get_time_milliseconds();

        if (P_RESET == pendulum_state) {
            process_debug_led(milliseconds, 500);
            if (milliseconds >= PUSH_TIME * 11 / 3) {
                pendulum_state = P_TESTING;
                timer_reset();
                continue;
            }

            if ((PUSH_TIME / 4) > (milliseconds % PUSH_TIME)) {
                coil_on();
            } else {
                coil_off();
            }
        }

        if (P_TESTING == pendulum_state) {
            process_debug_led(milliseconds, 250);
            if (milliseconds >= 2 * PUSH_TIME) {
                pendulum_state = P_RESET;
                timer_reset();
                continue;
            }
            if (coil_sensor_adc()) {
                pendulum_state = P_SWINGING;
                timer_reset();
                continue;
            }
        }

        if (P_SWINGING == pendulum_state) {
            if (coil_active) {
                debug_led_on();
            } else {
                debug_led_off();
            }
            if (milliseconds >= 5000) {
                pendulum_state = P_RESET;
                timer_reset();
                continue;
            }

            uint16_t target_time = get_target_time();

            // the result is a fixed point number between 0 and 65536
            uint32_t relative_time = (((uint32_t)milliseconds) << 16) / target_time;

            unsigned int current_position = get_position(relative_time);

            unsigned int snake_offset = (RIGHT == side) ? 30 : 59;

            snake_set_position(&snake, snake_offset + current_position);

            if (current_position > p_position) {
                snake_draw(&snake);
                cli();
                scan_strip();
                sei();
            }
            p_position = current_position;

            if (target_time / 4 + 50 < milliseconds) {
                coil_active = 0;
            } else if (target_time / 4 < milliseconds) {
                coil_off();
            } else if (10 < milliseconds) {
                coil_on();
            }

            if (coil_active) {
                continue;
            }

            int sensor = coil_sensor_adc();
            if (sensor != last_sensor && milliseconds > sensor_last_edge) {
                sensor_last_edge = milliseconds;
                last_sensor = sensor;
                if (!sensor) {
                    process_coil_sensed();
                } else {
                    process_pendulum(milliseconds);
                    snake_color_mode(2);
                    coil_active = 1;
                    sensor_last_edge = 0;
                    timer_reset();
                }
            }
        }
    }
    return 0;
}
