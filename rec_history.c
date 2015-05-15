/* rec_history.c -- average position, size, and orientation of recent strokes

   Copyright 2001 Carl Worth

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI_16 (M_PI_4 / 4.0)

#include "rec_history.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int rec_history_init(rec_history_t *history, int x, int y,
		     int width, int height,
		     double orientation)
{
    int i;

    history->x = x;
    history->y = y;
    for (i=0; i < RH_POS_SAMPLES; i++) {
	history->x_history[i] = x;
	history->y_history[i] = y;
    }

    history->width = width;
    history->height = height;
    for (i=0; i < RH_SIZE_SAMPLES; i++) {
	history->width_history[i] = width;
	history->height_history[i] = height;
    }


    history->orientation = orientation;
    for (i=0; i < RH_ORIENTATION_SAMPLES; i++) {
	history->orientation_history[i] = orientation;
    }

    return 0;
}

void rec_history_deinit(rec_history_t *history)
{
    /* nothing to see here, move along... */
}

void rec_history_update_position(rec_history_t *history, int x, int y)
{
    int i;

    for (i=0; i < RH_POS_SAMPLES - 1; i++) {
	history->x_history[i] = history->x_history[i+1];
	history->y_history[i] = history->y_history[i+1];
    }
    history->x_history[i] = x;
    history->y_history[i] = y;

    history->x = 0;
    history->y = 0;
    for (i=0; i < RH_POS_SAMPLES; i++) {
	history->x += history->x_history[i];
	history->y += history->y_history[i];
    }
    history->x /= RH_POS_SAMPLES;
    history->y /= RH_POS_SAMPLES;
}

void rec_history_update_size(rec_history_t *history, int width, int height)
{
    int i;

    for (i=0; i < RH_SIZE_SAMPLES - 1; i++) {
	history->width_history[i] = history->width_history[i+1];
	history->height_history[i] = history->height_history[i+1];
    }
    history->width_history[i] = width;
    history->height_history[i] = height;

    history->width = 0;
    history->height = 0;
    for (i=0; i < RH_SIZE_SAMPLES; i++) {
	history->width += history->width_history[i];
	history->height += history->height_history[i];
    }
    history->width /= RH_SIZE_SAMPLES;
    history->height /= RH_SIZE_SAMPLES;
}

void rec_history_nudge_orientation(rec_history_t *history, double correction)
{
    int i;

    for (i=0; i < RH_ORIENTATION_SAMPLES - 1; i++) {
	history->orientation_history[i] = history->orientation_history[i+1];
    }
    history->orientation_history[i] = history->orientation + correction;

    history->orientation = 0.0;
    for (i=0; i < RH_ORIENTATION_SAMPLES; i++) {
	history->orientation += history->orientation_history[i];
    }
    history->orientation /= (double) RH_ORIENTATION_SAMPLES;
}
