/* rec_mode.h -- data for a mode within the recognizer

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

#ifndef REC_MODE_H
#define REC_MODE_H

#include "gesture.h"

struct rec_mode_list
{
    int num_modes;
    struct rec_mode **modes;
};
typedef struct rec_mode_list rec_mode_list_t;

struct rec_mode
{
    char *name;

    struct rec_mode_list parents;

    struct gesture_list gestures;
};
typedef struct rec_mode rec_mode_t;

int rec_mode_init(rec_mode_t *mode, char *name);
void rec_mode_deinit(rec_mode_t *mode);

int rec_mode_add_parent(rec_mode_t *mode, rec_mode_t *parent);
int rec_mode_add_parents(rec_mode_t *mode, rec_mode_list_t *parents);
int rec_mode_add_gesture(rec_mode_t *mode, gesture_t *gesture);
int rec_mode_add_gestures(rec_mode_t *mode, gesture_list_t *gestures);

int rec_mode_recognize(rec_mode_t *mode, stroke_t *stroke,
		       action_t **action_ret, double *probability_ret);

int rec_mode_list_init(rec_mode_list_t *list);
void rec_mode_list_deinit_shallow(rec_mode_list_t *list);
int rec_mode_list_append(rec_mode_list_t *list, rec_mode_t *mode);

#endif
