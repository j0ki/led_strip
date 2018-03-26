/*
 * coil.c
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

#include "adc.h"

void coil_init()
{
    //we are the only module that uses adc
    ADC_Init();

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

int coil_sensor_adc()
{
    return (ADC_Read() < 200);
}

int coil_sensor_pin()
{
    return !(PIND & _BV(PD2));
}
