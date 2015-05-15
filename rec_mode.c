/* rec_mode.c -- data for a mode within the recognizer

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
#include <errno.h>

#include "rec.h"
#include "rec_mode.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int rec_mode_init(rec_mode_t *mode, char *name)
{
    mode->name = strdup(name);

    rec_mode_list_init(&mode->parents);
    gesture_list_init(&mode->gestures);

    return 0;
}

void rec_mode_deinit(rec_mode_t *mode)
{
    free(mode->name);
    rec_mode_list_deinit_shallow(&mode->parents);
    gesture_list_deinit(&mode->gestures);
}

int rec_mode_add_gesture(rec_mode_t *mode, gesture_t *gesture)
{
    return gesture_list_append(&mode->gestures, gesture);
}

int rec_mode_add_gestures(rec_mode_t *mode, gesture_list_t *gestures)
{
    if (mode->gestures.num_gestures == 0) {
	mode->gestures = *gestures;
    } else {
	if (mode->gestures.num_gestures > gestures->num_gestures) {
	    gesture_list_append_list(&mode->gestures, gestures);
	    gesture_list_deinit_shallow(gestures);
	} else {
	    gesture_list_append_list(gestures, &mode->gestures);
	    gesture_list_deinit_shallow(&mode->gestures);
	    mode->gestures = *gestures;
	}
    }

    return 0;
}

int rec_mode_add_parents(rec_mode_t *mode, rec_mode_list_t *parents)
{
    int i;

    for (i=0; i < parents->num_modes; i++) {
	rec_mode_list_append(&mode->parents, parents->modes[i]);
    }

    return 0;
}

int rec_mode_recognize(rec_mode_t *mode, stroke_t *stroke,
		       action_t **action_ret, double *probability_ret)
{
    gesture_t *gesture;
    double probability;
    int i;

    for (i=0; i < mode->gestures.num_gestures; i++) {
	gesture = &mode->gestures.gestures[i];
	probability = gesture_recognize(gesture, stroke);
	if (probability > *probability_ret) {
	    *probability_ret = probability;
	    *action_ret = &gesture->action;

	    /* Break out of recursion early if we have absolute certainty */
	    if (*probability_ret >= 1.0) {
		return 1;
	    }
	}
    }

    /* Now look in parent modes */
    for (i=0; i < mode->parents.num_modes; i++) {
	if (rec_mode_recognize(mode->parents.modes[i], stroke,
			       action_ret, probability_ret))
	    return 1;
    }

    return 0;
}

int rec_mode_list_init(rec_mode_list_t *list)
{
    list->num_modes = 0;
    list->modes = NULL;

    return 0;
}

void rec_mode_list_deinit_shallow(rec_mode_list_t *list)
{
    if (list->modes) {
	free(list->modes);
	list->modes = NULL;
    }
    list->num_modes = 0;
}

/* Full, deep deinit */
/* This function doesn't make sense as no one would have the chance to
   call free on all of the rec_mode_t *s
void rec_mode_list_deinit(rec_mode_list_t *list)
{
    int i;

    for (i=0; i < list->num_modes; i++) {
	rec_mode_deinit(list->modes[i]);
    }
    rec_mode_list_deinit_shalllow(list);
}
*/

int rec_mode_list_append(rec_mode_list_t *list, rec_mode_t *mode)
{
    rec_mode_t **new_modes;

    list->num_modes++;

    new_modes = realloc(list->modes, list->num_modes * sizeof(rec_mode_t *));
    if (new_modes == NULL) {
	fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	list->num_modes--;
	return ENOMEM;
    }

    list->modes = new_modes;
    list->modes[list->num_modes - 1] = mode;

    return 0;
}

