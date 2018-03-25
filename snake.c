/*
 * snake.c
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

#include "snake.h"
#include "led_strip.h"

//TODO: in real hardware test which value is good
#define INITIAL_POSITION (15)

struct Led background_color;
struct Led head_color;
struct Led tail_color[2];

void snake_step(struct Snake* s)
{
    s->head = s->head + 1; // don't care about int overflow at this time of writing
}

void snake_set_position(struct Snake* s, unsigned int position)
{
    s->head = INITIAL_POSITION + position;
}

void snake_draw(struct Snake* s)
{
    clear_strip(7, 7, 15);

    int tail = s->head - s->length;
    for (int i = 0; i < s->length; i++) {
        led_strip_set(mirror(tail, STRIP_LENGTH), &tail_color[i % 2]);
        tail += 1;
    }
    led_strip_set(mirror(s->head, STRIP_LENGTH), &head_color);
}

void snake_init(struct Snake* s)
{
    s->length = 6;

    s->head = INITIAL_POSITION;

    head_color.g = 15;
    head_color.r = 100;
    head_color.b = 15;

    tail_color[0].g = 255;
    tail_color[0].r = 70;
    tail_color[0].b = 40;

    tail_color[1].g = 120;
    tail_color[1].r = 70;
    tail_color[1].b = 40;

    background_color.g = 7;
    background_color.r = 7;
    background_color.b = 15;

    //background_color = {.g=7, .r=7, .b=15};
}
