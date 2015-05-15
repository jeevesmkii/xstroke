/* raw_engine.h -- not a real recognizer, just a way to store raw strokes

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

#ifndef RAW_ENGINE_H
#define RAW_ENGINE_H

#include "rec_engine.h"
#include "stroke.h"

/* How many pixels can we drift, (both directions), and still be a tap? */
#define RAW_DEFAULT_TAP_DRIFT 6

struct raw_priv
{
    stroke_t stroke;
};
typedef struct raw_priv raw_priv_t;

int raw_priv_alloc(rec_engine_t *engine);
void raw_priv_free(rec_engine_t *engine);

void *raw_feature_data_alloc(rec_engine_t *engine,
			      char *feature_data_str);
void raw_feature_data_free(rec_engine_t *engine, void *feature_data);

void raw_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
char *raw_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke);
void raw_free_classification(rec_engine_t *engine, stroke_t *stroke);

double raw_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data);

int raw_set_option(rec_engine_t *engine, char *name, char *value);

#endif
