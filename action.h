/* action.h -- data for recognizer actions (one or more action_items)

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

#ifndef ACTION_H
#define ACTION_H

#include "action_item.h"

struct action
{
    int num_items;
    action_item_t *items;
};
typedef struct action action_t;

int action_init(action_t *action);
void action_deinit(action_t *action);
int action_add_item(action_t *action, action_item_t item);
char *action_str_alloc(action_t *action);

#endif
