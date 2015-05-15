/* action_item.h -- data for a single recognizer action

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

#ifndef ACTION_ITEM_H
#define ACTION_ITEM_H

#include <X11/X.h>
#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <X11/keysymdef.h>
#include <X11/Xutil.h>

enum action_type
{
    ACTION_KEY,
    ACTION_BUTTON,
    ACTION_MODE,
    ACTION_EXEC,
    ACTION_ORIENT,
    LAST_ACTION /* Marker for end of list */
};
typedef enum action_type action_type_t;

struct action_item
{
    action_type_t type;
    void* data; 
};
typedef struct action_item action_item_t;

#define ACTION_KEY_IS_PRESS       0x1
#define ACTION_KEY_IS_MODIFIER    0x2
#define ACTION_KEY_REQUIRES_SHIFT 0x4
struct action_key_data
{
    KeySym keysym;
    KeyCode keycode;
    int flags;
};
typedef struct action_key_data action_key_data_t;

struct action_button_data
{
    int button;
};
typedef struct action_button_data action_button_data_t;

struct action_mode_data
{
    struct rec_mode *mode;
    int permanent;
};
typedef struct action_mode_data action_mode_data_t;

struct action_exec_data
{
    char *exec;
};
typedef struct action_exec_data action_exec_data_t;

struct action_orient_data
{
    double orientation;
};
typedef struct action_orient_data action_orient_data_t;

int action_item_key_init(action_item_t *item, char *key, int press);
int action_item_button_init(action_item_t *item, int button);
int action_item_mode_init(action_item_t *item, struct rec_mode *mode,
			  int permanent);
int action_item_exec_init(action_item_t *item, char *exec);
int action_item_orient_init(action_item_t *item, double orientation);
void action_item_deinit(action_item_t *item);
char *action_item_str_alloc(action_item_t *item);

action_key_data_t *action_key_data_alloc(char *key, int press);
int action_key_data_init(action_key_data_t *key_data, char *key, int press);
int action_keysym_data_init(action_key_data_t *key_data, KeySym keysym, int press);
void action_key_data_deinit(action_key_data_t *key_data);

action_button_data_t *action_button_data_alloc(int button);
int action_button_data_init(action_button_data_t *button_data, int button);
void action_button_data_deinit(action_button_data_t *button_data);

action_mode_data_t *action_mode_data_alloc(struct rec_mode *mode, int permanent);
int action_mode_data_init(action_mode_data_t *mode_data,
			  struct rec_mode *mode, int permanent);
void action_mode_data_deinit(action_mode_data_t *mode_data);

action_exec_data_t *action_exec_data_alloc(char *exec);
int action_exec_data_init(action_exec_data_t *exec_data, char *exec);
void action_exec_data_deinit(action_exec_data_t *exec_data);

action_orient_data_t *action_orient_data_alloc(double orientation);
int action_orient_data_init(action_orient_data_t *orient_data, double orientation);
void action_orient_data_deinit(action_orient_data_t *orient_data);

#endif
