/* grid.h -- helper functions for analyzing strokes with a 3X3 grid

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

#ifndef GRID_H
#define GRID_H

#include "stroke.h"

char *grid_stroke_sequence_alloc(stroke_t *stroke,
				 int x_thresh_low, int x_thresh_high,
				 int y_thresh_low, int y_thresh_high);

#endif
