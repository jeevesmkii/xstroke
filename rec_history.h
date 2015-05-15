/* rec_history.h -- average position, size, and orientation of recent strokes

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

#ifndef REC_HISTORY_H
#define REC_HISTORY_H

/* need pt_t */
#include "stroke.h"

#define RH_POS_SAMPLES 3
#define RH_SIZE_SAMPLES RH_POS_SAMPLES
#define RH_ORIENTATION_SAMPLES 3
struct rec_history
{
    int x;
    int y;
    int width;
    int height;
    double orientation;

    int x_history[RH_POS_SAMPLES];
    int y_history[RH_POS_SAMPLES];
    int width_history[RH_SIZE_SAMPLES];
    int height_history[RH_SIZE_SAMPLES];
    double orientation_history[RH_ORIENTATION_SAMPLES];
};
typedef struct rec_history rec_history_t;

int rec_history_init(rec_history_t *history, int x, int y,
		     int width, int height,
		     double orientation);
void rec_history_deinit(rec_history_t *history);

void rec_history_rotate_for_orientation(rec_history_t *history, int *x, int *y);

void rec_history_update_position(rec_history_t *history, int x, int y);
void rec_history_update_size(rec_history_t *history, int width, int height);
void rec_history_nudge_orientation(rec_history_t *history, double correction);

#endif

