/* action_item.c -- data for a single recognizer action

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
#include "action_item.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int action_item_key_init(action_item_t *item, char *key, int press)
{
    item->type = ACTION_KEY;
    item->data = action_key_data_alloc(key, press);

    return item->data ? 0 : ENOMEM;
}

int action_item_button_init(action_item_t *item, int button)
{
    item->type = ACTION_BUTTON;
    item->data = action_button_data_alloc(button);

    return item->data ? 0 : ENOMEM;
}

int action_item_mode_init(action_item_t *item, rec_mode_t *mode, int permanent)
{
    item->type = ACTION_MODE;
    item->data = action_mode_data_alloc(mode, permanent);

    return item->data ? 0 : ENOMEM;
}

int action_item_exec_init(action_item_t *item, char *exec)
{
    item->type = ACTION_EXEC;
    item->data = action_exec_data_alloc(exec);

    return item->data ? 0 : ENOMEM;
}

int action_item_orient_init(action_item_t *item, double orientation)
{
    item->type = ACTION_ORIENT;
    item->data = action_orient_data_alloc(orientation);

    return item->data ? 0 : ENOMEM;
}

void action_item_deinit(action_item_t *item)
{
    switch (item->type) {
    case ACTION_KEY:
	action_key_data_deinit(item->data);
	break;
    case ACTION_BUTTON:
	action_button_data_deinit(item->data);
	break;
    case ACTION_MODE:
	action_mode_data_deinit(item->data);
	break;
    case ACTION_EXEC:
	action_exec_data_deinit(item->data);
	break;
    case ACTION_ORIENT:
	action_orient_data_deinit(item->data);
	break;
    default:
	fprintf(stderr, "%s: Unknown action type: %d\n", __FUNCTION__, item->type);
	break;
    }
    free(item->data);
    item->data = NULL;
    item->type = 0;
}

action_key_data_t *action_key_data_alloc(char *key, int press)
{
    action_key_data_t *data;

    data = malloc(sizeof(action_key_data_t));
    if (data == NULL) {
	fprintf(stderr, "%s: Out of memory", __FUNCTION__);
	return NULL;
    }
    action_key_data_init(data, key, press);
    return data;
}

int action_key_data_init(action_key_data_t *key_data, char *key, int press)
{
    KeySym keysym = XStringToKeysym(key);

    if (keysym == NoSymbol) {
	fprintf(stderr, "%s: ERROR: No Keysym found for %s\n", __FUNCTION__, key);
	return EINVAL;
    }

    return action_keysym_data_init(key_data, keysym, press);
}

int action_keysym_data_init(action_key_data_t *key_data, KeySym keysym, int press)
{
    key_data->keysym = keysym;
    key_data->keycode = 0;
    key_data->flags = 0;
    if (press) {
	key_data->flags |= ACTION_KEY_IS_PRESS;
    }
    if (IsModifierKey(keysym)) {
	key_data->flags |= ACTION_KEY_IS_MODIFIER;
    }

    return 0;
}

void action_key_data_deinit(action_key_data_t *key_data)
{
    key_data->keysym = 0;
    key_data->keycode = 0;
    key_data->flags = 0;
}

action_button_data_t *action_button_data_alloc(int button)
{
    action_button_data_t *data;

    data = malloc(sizeof(action_button_data_t));
    if (data == NULL) {
	fprintf(stderr, "%s: Out of memory", __FUNCTION__);
	return NULL;
    }
    action_button_data_init(data, button);
    return data;
}

int action_button_data_init(action_button_data_t *button_data, int button)
{
    button_data->button = button;

    return 0;
}

void action_button_data_deinit(action_button_data_t *button_data)
{
    button_data->button = 0;
}

action_mode_data_t *action_mode_data_alloc(struct rec_mode *mode, int permanent)
{
    action_mode_data_t *data;

    data = malloc(sizeof(action_mode_data_t));
    if (data == NULL) {
	fprintf(stderr, "%s: Out of memory", __FUNCTION__);
	return NULL;
    }
    action_mode_data_init(data, mode, permanent);
    return data;
}

int action_mode_data_init(action_mode_data_t *mode_data,
			  struct rec_mode *mode, int permanent)
{
    mode_data->mode = mode;
    mode_data->permanent = permanent;

    return 0;
}

void action_mode_data_deinit(action_mode_data_t *mode_data)
{
    mode_data->permanent = 0;
    mode_data->mode = NULL;
}

action_exec_data_t *action_exec_data_alloc(char *exec)
{
    action_exec_data_t *data;

    data = malloc(sizeof(action_exec_data_t));
    if (data == NULL) {
	fprintf(stderr, "%s: Out of memory", __FUNCTION__);
	return NULL;
    }
    action_exec_data_init(data, exec);
    return data;
}

int action_exec_data_init(action_exec_data_t *exec_data, char *exec)
{
    exec_data->exec = strdup(exec);

    return 0;
}

void action_exec_data_deinit(action_exec_data_t *exec_data)
{
    free(exec_data->exec);
    exec_data->exec = NULL;
}

action_orient_data_t *action_orient_data_alloc(double orientation)
{
    action_orient_data_t *data;

    data = malloc(sizeof(action_orient_data_t));
    if (data == NULL) {
	fprintf(stderr, "%s: Out of memory", __FUNCTION__);
	return NULL;
    }
    action_orient_data_init(data, orientation);
    return data;
}

int action_orient_data_init(action_orient_data_t *orient_data, double orientation)
{
    orient_data->orientation = orientation;
    return 0;
}

void action_orient_data_deinit(action_orient_data_t *orient_data)
{
    /* nothing to see here, move along... */
}

char *action_item_str_alloc(action_item_t *item)
{
    char *str = NULL;

    switch (item->type) {
    case ACTION_KEY:
    {
	action_key_data_t *key_data = (action_key_data_t *) item->data;
	sprintf_alloc(&str, "Key %s", XKeysymToString(key_data->keysym));
	break;
    }
    case ACTION_BUTTON:
    {
	action_button_data_t *button_data = (action_button_data_t *) item->data;
	sprintf_alloc(&str, "Button %d", button_data->button);
	break;
    }
    case ACTION_MODE:
    {
	action_mode_data_t *mode_data = (action_mode_data_t *) item->data;
	sprintf_alloc(&str, "%s %s",
		      mode_data->permanent ? "ModeLock" : "ModeShift",
		      mode_data->mode->name);
	break;
    }
    case ACTION_EXEC:
    {
	action_exec_data_t *exec_data = (action_exec_data_t *) item->data;
	sprintf_alloc(&str, "Exec \"%s\"", exec_data->exec);
	break;
    }
    case ACTION_ORIENT:
    {
	action_orient_data_t *orient_data = (action_orient_data_t *) item->data;
	sprintf_alloc(&str, "OrientationCorrection %f",
		      (180 / M_PI) * orient_data->orientation);
	break;
    }
    default:
	fprintf(stderr, "%s: Unknown action type: %d\n", __FUNCTION__, item->type);
	break;
    }
    return str;
}
