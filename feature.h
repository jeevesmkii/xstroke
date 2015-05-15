/* feature.h -- recognizer engine-specific feature data

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

#ifndef FEATURE_H
#define FEATURE_H

#include "rec.h"
#include "rec_engine.h"

struct rec;
struct feature
{
    rec_engine_t *engine;
    void *data;
};
typedef struct feature feature_t;

struct feature_list
{
    int num_features;
    feature_t *features;
};
typedef struct feature_list feature_list_t;

int feature_init(feature_t *feature, struct rec *rec, char *engine_name, char *data_str);
void feature_deinit(feature_t *feature);

double feature_recognize(feature_t *feature, stroke_t *stroke);

int feature_list_init(feature_list_t *list);
void feature_list_deinit(feature_list_t *list);
int feature_list_append(feature_list_t *list, feature_t *feature);

#endif
