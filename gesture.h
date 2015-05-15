/* gesture.h -- data used to recognize strokes, (made up of several features)

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

#ifndef GESTURE_H
#define GESTURE_H

#include "action.h"
#include "feature.h"

#define GESTURE_LIST_GROWTH_INC 10

struct gesture
{
    action_t action;

    feature_list_t features;
};
typedef struct gesture gesture_t;

struct gesture_list
{
    int num_gestures;
    int max_gestures;
    gesture_t *gestures;
};
typedef struct gesture_list gesture_list_t;

int gesture_init(gesture_t *gesture, action_t action, feature_list_t features);
void gesture_deinit(gesture_t *gesture);

double gesture_recognize(gesture_t *gesture, stroke_t *stroke);

int gesture_list_init(gesture_list_t *list);
void gesture_list_deinit_shallow(gesture_list_t *list);
void gesture_list_deinit(gesture_list_t *list);

int gesture_list_append(gesture_list_t *list, gesture_t *gesture);
int gesture_list_append_list(gesture_list_t *list, gesture_list_t *other);

#endif
