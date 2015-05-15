/* grid.c -- helper functions for analyzing strokes with a 3X3 grid

   Copyright (C) 2000 Carl Worth

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
#include <string.h>

#include "xstroke.h"
#include "grid.h"

static int find_cell(stroke_t *stroke, int x, int y,
		     int x_thresh_low, int x_thresh_high,
		     int y_thresh_low, int y_thresh_high);

char *grid_stroke_sequence_alloc(stroke_t *stroke,
				 int x_thresh_low, int x_thresh_high,
				 int y_thresh_low, int y_thresh_high)
{
    int i;
    pt_t *pt;
    int seq_size;
    int seq_len;
    char *seq;
    int cell;

    seq_size = 0;
    seq_len = 0;
    seq = NULL;

    for (i=0; i < stroke->num_pts; i++) {
	seq_len++;
	if ((seq_len + 1) > seq_size) {
	    seq_size += 10;
	    seq = realloc(seq, seq_size);
	    if (seq == NULL) {
		fprintf(stderr, "%s: Out of memory.\n", __FUNCTION__);
		free(seq);
		return strdup("");
	    }
	}

	pt = &stroke->pts[i];
	cell = find_cell(stroke, pt->x, pt->y,
			 x_thresh_low, x_thresh_high,
			 y_thresh_low, y_thresh_high);
	seq[seq_len - 1] = '0' + cell;

	/* Discard duplicates */
	while (i < (stroke->num_pts - 1)) {
	    pt = &stroke->pts[i+1];
	    if (find_cell(stroke, pt->x, pt->y,
			  x_thresh_low, x_thresh_high,
			  y_thresh_low, y_thresh_high) == cell) {
		i++;
	    } else {
		break;
	    }
	}
    }

    seq[seq_len] = '\0';

    return seq;
}

static int find_cell(stroke_t *stroke, int x, int y,
		     int x_thresh_low, int x_thresh_high,
		     int y_thresh_low, int y_thresh_high)
{
    int cell_x, cell_y;

    if (stroke->aspect == TALL_ASPECT) {
	cell_x = 1;
    } else {
	if (x < x_thresh_low)
	    cell_x = 0;
	else if (x < x_thresh_high)
	    cell_x = 1;
	else
	    cell_x = 2;
    }

    if (stroke->aspect == WIDE_ASPECT) {
	cell_y = 1;
    } else {
	if (y < y_thresh_low)
	    cell_y = 0;
	else if (y < y_thresh_high)
	    cell_y = 1;
	else
	    cell_y = 2;
    }

#ifdef XSTROKE_DEBUG
    if (cell_x < 0 || cell_x > 2
	|| cell_y < 0 || cell_y > 2) {
	fprintf(stderr, "%s: Computed cell (%d, %d) out of range!\n",
		__FUNCTION__, cell_x, cell_y);
    }
#endif

    return cell_y * 3 + cell_x + 1;
}
