/* grid_engine.h -- stroke recognizer based on a 3X3 grid

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

#ifndef GRID_ENGINE_H
#define GRID_ENGINE_H

#include "rec_engine.h"
#include "stroke.h"

/* How many pixels can we drift, (both directions), and still be a tap? */
#define GRID_DEFAULT_TAP_DRIFT 6

struct grid_priv
{
    int tap_drift;
};
typedef struct grid_priv grid_priv_t;

int grid_priv_alloc(rec_engine_t *engine);
void grid_priv_free(rec_engine_t *engine);

void *grid_feature_data_alloc(rec_engine_t *engine,
			      char *feature_data_str);
void grid_feature_data_free(rec_engine_t *engine, void *feature_data);

void grid_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
char *grid_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke);
void grid_free_classification(rec_engine_t *engine, stroke_t *stroke);

double grid_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data);

int grid_set_option(rec_engine_t *engine, char *name, char *value);

#endif
