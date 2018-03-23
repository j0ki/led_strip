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

#include <avr/io.h>

void debug_led_on() {
    PORTD &= ~_BV(PD1);
}

void debug_led_off() {
    PORTD |= _BV(PD1);
}

volatile int error_level;

void error_level_init() {
    error_level = 5;
}

inline void watchdog_bark() {
    if ( error_level < 10 ) {
        error_level += 3;
    }
}

ISR(TIMER1_OVF_vect) {
    watchdog_bark();
}

void timer_reset() {
    cli();
    //the order of writing registers is important
    //_T_imer _C_ou_NT_er for timer_1_ (high and low)
    TCNT1H = 0;
    TCNT1L = 0;
    sei();
}

void timer_init() {
    //disable all interrupt while setting up timer
    cli();
    //timer interrupt mask register
    //TIMSK1

    //timer control register A
    //TCCR1A

    //TCCR1B: timer control register B
    //set prescaler to 1:256
    // with 8MHz -> overflow after 2.097152 seconds
    //  and resolution: 32us
    TCCR1B |= _BV(CS12);
    TCCR1B &= ~_BV(CS11);
    TCCR1B &= ~_BV(CS10);

    //TIMSK: timer interrupt mask
    //(general timer interrupt register)

    // _T_imer _O_verflow _I_nterrupt _E_nable at timer_1_
    TIMSK |= _BV(TOIE1);



    //the order of writing registers is important
    //set timer compare register to 2 seconds
    //OCR1H = 62000 / 256;
    //OCR1L = 62000 % 256;

    sei();
}

uint16_t timer_read() {
    cli();
    //the order of reading registers is important
    volatile uint16_t t = TCNT1L;
    t += TCNT1H * 256;
    sei(); // concurrency problem, but ignore it for now.
    return t;
}

uint16_t get_time() {
    return timer_read();
}

void coil_init() {
    //debug led output pin
    DDRD |= _BV(PD1);

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

void coil_on() {
    PORTD |= _BV(PD0);
}

void coil_off() {
    PORTD &= ~_BV(PD0);
}

#define DEFAULT_TIME (34000)

uint16_t measured_time_left;
uint16_t measured_time_right;
int direction;
#define LEFT (1) // left and right really depends on perspective.....
#define RIGHT (!LEFT)

uint16_t get_target_time() {
    if (LEFT == direction) {
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

void process_pendulum(uint16_t time) {
    direction = !direction;
    if (direction == RIGHT)
        measured_time_left = time;
    else
        measured_time_right = time;
}

void wait_until(uint16_t time) {
    while (time > get_time()) {}
}

struct Snake snake;

void init_snake() {
    snake_init(&snake);
    for( int i = 0; i < 9; i++) {
        snake_step(&snake);
        scan_strip();
    }
}

void process_debug_led() {
    if (error_level == 0) {
        debug_led_off();
    } else {
        debug_led_on();
    }
}

//TODO: implement!
// improve timing and coordination between pendulum and ledsnake
void process_coil_sensed(uint16_t coil_sensed_time) {}

int main(int argc, char **argv)
{
    ADC_Init();
    error_level_init();
    coil_init();
    timer_init();
    led_strip_init();

    init_snake();

    int led_step_scale = 1;

    int p_position = 0;

    int speed = 0;

    int coil_sensed = 0;

    timer_reset();
    while (1) {
        process_debug_led();

        uint16_t time = get_time();

        if (error_level > 0) {
            p_position = 0;
            speed = 0;
            init_snake();
        }

        if (error_level >= 4) {
            if (time > 35000) timer_reset();
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
            } else {
                coil_sensed = (0 < time) ? time : 1;
            }

            error_level = 0;

            process_pendulum(time);

            p_position = 0;

        } else {
            if (0 < coil_sensed) {
                process_coil_sensed(coil_sensed);
            }
            coil_sensed = 0;
        }

        uint16_t target_time = get_target_time();

        //~ uint16_t current_position = ((uint32_t)time) * 30 / ((uint32_t)target_time);
        //~ uint16_t current_position = time / (target_time / 30);
        //this should be quicker, but it's 2 extra steps:
        //~ uint16_t current_position = time / (target_time/32);

        // the result is a fixed point number between 0 and 65536
        uint32_t relative_time = (((uint32_t)time)<<16) / target_time;
        // this can happen, when the pendulum period changes.
        if( (1<<16) >= relative_time ) {
            relative_time = (1<<16) - 1;
        }
        uint16_t current_position = get_position(relative_time);



        int step = current_position - p_position;

        if( 1 < current_position && current_position < 10 ) {
            coil_on();
        } else {
            coil_off();
        }

        for( int i = 0; i < step; i++ ) {
            snake_step(&snake);
        }
        if( 0 < step ) {
            cli();
            scan_strip();
            sei();
        }
    }
    return 0;
}

