/* feature.c -- recognizer engine-specific feature data

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
#include <errno.h>

#include "rec.h"
#include "rec_engine.h"
#include "feature.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int feature_init(feature_t *feature, rec_t *rec, char *engine_name, char *data_str)
{
    feature->engine = rec_get_engine(rec, engine_name);
    if (feature->engine) {
	feature->data = rec_engine_feature_data_alloc(feature->engine, data_str);
    }

    return 0;
}

void feature_deinit(feature_t *feature)
{
    rec_engine_feature_data_free(feature->engine, feature->data);
    feature->data = NULL;
    feature->engine = NULL;
}

double feature_recognize(feature_t *feature, stroke_t *stroke)
{
    return rec_engine_recognize_stroke(feature->engine, stroke, feature->data);
}

int feature_list_init(feature_list_t *list)
{
    list->num_features = 0;
    list->features = NULL;

    return 0;
}

void feature_list_deinit(feature_list_t *list)
{
    int i;

    for (i=0; i < list->num_features; i++) {
	feature_deinit(&list->features[i]);
    }

    if (list->features) {
	free(list->features);
	list->features = NULL;
    }
    list->num_features = 0;
}

int feature_list_append(feature_list_t *list, feature_t *feature)
{
    feature_t *new_features;

    if (feature->engine == NULL) {
	return 0;
    }

    list->num_features++;
    new_features = realloc(list->features, list->num_features * sizeof(feature_t));

    if (new_features == NULL) {
	fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	list->num_features--;
	return ENOMEM;
    }

    list->features = new_features;
    list->features[list->num_features - 1] = *feature;

    return 0;
}
