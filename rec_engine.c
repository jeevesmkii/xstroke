/* rec_engine.c -- generic stroke recognition engine interface

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
#include <string.h>
#include <dlfcn.h>
#include <errno.h>

#include "rec.h"
#include "rec_engine.h"
#include "anchor_engine.h"
#include "dir_engine.h"
#include "grid_engine.h"
#include "raw_engine.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

struct engine_func_map
{
    char *name;
    rec_engine_funcs_t funcs;
};
typedef struct engine_func_map engine_func_map_t;

static engine_func_map_t static_engines[] = {
    { "anchor", 
      {
	  anchor_priv_alloc,
	  anchor_priv_free,
	  anchor_feature_data_alloc,
	  anchor_feature_data_free,
	  anchor_classify_stroke,
	  anchor_classification_str_alloc,
	  anchor_free_classification,
	  anchor_recognize_stroke,
	  anchor_set_option
      }
    },
    { "dir", 
      {
	  dir_priv_alloc,
	  dir_priv_free,
	  dir_feature_data_alloc,
	  dir_feature_data_free,
	  dir_classify_stroke,
	  dir_classification_str_alloc,
	  dir_free_classification,
	  dir_recognize_stroke,
	  dir_set_option
      }
    },
    { "dir_length", 
      {
	  dir_priv_alloc,
	  dir_priv_free,
	  dir_feature_data_alloc,
	  dir_feature_data_free,
	  dir_length_classify_stroke,
	  dir_classification_str_alloc,
	  dir_free_classification,
	  dir_recognize_stroke,
	  dir_set_option
      }
    },
    { "grid", 
      {
	  grid_priv_alloc,
	  grid_priv_free,
	  grid_feature_data_alloc,
	  grid_feature_data_free,
	  grid_classify_stroke,
	  grid_classification_str_alloc,
	  grid_free_classification,
	  grid_recognize_stroke,
	  grid_set_option
      }
    },
    { "raw", 
      {
	  raw_priv_alloc,
	  raw_priv_free,
	  raw_feature_data_alloc,
	  raw_feature_data_free,
	  raw_classify_stroke,
	  raw_classification_str_alloc,
	  raw_free_classification,
	  raw_recognize_stroke,
	  raw_set_option
      }
    }
};
static int num_static_engines = sizeof(static_engines) / sizeof(engine_func_map_t);

int rec_engine_init(rec_engine_t *engine, rec_t *rec, char *engine_name)
{
    int i;
    int err;
    void *dlh;
    char *error;
    char *libname;
    int chars_matched;
    int ret_val = 0;

    engine->rec = rec;
    engine->name = strdup(engine_name);
    engine->num = 0;
    engine->priv = NULL;

    engine->funcs = NULL;
    chars_matched = 0;
    for (i=0; i < num_static_engines; i++) {
	char *name_i = static_engines[i].name;
	int name_i_len = strlen(name_i);
	if (strncmp(engine_name, name_i, name_i_len) == 0) {
	    if (name_i_len > chars_matched) {
		chars_matched = name_i_len;
		engine->funcs = &static_engines[i].funcs;
	    }
	}
    }

    if (engine->funcs == NULL) {
	sprintf_alloc(&libname, "%s%s", REC_LIB_PREFIX, engine_name);
	dlh = dlopen(libname, RTLD_LAZY);
	if (dlh == NULL) {
	    fprintf(stderr, "%s: failed to open library %s: %s",
		    __FUNCTION__, libname, dlerror());
	    ret_val = EINVAL;
	    goto CLEANUP;
	}

	engine->funcs = dlsym(dlh, REC_ENGINE_FUNCS_SYMBOL);
	if ((error = dlerror()) != NULL) {
	    fprintf(stderr, "%s: Failed to find symbol %s in %s: %s\n",
		    __FUNCTION__, REC_ENGINE_FUNCS_SYMBOL, libname, error);
	    ret_val = EINVAL;
	    goto CLEANUP;
	}

      CLEANUP:
	if (dlh) {
	    dlclose(dlh);
	}
	free(libname);
    }

    if (engine->funcs->priv_alloc) {
	err = (engine->funcs->priv_alloc)(engine);
	if (err) {
	    fprintf(stderr, "%s: An error occurred during %s:priv_alloc: %s\n",
		    __FUNCTION__, engine->name, strerror(err));
	    ret_val = err;
	}
    }

    return ret_val;
}

void rec_engine_deinit(rec_engine_t *engine)
{
    if (engine->funcs->priv_free) {
	(engine->funcs->priv_free)(engine);
    }
    engine->funcs = NULL;
    engine->num = 0;
    free(engine->name);
}

void *rec_engine_feature_data_alloc(rec_engine_t *engine, char *feature_data_str)
{
    if (engine->funcs->feature_data_alloc == NULL) {
	return NULL;
    }

    return (engine->funcs->feature_data_alloc)(engine, feature_data_str);
}

void rec_engine_feature_data_free(rec_engine_t *engine, void *feature_data)
{
    if (engine->funcs->feature_data_free) {
	(engine->funcs->feature_data_free)(engine, feature_data);
    }
}

void rec_engine_classify_stroke(rec_engine_t *engine, stroke_t *stroke)
{
    if (engine->funcs->classify_stroke) {
	(engine->funcs->classify_stroke)(engine, stroke);
    }
}

char *rec_engine_classification_str_alloc(rec_engine_t *engine, stroke_t *stroke)
{
    if (engine->funcs->classification_str_alloc == NULL) {
	return strdup("");
    }

    return (engine->funcs->classification_str_alloc)(engine, stroke);
}

void rec_engine_free_classification(rec_engine_t *engine, stroke_t *stroke)
{
    if (engine->funcs->free_classification) {
	(engine->funcs->free_classification)(engine, stroke);
    }
}

double rec_engine_recognize_stroke(rec_engine_t *engine, stroke_t *stroke,
				   void *feature_data)
{
    if (engine->funcs->recognize_stroke == NULL) {
	return 1.0;
    }

    return (engine->funcs->recognize_stroke)(engine, stroke, feature_data);
}

int rec_engine_set_option(rec_engine_t *engine, char *name, char *value)
{
    if (engine->funcs->set_option == NULL) {
	return 0;
    }

    return (engine->funcs->set_option)(engine, name, value);
}

int rec_engine_list_init(rec_engine_list_t *list)
{
    list->num_engines = 0;
    list->engines = NULL;

    return 0;
}

void rec_engine_list_deinit_shallow(rec_engine_list_t *list)
{
    if (list->engines) {
	free(list->engines);
	list->engines = NULL;
    }
    list->num_engines = 0;
}

/* Full, deep deinit */
/* This function doesn't make sense as no one would have the chance to
   call free on all of the rec_engine_t *s
void rec_engine_list_deinit(rec_engine_list_t *list)
{
    int i;

    for (i=0; i < list->num_engines; i++) {
	rec_engine_deinit(list->engines[i]);
    }
    rec_engine_list_deinit_shallow(list);
}
*/

int rec_engine_list_append(rec_engine_list_t *list, rec_engine_t *engine)
{
    rec_engine_t **new_engines;

    list->num_engines++;

    new_engines = realloc(list->engines, list->num_engines * sizeof(rec_engine_t *));
    if (new_engines == NULL) {
	fprintf(stderr, "%s: Out of memory.\n", __FUNCTION__);
	list->num_engines--;
	return ENOMEM;
    }

    list->engines = new_engines;
    list->engines[list->num_engines - 1] = engine;
    engine->num = list->num_engines - 1;

    return 0;
}
