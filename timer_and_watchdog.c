/*
 * timer_and_watchdog.c
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

#include <avr/interrupt.h>
#include <avr/io.h>

volatile int error_level;

void error_level_init()
{
    error_level = 5;
}

int get_error_level()
{
    return error_level;
}

void set_error_level(int level)
{
    error_level = level;
}

inline void watchdog_bark()
{
    if (error_level < 10) {
        error_level += 3;
    }
}

ISR(TIMER1_OVF_vect)
{
    watchdog_bark();
}

void timer_reset()
{
    cli();
    //the order of writing registers is important
    //_T_imer _C_ou_NT_er for timer_1_ (high and low)
    TCNT1H = 0;
    TCNT1L = 0;
    sei();
}

//TODO: clean up comments in these functions
void timer_init()
{
    //disable all interrupt while setting up timer
    cli();
    //timer interrupt mask register
    //TIMSK1

    //timer control register A
    //TCCR1A

    //TCCR1B: timer control register B

    //TODO: make these registers a function of the PRESCALER macro
    //set prescaler to 1:1024
    // with 8MHz -> overflow after 8.388608 seconds
    //  and resolution: 128us
    TCCR1B |= _BV(CS12);
    TCCR1B &= ~_BV(CS11);
    TCCR1B |= _BV(CS10);

    //TIMSK: timer interrupt mask
    //(general timer interrupt register)

    //the order of writing registers is important
    //set timer compare register to 2 seconds
    //OCR1H = 62000 / 256;
    //OCR1L = 62000 % 256;

    sei();
}

void watchdog_on()
{
    // _T_imer _O_verflow _I_nterrupt _E_nable at timer_1_
    TIMSK |= _BV(TOIE1);
}

void watchdog_off()
{
    // _T_imer _O_verflow _I_nterrupt _E_nable at timer_1_
    TIMSK &= ~_BV(TOIE1);
}

void timer_and_watchdog_init()
{
    error_level_init();
    timer_init();
}

uint16_t timer_read()
{
    cli();
    //the order of reading registers is important
    volatile uint16_t t = TCNT1L;
    t += TCNT1H * 256;
    sei(); // concurrency problem, but ignore it for now.
    return t;
}

uint16_t get_time()
{
    return timer_read();
}

uint16_t get_time_seconds()
{
    uint32_t time = timer_read();
    return time * PRESCALER / F_CPU;
}

//TODO: test this more precise calculation
uint16_t get_time_milliseconds()
{
    uint32_t time = timer_read();
    //assume F_CPU has some decimal zeros
    uint32_t divisor = F_CPU / 1000;
    //maximum PRESCALER is 1024 (10bit)
    //that means:
    //we have at least 6 unused bits we can use for precision
    time = (time<<6) * PRESCALER / divisor;
    return (time>>6);
}
