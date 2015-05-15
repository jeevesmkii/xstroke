/* rec.h - top-level interface to a recognizer

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

#ifndef REC_H
#define REC_H

#include "feature.h"
#include "action.h"
#include "gesture.h"
#include "rec_history.h"
#include "rec_callback.h"
#include "rec_mode.h"
#include "stroke.h"
#include "log.h"

#define REC_GLOBAL_MODE_NAME "__GLOBAL__"
#define REC_FALLBACK_MODE_NAME "__FALLBACK__"
#define REC_UNKNOWN_STROKE_STR  "<unknown>"
#define REC_LOG_FILE_NAME     "strokes.log"
#define REC_ERR_LOG_FILE_NAME "errors.log"

struct rec
{
    struct rec_callback *callbacks[LAST_ACTION];

    struct rec_engine_list engines;
    struct rec_mode_list modes;

    struct rec_mode *current_mode;
    struct rec_mode *persistent_mode;
    struct rec_mode *global_mode;
    struct rec_mode *fallback_mode;

    rec_history_t history;

    stroke_t stroke;

    int width;
    int height;

    log_t log;
    log_t err_log;
    int log_raw;

    int rotation;
    double rotation_limit;
};
typedef struct rec rec_t;

#include "args.h"

int rec_init(rec_t *rec, args_t *args, int width, int height);
void rec_deinit(rec_t *rec);

void rec_set_size(rec_t *rec, int width, int height);

struct rec_engine *rec_get_engine(rec_t *rec, char *engine_name);
struct rec_mode *rec_get_mode(rec_t *rec, char *mode_name);
int rec_register_callback(rec_t *rec, action_type_t type, rec_cb_fun_t cb_fun, void *data);

void rec_begin_stroke(rec_t *rec, int x, int y, unsigned long time);
void rec_extend_stroke(rec_t *rec, int x, int y, unsigned long time);
void rec_end_stroke(rec_t *rec, int x, int y, unsigned long time);

int rec_verify(rec_t *rec, char *file_name);

#endif
