/* rec.c - top-level interface to a recognizer

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
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#include "args.h"
#include "rec.h"
#include "rec_mode.h"
#include "option.h"
#include "rec_parse.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static action_t *recognize_stroke(rec_t *rec, stroke_t *stroke);
static void log_mode_change(rec_t *rec, rec_mode_t *mode);
static void perform_action(rec_t *rec, action_t *action);
static void perform_action_item(rec_t *rec, action_item_t *item);
static void reset_mode(rec_t *rec);

static void rec_mode_action_cb(void *action_data, void *data);
static void rec_key_action_cb(void *action_data, void *data);
static void rec_orient_action_cb(void *action_data, void *data);

static char *null_engines[] = {"doc"};
static int num_null_engines = sizeof(null_engines) / sizeof(char *);

int rec_init(rec_t *rec, args_t *args, int width, int height)
{
    int i, err;
    char *log_file;

    rec->width = width;
    rec->height = height;

    /* We need the logs in place before we do anything else */
    sprintf_alloc(&log_file, "%s/%s", args->logdir, REC_LOG_FILE_NAME);
    err = log_init(&rec->log, log_file, args->logmax);
    free(log_file);
    if (err) {
	fprintf(stderr, "%s: An error occurred during log_init\n", __FUNCTION__);
	return err;
    }

    sprintf_alloc(&log_file, "%s/%s", args->logdir, REC_ERR_LOG_FILE_NAME);
    err = log_init(&rec->err_log, log_file, args->logmax);
    free(log_file);
    if (err) {
	fprintf(stderr, "%s: An error occurred during log_init\n", __FUNCTION__);
	return err;
    }
    
    rec_engine_list_init(&rec->engines);
    if (args->lograw) {
	rec_get_engine(rec, "raw");
    }

    rec_mode_list_init(&rec->modes);

    rec->rotation = args->rotation;
    rec->rotation_limit = args->rotation_limit;
    rec->global_mode = rec_get_mode(rec, REC_GLOBAL_MODE_NAME);
    rec->fallback_mode = rec_get_mode(rec, REC_FALLBACK_MODE_NAME);

    rec->current_mode = rec->global_mode;
    rec->persistent_mode = rec->global_mode;

    for (i=0; i < LAST_ACTION; i++) {
	rec->callbacks[i] = NULL;
    }

    err = stroke_init(&rec->stroke);
    if (err) {
	fprintf(stderr, "%s: An error occurred during stroke_init\n", __FUNCTION__);
	return err;
    }

    rec_history_init(&rec->history, width / 2, height / 2, width, height, 0.0);

    rec_register_callback(rec, ACTION_MODE, rec_mode_action_cb, rec);
    rec_register_callback(rec, ACTION_KEY, rec_key_action_cb, rec);
    if (rec->rotation) {
	rec_register_callback(rec, ACTION_ORIENT, rec_orient_action_cb, rec);
    }

    err = rec_parse(rec, args->alphabet);
    if (err) {
	fprintf(stderr, "%s: An error occurred during rec_parse\n", __FUNCTION__);
	return err;
    }

    return 0;
}

void rec_deinit(rec_t *rec)
{
    int i;
    rec_callback_t *cb, *prev_cb;

    log_deinit(&rec->log);
    log_deinit(&rec->err_log);
    stroke_deinit(&rec->stroke);
    rec_history_deinit(&rec->history);

    for (i=0; i < LAST_ACTION; i++) {
	cb = rec->callbacks[i];
	while (cb) {
	    prev_cb = cb;
	    cb = cb->next;
	    rec_callback_deinit(prev_cb);
	    free(prev_cb);
	}
    }

    for (i=0; i < rec->modes.num_modes; i++) {
	rec_mode_deinit(rec->modes.modes[i]);
	/* malloced in rec_get_mode */
	free(rec->modes.modes[i]);
    }
    rec_mode_list_deinit_shallow(&rec->modes);

    for (i=0; i < rec->engines.num_engines; i++) {
	rec_engine_deinit(rec->engines.engines[i]);
	/* malloced in rec_get_engine */
	free(rec->engines.engines[i]);
    }
    rec_engine_list_deinit_shallow(&rec->engines);
}

void rec_set_size(rec_t *rec, int width, int height)
{
    rec->width = width;
    rec->height = height;
}

rec_engine_t *rec_get_engine(rec_t *rec, char *engine_name)
{
    int i;
    rec_engine_t *new_engine;

    for (i=0; i < rec->engines.num_engines; i++) {
	if (strcmp(engine_name, rec->engines.engines[i]->name) == 0) {
	    return rec->engines.engines[i];
	}
    }
    for (i=0; i < num_null_engines; i++) {
	if (strcmp(engine_name, null_engines[i]) == 0) {
	    return NULL;
	}
    }

    /* freed in rec_deinit */
    new_engine = malloc(sizeof(rec_engine_t));
    if (new_engine == NULL) {
	fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	return NULL;
    }
    rec_engine_init(new_engine, rec, engine_name);

    rec_engine_list_append(&rec->engines, new_engine);

    return new_engine;
}

