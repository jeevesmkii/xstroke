/* rec_engine.h -- generic stroke recognition engine interface

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

#ifndef REC_ENGINE_H
#define REC_ENGINE_H

#include "stroke.h"

#define REC_LIB_PREFIX "rec_"
#define REC_ENGINE_FUNCS_SYMBOL "engine_funcs"

struct rec_engine;

struct rec_engine_funcs
{
    int    (*priv_alloc)(struct rec_engine *engine);
    void   (*priv_free)(struct rec_engine *engine);
    void * (*feature_data_alloc)(struct rec_engine *engine,
				 char *feature_data_str);
    void   (*feature_data_free)(struct rec_engine *engine, void *feature_data);
    void   (*classify_stroke)(struct rec_engine *engine, stroke_t *stroke);
    char * (*classification_str_alloc)(struct rec_engine *engine, stroke_t *stroke);
    void   (*free_classification)(struct rec_engine *engine, stroke_t *stroke);
    double (*recognize_stroke)(struct rec_engine *engine, stroke_t *stroke,
			       void *feature_data);
    int    (*set_option)(struct rec_engine *engine, char *option, char *value);
};
typedef struct rec_engine_funcs rec_engine_funcs_t;

struct rec_engine
{
    struct rec *rec;
    char *name;
    int num;
    rec_engine_funcs_t *funcs;
    void *priv;
};
typedef struct rec_engine rec_engine_t;

struct rec_engine_list
{
    int num_engines;
    rec_engine_t **engines;
};
typedef struct rec_engine_list rec_engine_list_t;

int rec_engine_init(rec_engine_t *engine, struct rec *rec, char *engine_name);
void rec_engine_deinit(rec_engine_t *engine);

void *rec_engine_feature_data_alloc(rec_engine_t *engine, char *feature_data_str);
void rec_engine_feature_data_free(rec_engine_t *engine, void *feature_data);

void rec_engine_classify_stroke(rec_engine_t *engine, stroke_t *stroke);
char *rec_engine_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke);
void rec_engine_free_classification(rec_engine_t *engine, stroke_t *stroke);

double rec_engine_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
				   void *feature_data);

int rec_engine_set_option(rec_engine_t *engine, char *name, char *value);

int rec_engine_list_init(rec_engine_list_t *list);
void rec_engine_list_deinit_shallow(rec_engine_list_t *list);
int rec_engine_list_append(rec_engine_list_t *list, rec_engine_t *engine);

#endif
