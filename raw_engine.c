/* raw_engine.c -- not a real recognizer, just a way to store raw strokes

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
#include <errno.h>
#include <string.h>

#include "raw_engine.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* raw_engine_funcs are defined statically in rec_engine.c
rec_engine_funcs_t engine_funcs =
{
    raw_priv_alloc,
    raw_priv_free,
    raw_feature_data_alloc,
    raw_feature_data_free,
    raw_classify_stroke,
    raw_classification_str_alloc,
    raw_free_classification,
    raw_recognize_feature
};
*/

int raw_priv_alloc(rec_engine_t *engine)
{
    return 0;
}

void raw_priv_free(rec_engine_t *engine)
{
    /* that's all she wrote */
}

void *raw_feature_data_alloc(rec_engine_t *engine, char *feature_data_str)
{
    stroke_t *stroke;
    char *nptr, *endptr;
    long x, y, time;

    stroke = malloc(sizeof(stroke_t));
    if (stroke == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return NULL;
    }

    stroke_init(stroke);
    nptr = feature_data_str;
    while (1) {
	x = strtol(nptr, &endptr, 0);
	if (endptr == nptr && endptr[0] != '\0') {
	    fprintf(stderr, "%s: Error parsing X value in raw stroke data at: %s\n",
		    __FUNCTION__, nptr);
	    break;
	}
	nptr = endptr + 1;
	y = strtol(nptr, &endptr, 0);
	if (endptr == nptr && endptr[0] != '\0') {
	    fprintf(stderr, "%s: Error parsing Y value in raw stroke data at: %s\n",
		    __FUNCTION__, nptr);
	    break;
	}
	nptr = endptr + 1;
	time = strtol(nptr, &endptr, 0);
	if (endptr == nptr && endptr[0] != '\0') {
	    fprintf(stderr, "%s: Error parsing time value in raw stroke data at: %s\n",
		    __FUNCTION__, nptr);
	    break;
	}
	nptr = endptr + 1;
	stroke_add_pt(stroke, x, y, time);
	if (endptr[0] == '\0') {
	    break;
	}
    }

    return stroke;
}

void raw_feature_data_free(rec_engine_t *engine, void *feature_data)
{
    stroke_deinit((stroke_t *) feature_data);
}

void raw_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    stroke->classifications[engine->num] = NULL;
}

char *raw_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke)
{
    return stroke_str_alloc(stroke);
}

void raw_free_classification(rec_engine_t *engine, stroke_t *stroke)
{
    /* Nothing to do here */
}

double raw_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data)
{
    /* raw_engine recognizes anything, (else the veryify will never work) */
    return 1;
}

int raw_set_option(rec_engine_t *engine, char *name, char *value)
{
    return EINVAL;
}