rec_mode_t *rec_get_mode(rec_t *rec, char *mode_name)
{
    int i;
    rec_mode_t *new_mode;

    for (i=0; i < rec->modes.num_modes; i++) {
	if (strcmp(mode_name, rec->modes.modes[i]->name) == 0) {
	    return rec->modes.modes[i];
	}
    }

    /* freed in rec_deinit */
    new_mode = malloc(sizeof(rec_mode_t));
    if (new_mode == NULL) {
	fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	return NULL;
    }
    rec_mode_init(new_mode, mode_name);

    rec_mode_list_append(&rec->modes, new_mode);

    if (rec->current_mode == rec->global_mode) {
	rec->current_mode = new_mode;
	rec->persistent_mode = new_mode;
	log_mode_change(rec, new_mode);
    }

    return new_mode;
}

int rec_register_callback(rec_t *rec, action_type_t type, rec_cb_fun_t cb_fun, void *data)
{
    rec_callback_t *cb;
    /* freed in rec_deinit */
    cb = malloc(sizeof(rec_callback_t));
    if (cb == NULL) {
	fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	return ENOMEM;
    }
    rec_callback_init(cb, cb_fun, data);
    cb->next = rec->callbacks[type];
    rec->callbacks[type] = cb;

    return 0;
}

void rec_begin_stroke(rec_t *rec, int x, int y, unsigned long time)
{
    stroke_deinit(&rec->stroke);
    stroke_init(&rec->stroke);
    stroke_add_pt(&rec->stroke, x, y, time);
}

void rec_extend_stroke(rec_t *rec, int x, int y, unsigned long time)
{
    stroke_add_pt(&rec->stroke, x, y, time);
}

void rec_end_stroke(rec_t *rec, int x, int y, unsigned long time)
{
    matrix_t transform;
    double center_x, center_y;
    action_t *action;

    center_x = rec->stroke.min_x + rec->stroke.width/2.0;
    center_y = rec->stroke.min_y + rec->stroke.height/2.0;

    if (rec->rotation) {
	/* X restricts coordinates to 16 bits, so we have 16 more to
           play with for this transformation */
	matrix_init(&transform, 16);
	matrix_set_rotate_about(&transform, - rec->history.orientation, center_x, center_y);
	stroke_transform(&rec->stroke, &transform);
    }

    action = recognize_stroke(rec, &rec->stroke);

    if (action) {
	perform_action(rec, action);
    } else {
	reset_mode(rec);
    }

    if (rec->rotation) {
	/* Normalize only after recognition to preserve precision
           during recognition */
	stroke_normalize(&rec->stroke);
    }

    rec_history_update_position(&rec->history, center_x, center_y);
    rec_history_update_size(&rec->history,
			    (rec->stroke.aspect == TALL_ASPECT ?
			     rec->stroke.height : rec->stroke.width),
			    (rec->stroke.aspect == WIDE_ASPECT ?
			     rec->stroke.width : rec->stroke.height));
    stroke_deinit(&rec->stroke);
}

action_t *recognize_stroke(rec_t *rec, stroke_t *stroke)
{
    action_t *action = NULL;
    double probability;
    int i, done;
    char *msg, *a_str, *c_str;

    stroke->classifications = malloc(rec->engines.num_engines * sizeof(void *));
    if (stroke->classifications == NULL) {
	fprintf(stderr, "%s: Out of memory.\n", __FUNCTION__);
	return NULL;
    }

    for (i=0; i < rec->engines.num_engines; i++) {
	rec_engine_classify_stroke(rec->engines.engines[i], stroke);
    }

    probability = 0.0;
    done = rec_mode_recognize(rec->global_mode, stroke, &action, &probability);
    if (! done) {
	done = rec_mode_recognize(rec->current_mode, stroke,
				  &action, &probability);
    }
    if (! done) {
	rec_mode_recognize(rec->fallback_mode, stroke, &action, &probability);
    }

    /* Log this stroke */
    if (action) {
	a_str = action_str_alloc(action);
    } else {
	a_str = REC_UNKNOWN_STROKE_STR;
    }
    sprintf_alloc(&msg, "%s = {", a_str);
    if (action) {
	free(a_str);
    }
    for (i=0; i < rec->engines.num_engines; i++) {
	c_str = rec_engine_classification_str_alloc(rec->engines.engines[i],
						    stroke);
	sprintf_append_alloc(&msg, " %s(\"%s\")",
			     rec->engines.engines[i]->name, c_str);
	free(c_str);
    }
    sprintf_append_alloc(&msg, " }\n");

    log_msg(&rec->log, msg);
    if (action == NULL) {
	log_msg(&rec->err_log, msg);
    }

    free(msg);

    for (i=0; i < rec->engines.num_engines; i++) {
	rec_engine_free_classification(rec->engines.engines[i], stroke);
    }
    free(stroke->classifications);

    return action;
}

