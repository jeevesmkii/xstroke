/* stroke.c -- store information on a stroke

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
#include <limits.h>

#include "fixed.h"
#include "stroke.h"
#include "matrix.h"
#include "bresenham.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static void add_interpolated_pt_cb(stroke_t *stroke, int x, int y);
static pt_t *do_add_pt(stroke_t *stroke, int x, int y, unsigned long time);
static void stroke_find_bounds(stroke_t *stroke);
static void stroke_find_aspect(stroke_t *stroke);
static void grow_pts(stroke_t *stroke);

int stroke_init(stroke_t *stroke)
{
    stroke->pts = NULL;
    stroke->num_pts = 0;
    stroke->max_pts = 0;

    stroke->min_x = INT_MAX;
    stroke->min_y = INT_MAX;
    stroke->width = 0;
    stroke->height = 0;

    stroke->precision_bits = 0;

    stroke->aspect = NORMAL_ASPECT;
    stroke->classifications = NULL;

    return 0;
}

void stroke_deinit(stroke_t *stroke)
{
    if (stroke->pts) {
	free(stroke->pts);
    }
    /* Reset all fields to defaults */
    stroke_init(stroke);
}

int pt_init(pt_t *pt, int x, int y, int time)
{
    pt->x = x;
    pt->y = y;
    pt->time = time;
    pt->interpolated = 0;

    return 0;
}

static void add_interpolated_pt_cb(stroke_t *stroke, int x, int y)
{
    pt_t *pt = do_add_pt(stroke,
			 f_to_fixed(x, stroke->precision_bits),
			 f_to_fixed(y, stroke->precision_bits),
			 0);
    pt->interpolated = 1;
}

static pt_t *do_add_pt(stroke_t *stroke, int x, int y, unsigned long time)
{
    pt_t *pt;

    if (stroke->num_pts == 0) {
	stroke->min_x = x;
	stroke->min_y = y;
	stroke->width = 1;
	stroke->height = 1;
    }

    while (stroke->num_pts >= stroke->max_pts) {
	grow_pts(stroke);
    }

    if (x < stroke->min_x) {
	stroke->width += (stroke->min_x - x);
	stroke->min_x = x;
	stroke_find_aspect(stroke);
    }
    if (y < stroke->min_y) {
	stroke->height += (stroke->min_y - y);
	stroke->min_y = y;
	stroke_find_aspect(stroke);
    }

    if ((x - stroke->min_x + 1) > stroke->width) {
	stroke->width = x - stroke->min_x + 1;
	stroke_find_aspect(stroke);
    }
    if ((y - stroke->min_y + 1) > stroke->height) {
	stroke->height = y - stroke->min_y + 1;
	stroke_find_aspect(stroke);
    }

    pt = &stroke->pts[stroke->num_pts++];
    pt_init(pt, x, y, time);

    return pt;
}

void stroke_add_pt(stroke_t *stroke, int x, int y, unsigned long time)
{
    int p_bits = stroke->precision_bits;

    if (stroke->num_pts) {
	pt_t *prev = &stroke->pts[stroke->num_pts - 1];
	int dx = abs(fixed_to_i_round(x - prev->x, p_bits));
	int dy = abs(fixed_to_i_round(y - prev->y, p_bits));
	if (dx > 1 || dy > 1) {
	    bresenham_skip_first_last((bresenham_cb_t) add_interpolated_pt_cb,
				      stroke,
				      fixed_to_i_round(prev->x, p_bits),
				      fixed_to_i_round(prev->y, p_bits),
				      fixed_to_i_round(x, p_bits),
				      fixed_to_i_round(y, p_bits));
	}
    }

    do_add_pt(stroke, x, y, time);
}

/* Note: this function does not do any renormalization after doing a
   fixed-point multiplication, (so we don't lose any precision). This
   means that there will be bit growth of m->precision_bits in all of
   the stroke pt values. Consider yourself warned.  And call
   stroke_normalize after stroke_transform if you want to throw all
   those precious little bits away. */
void stroke_transform(stroke_t *stroke, matrix_t *m)
{
    int i;
    int x, y;

    pt_t *pts = stroke->pts;
    int num_pts = stroke->num_pts;

    stroke->pts = NULL;
    stroke->num_pts = 0;
    stroke->max_pts = 0;

    stroke->precision_bits += m->precision_bits;

    for (i=0; i < num_pts; i++) {
	if (pts[i].interpolated == 0) {
	    x = m->m[0][0] * pts[i].x + m->m[0][1] * pts[i].y + m->m[0][2];
	    y = m->m[1][0] * pts[i].x + m->m[1][1] * pts[i].y + m->m[1][2];
	    stroke_add_pt(stroke, x, y, pts[i].time);
	}
    }
    free(pts);
    stroke_find_bounds(stroke);
}

/* Normalize values to eliminate bit growth from stroke_transform */
void stroke_normalize(stroke_t *stroke)
{
    int i;
    int p_bits = stroke->precision_bits;

    for (i=0; i < stroke->num_pts; i++) {
	stroke->pts[i].x = fixed_to_i_round(stroke->pts[i].x, p_bits);
	stroke->pts[i].y = fixed_to_i_round(stroke->pts[i].y, p_bits);
    }
    stroke->precision_bits = 0;
    stroke_find_bounds(stroke);
}

char *stroke_str_alloc(stroke_t *stroke)
{
    int i;
    char *str = NULL;
    unsigned long min_time = stroke->pts[0].time;
    
    for (i=0; i < stroke->num_pts; i++) {
	pt_t *pt = &stroke->pts[i];
	if (pt->interpolated == 0) {
	    sprintf_append_alloc(&str, "%s%d,%d,%lu",
				 i == 0 ? "" : ",",
				 fixed_to_i_round(pt->x, stroke->precision_bits),
				 fixed_to_i_round(pt->y, stroke->precision_bits),
				 pt->time - min_time);
	}
    }

    return str;
}

static void stroke_find_aspect(stroke_t *stroke)
{
    if (stroke->height / (double) stroke->width > ASPECT_THRESHOLD) {
	stroke->aspect = TALL_ASPECT;
    } else if (stroke->width / (double) stroke->height > ASPECT_THRESHOLD) {
	stroke->aspect = WIDE_ASPECT;
    } else {
	stroke->aspect = NORMAL_ASPECT;
    }
}

static void stroke_find_bounds(stroke_t *stroke)
{
    int i;
    int x, y;
    int max_x, max_y;

    stroke->min_x = INT_MAX;
    stroke->min_y = INT_MAX;
    max_x = INT_MIN;
    max_y = INT_MIN;

    for (i=0; i < stroke->num_pts; i++) {
	x = stroke->pts[i].x;
	y = stroke->pts[i].y;
	
	if (x < stroke->min_x)
	    stroke->min_x = x;
	if (y < stroke->min_y)
	    stroke->min_y = y;
	if (x > max_x)
	    max_x = x;
	if (y > max_y)
	    max_y = y;
    }
    stroke->width = max_x - stroke->min_x + f_to_fixed(1, stroke->precision_bits);
    stroke->height = max_y - stroke->min_y + f_to_fixed(1, stroke->precision_bits);

    stroke_find_aspect(stroke);
}

static void grow_pts(stroke_t *stroke)
{
    stroke->max_pts += STROKE_GROWTH_INC;
    stroke->pts = realloc(stroke->pts, sizeof(pt_t) * stroke->max_pts);
    if (stroke->pts == NULL) {
	fprintf(stderr, "%s out of memory\n", __FUNCTION__);
	stroke->num_pts = 0;
	stroke->max_pts = 0;		
    }
}

