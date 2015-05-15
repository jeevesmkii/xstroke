/* anchor_engine.c -- identify strokes along full-screen, non-uniform 3X3 grid

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

#include "rec.h"
#include "grid.h"
#include "regex_feature.h"
#include "anchor_engine.h"
#include "sprintf_alloc.h"
#include "fixed.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* anchor_engine_funcs are defined statically in rec_engine.c
rec_engine_funcs_t engine_funcs =
{
    anchor_priv_alloc,
    anchor_priv_free,
    anchor_feature_data_alloc,
    anchor_feature_data_free,
    anchor_classify_stroke,
    anchor_classification_str_alloc,
    anchor_free_classification,
    anchor_recognize_feature
};
*/

int anchor_priv_alloc(rec_engine_t *engine)
{
    anchor_priv_t *priv;

    priv = malloc(sizeof(anchor_priv_t));
    if (priv == NULL) {
	fprintf(stderr, "%s: Out of memory\n", __FUNCTION__);
	return ENOMEM;
    }

    priv->border_width_ratio = ANCHOR_DEFAULT_BORDER_WIDTH_PERCENT / 100.0;
    engine->priv = priv;
    return 0;
}

void anchor_priv_free(rec_engine_t *engine)
{
    free(engine->priv);
    engine->priv = NULL;
}

void *anchor_feature_data_alloc(rec_engine_t *engine, char *feature_data_str)
{
    return regex_feature_alloc(feature_data_str);
}

void anchor_feature_data_free(rec_engine_t *engine, void *feature_data)
{
    regex_feature_free((regex_t *) feature_data);
}

void anchor_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    anchor_priv_t *priv = engine->priv;
    double bwr = priv->border_width_ratio;
    int x_thresh_low, x_thresh_high;
    int y_thresh_low, y_thresh_high;

    x_thresh_low = f_to_fixed(engine->rec->width * bwr, stroke->precision_bits);
    x_thresh_high = f_to_fixed(engine->rec->width * (1.0 - bwr), stroke->precision_bits);
    y_thresh_low = f_to_fixed(engine->rec->height * bwr, stroke->precision_bits);
    y_thresh_high = f_to_fixed(engine->rec->height * (1.0 - bwr), stroke->precision_bits);

    stroke->classifications[engine->num] = 
	grid_stroke_sequence_alloc(stroke,
				   x_thresh_low, x_thresh_high,
				   y_thresh_low, y_thresh_high);
}

char *anchor_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke)
{
    return strdup((char *) stroke->classifications[engine->num]);
}

void anchor_free_classification(rec_engine_t *engine, stroke_t *stroke)
{
    free(stroke->classifications[engine->num]);
}

double anchor_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
			       void *feature_data)
{
    char *sequence = (char *) stroke->classifications[engine->num];
    regex_t *regex = (regex_t *) feature_data;

    return regex_feature_recognize(regex, sequence);
}

int anchor_set_option(rec_engine_t *engine, char *option, char *value)
{
    anchor_priv_t *priv;

    priv = (anchor_priv_t *) engine->priv;
    if (strcmp(option, "BorderWidthPercent") == 0) {
	priv->border_width_ratio = atof(value) / 100.0;
	return 0;
    }

    return EINVAL;
}
