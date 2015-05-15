/* anchor_engine.h -- identify strokes along full-screen, non-uniform 3X3 grid

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

#ifndef ANCHOR_ENGINE_H
#define ANCHOR_ENGINE_H

#include "rec_engine.h"
#include "stroke.h"

/* How wide are the grid edges, (in percent of width/height) */
#define ANCHOR_DEFAULT_BORDER_WIDTH_PERCENT 5.0

struct anchor_priv
{
    double border_width_ratio;
};
typedef struct anchor_priv anchor_priv_t;

int anchor_priv_alloc(rec_engine_t *engine);
void anchor_priv_free(rec_engine_t *engine);

void *anchor_feature_data_alloc(rec_engine_t *engine, char *feature_data_str);
void anchor_feature_data_free(rec_engine_t *engine, void *feature_data);

void anchor_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
char *anchor_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke);
void anchor_free_classification(rec_engine_t *engine, stroke_t *stroke);

double anchor_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			       void *feature_data);

int anchor_set_option(rec_engine_t *engine, char *option, char *value);

#endif
