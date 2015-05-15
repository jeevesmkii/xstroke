/* stroke.h -- store information on a stroke

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

#ifndef STROKE_H
#define STROKE_H

#include "matrix.h"

#define STROKE_GROWTH_INC 100

struct pt
{
    int x;
    int y;
    unsigned long time;
    int interpolated;
};
typedef struct pt pt_t;

#define ASPECT_THRESHOLD 4

enum aspect {NORMAL_ASPECT, WIDE_ASPECT, TALL_ASPECT};
typedef enum aspect aspect_t;

struct stroke
{
    int max_pts;
    int num_pts;
    pt_t *pts;

    int min_x;
    int min_y;
    int width;
    int height;

    int precision_bits;

    aspect_t aspect;

    void **classifications;
};
typedef struct stroke stroke_t;

int stroke_init(stroke_t *stroke);
void stroke_deinit(stroke_t *stroke);

void stroke_add_pt(stroke_t *stroke, int x, int y, unsigned long time);

void stroke_transform(stroke_t *stroke, matrix_t *m);
void stroke_normalize(stroke_t *stroke);
char *stroke_str_alloc(stroke_t *stroke);

#endif