int rec_verify(rec_t *rec, char *file_name)
{
    rec_t other_rec;
    int err, errors;
    int m, g, f;
    rec_engine_t *raw_engine;
    rec_mode_t *mode,  *current_mode_orig;
    gesture_t *gesture;
    feature_t *feature;
    action_t *action;
    char *a_str, *gesture_a_str;

    other_rec = *rec;
    rec_engine_list_init(&other_rec.engines);
    rec_mode_list_init(&other_rec.modes);
    err = rec_parse(&other_rec, file_name);
    if (err) {
	fprintf(stderr, "%s: An error occurred during rec_parse\n", __FUNCTION__);
	return err;
    }
    
    current_mode_orig = rec->current_mode;
    raw_engine = rec_get_engine(&other_rec, "raw");

    errors = 0;
    for (m=0; m < other_rec.modes.num_modes; m++) {
	mode = other_rec.modes.modes[m];
	rec->current_mode = rec_get_mode(rec, mode->name);

	for (g=0; g < mode->gestures.num_gestures; g++) {
	    gesture = &mode->gestures.gestures[g];
	    for (f=0; f < gesture->features.num_features; f++) {
		feature = &gesture->features.features[f];
		if (feature->engine == raw_engine) {
		    action = recognize_stroke(rec, (stroke_t *) feature->data);
		    a_str = action_str_alloc(action);
		    gesture_a_str = action_str_alloc(&gesture->action);
		    if (strcmp(a_str, gesture_a_str)) {
			fprintf(stderr, "%s: Error: recognized stroke as `%s\' instead of `%s\'\n",
				__FUNCTION__, a_str, gesture_a_str);
			errors++;
		    }
		    free(a_str);
		    free(gesture_a_str);
		}
	    }
	}
    }

    rec->current_mode = current_mode_orig;

    return errors;
}

static void log_mode_change(rec_t *rec, rec_mode_t *mode)
{
    char *msg;
    static int first_change = 1;

    if (first_change) {
	first_change = 0;
	msg = NULL;
    } else {
	msg = strdup("}\n");
    }
    sprintf_append_alloc(&msg, "Mode \"%s\" {\n", mode->name);

    log_msg(&rec->log, msg);
    log_msg(&rec->err_log, msg);

    free(msg);
}

static void perform_action(rec_t *rec, action_t *action)
{
    int i;

    if (action) {
	for (i=0; i < action->num_items; i++) {
	    perform_action_item(rec, &action->items[i]);
	}
    }
}

static void perform_action_item(rec_t *rec, action_item_t *item)
{
    rec_callback_t *cb;

    if (item->type >= LAST_ACTION) {
	fprintf(stderr, "%s: ERROR: unknown action type %d\n", __FUNCTION__, item->type);
	return;
    }

    for (cb = rec->callbacks[item->type]; cb; cb = cb->next) {
	if (cb->cb_fun)
	    (cb->cb_fun)(item->data, cb->data);
    }
}

static void rec_mode_action_cb(void *action_data, void *data)
{
    action_mode_data_t *mode_data = (action_mode_data_t *) action_data;
    rec_t *rec = (rec_t *) data;

    rec->current_mode = mode_data->mode;
    if (mode_data->permanent) {
	rec->persistent_mode = mode_data->mode;
    }
    log_mode_change(rec, rec->current_mode);
}

static void rec_key_action_cb(void *action_data, void *data)
{
    rec_t *rec = (rec_t *) data;
    action_key_data_t *key_data = (action_key_data_t *) action_data;

    if ((key_data->flags & ACTION_KEY_IS_MODIFIER) == 0) {
	reset_mode(rec);
    }
}

static void rec_orient_action_cb(void *action_data, void *data)
{
    rec_t *rec = (rec_t *) data;
    action_orient_data_t *orient_data = (action_orient_data_t *) action_data;
    stroke_t *stroke;
    pt_t *first_pt, *last_pt;
    double stroke_orientation, correction;
    
    stroke = &rec->stroke;
    first_pt = &stroke->pts[0];
    last_pt = &stroke->pts[stroke->num_pts - 1];
    stroke_orientation = atan2(last_pt->y - first_pt->y,
			       last_pt->x - first_pt->x);

    /* The stroke coordinates we get here were already rotated to
       compensate for the inferred tilt, so we compare orientation
       directly with the golden value from the orient_action. */
    correction = stroke_orientation - orient_data->orientation;
    if (correction > 0 && correction > M_PI) {
	correction -= 2 * M_PI;
    }
    if (correction < 0 && correction < -M_PI) {
	correction += 2 * M_PI;
    }

    rec_history_nudge_orientation(&rec->history, correction);
    if (rec->history.orientation > rec->rotation_limit)
	rec->history.orientation = rec->rotation_limit;
    if (rec->history.orientation < -rec->rotation_limit)
	rec->history.orientation = -rec->rotation_limit;
}

static void reset_mode(rec_t *rec)
{
    action_t reset_mode_action;
    action_item_t reset_mode_action_item;

    if (rec->current_mode != rec->persistent_mode && rec->persistent_mode) {
	action_init(&reset_mode_action);
	action_item_mode_init(&reset_mode_action_item, rec->persistent_mode, 1);
	action_add_item(&reset_mode_action, reset_mode_action_item);

	perform_action(rec, &reset_mode_action);
	action_deinit(&reset_mode_action);
    }
}

