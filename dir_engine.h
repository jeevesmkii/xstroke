/* dir_engine.h -- recognizer that characterizes strokes by direction

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

#ifndef DIR_ENGINE_H
#define DIR_ENGINE_H

#include "rec_engine.h"
#include "stroke.h"

int dir_priv_alloc(rec_engine_t *engine);
void dir_priv_free(rec_engine_t *engine);

void *dir_feature_data_alloc(rec_engine_t *engine,
			      char *feature_data_str);
void dir_feature_data_free(rec_engine_t *engine, void *feature_data);

void dir_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
void dir_length_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
char *dir_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke);
void dir_free_classification(rec_engine_t *engine, stroke_t *stroke);

double dir_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data);

int dir_set_option(rec_engine_t *engine, char *name, char *value);

#endif
