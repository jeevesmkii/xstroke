/* grid_engine.c -- stroke recognizer based on a 3X3 grid

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

#include "grid.h"
#include "regex_feature.h"
#include "grid_engine.h"
#include "sprintf_alloc.h"
#include "fixed.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* grid_engine_funcs are defined statically in rec_engine.c
rec_engine_funcs_t engine_funcs =
{
    grid_priv_alloc,
    grid_priv_free,
    grid_feature_data_alloc,
    grid_feature_data_free,
    grid_classify_stroke,
    grid_classification_str_alloc,
    grid_free_classification,
    grid_recognize_feature
};
*/

int grid_priv_alloc(rec_engine_t *engine)
{
    grid_priv_t *priv;

    priv = malloc(sizeof(grid_priv_t));
    if (priv == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return ENOMEM;
    }

    priv->tap_drift = GRID_DEFAULT_TAP_DRIFT;
    engine->priv = priv;
    return 0;
}

void grid_priv_free(rec_engine_t *engine)
{
    free(engine->priv);
    engine->priv = NULL;
}

void *grid_feature_data_alloc(rec_engine_t *engine, char *feature_data_str)
{
    return regex_feature_alloc(feature_data_str);
}

void grid_feature_data_free(rec_engine_t *engine, void *feature_data)
{
    regex_feature_free((regex_t *) feature_data);
}

void grid_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    int x_thresh_low, x_thresh_high;
    int y_thresh_low, y_thresh_high;

    grid_priv_t *priv = engine->priv;

    /* Very short taps should simply be 5 */
    if (stroke->aspect == NORMAL_ASPECT
	&& (fixed_to_i_round(stroke->width + stroke->height,
			     stroke->precision_bits)
	    <= priv->tap_drift)) {
	stroke->classifications[engine->num] = strdup("5");
	return;
    }

    x_thresh_low = stroke->min_x + stroke->width / 3;
    x_thresh_high = stroke->min_x + (2 * stroke->width / 3);
    y_thresh_low = stroke->min_y + stroke->height / 3;
    y_thresh_high = stroke->min_y + (2 * stroke->height / 3);

    stroke->classifications[engine->num] =
	grid_stroke_sequence_alloc(stroke,
				   x_thresh_low, x_thresh_high,
				   y_thresh_low, y_thresh_high);
}

char *grid_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke)
{
    return strdup((char *) stroke->classifications[engine->num]);
}

void grid_free_classification(rec_engine_t *engine, stroke_t *stroke)
{
    free(stroke->classifications[engine->num]);
}

double grid_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			     void *feature_data)
{
    char *sequence = (char *) stroke->classifications[engine->num];
    regex_t *regex = (regex_t *) feature_data;

    return regex_feature_recognize(regex, sequence);
}

int grid_set_option(rec_engine_t *engine, char *name, char *value)
{
    grid_priv_t *priv;

    priv = (grid_priv_t *) engine->priv;
    if (strcmp(name, "TapDrift") == 0) {
	priv->tap_drift = atoi(value);
	return 0;
    }

    return EINVAL;
}
