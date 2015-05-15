/* action.c -- data for recognizer actions (one or more action_items)

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
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include <X11/Xlib.h>

#include "xstroke.h"
#include "rec.h"
#include "action.h"
#include "sprintf_alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int action_init(action_t *action)
{
    action->num_items = 0;
    action->items = NULL;

    return 0;
}

void action_deinit(action_t *action)
{
    int i;

    for (i=0; i < action->num_items; i++) {
	action_item_deinit(&action->items[i]);
    }
    free(action->items);
}

int action_add_item(action_t *action, action_item_t item)
{
    action->num_items++;
    action->items = realloc(action->items,
			    sizeof(action_item_t) * action->num_items);
    if (action->items == NULL) {
	action->num_items = 0;
	fprintf(stderr, "%s: out of memory.\n", __FUNCTION__);
	return ENOMEM;
    }
    action->items[action->num_items - 1] = item;

    return 0;
}

char *action_str_alloc(action_t *action)
{
    int i;
    char *item_str;
    char *str = strdup("");

    if (action) {
	for (i=0; i < action->num_items; i++) {
	    item_str = action_item_str_alloc(&action->items[i]);
	    sprintf_append_alloc(&str, "%s%s", item_str,
				 i == (action->num_items - 1) ? "" : ", ");
	    free(item_str);
	}
    }

    return str;
}
