/* gesture.c -- data used to recognize strokes, (made up of several features)

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
#include "rec_mode.h"
#include "gesture.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int gesture_init(gesture_t *gesture, action_t action, feature_list_t features)
{
    gesture->action = action;
    gesture->features = features;

    return 0;
}

void gesture_deinit(gesture_t *gesture)
{
    action_deinit(&gesture->action);
    feature_list_deinit(&gesture->features);
}

double gesture_recognize(gesture_t *gesture, stroke_t *stroke)
{
    double probability = 1.0;
    double feature_probability;
    int engine_num;
    int i;

    if (gesture->features.num_features == 0) {
	return 0.0;
    }
    
    for (i=0; i < gesture->features.num_features; i++) {
	engine_num = gesture->features.features[i].engine->num;
	feature_probability = feature_recognize(&gesture->features.features[i],
						stroke);
	/* For now, assume independent features */
	probability *= feature_probability;
	if (probability == 0.0)
	    break;
    }

    return probability;
}

int gesture_list_init(gesture_list_t *list)
{
    list->num_gestures = 0;
    list->max_gestures = 0;
    list->gestures = NULL;

    return 0;
}

void gesture_list_deinit_shallow(gesture_list_t *list)
{
    free(list->gestures);
    list->max_gestures = 0;
    list->num_gestures = 0;
}

/* Full, deep deinit */
void gesture_list_deinit(gesture_list_t *list)
{
    int i;

    for (i=0; i < list->num_gestures; i++) {
	gesture_deinit(&list->gestures[i]);
    }
    gesture_list_deinit_shallow(list);
}

int gesture_list_append(gesture_list_t *list, gesture_t *gesture)
{
    gesture_t *new_gestures;

    list->num_gestures++;

    if (list->num_gestures > list->max_gestures) {
	list->max_gestures += GESTURE_LIST_GROWTH_INC;
	new_gestures = realloc(list->gestures,
			       list->max_gestures * sizeof(gesture_t));
	if (new_gestures) {
	    list->gestures = new_gestures;
	} else {
	    fprintf(stderr, "%s: out of memory\n", __FUNCTION__);
	    list->max_gestures -= GESTURE_LIST_GROWTH_INC;
	    list->num_gestures--;
	    return ENOMEM;
	}
    }

    list->gestures[list->num_gestures - 1] = *gesture;

    return 0;
}

int gesture_list_append_list(gesture_list_t *list, gesture_list_t *other)
{
    int i;

    for (i=0; i < other->num_gestures; i++) {
	gesture_list_append(list, &other->gestures[i]);
    }

    return 0;
}
