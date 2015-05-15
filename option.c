/* option.c -- Simple data structure for storing a name/value pair

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

#include "option.h"

int option_init(option_t *option, char *name, char *value)
{
    option->name = name;
    option->value = value;

    return 0;
}

void option_deinit(option_t *option)
{
    option->name = NULL;
    option->value = NULL;
}

int option_list_init(option_list_t *list)
{
    list->num_options = 0;
    list->options = NULL;

    return 0;
}

int option_list_append(option_list_t *list, option_t *option)
{
    option_t *new_options;

    list->num_options++;

    new_options = realloc(list->options, list->num_options * sizeof(option_t));
    if (new_options == NULL) {
	fprintf(stderr, "%s: Out of memory.\n", __FUNCTION__);
	list->num_options--;
	return ENOMEM;
    }

    list->options = new_options;
    list->options[list->num_options - 1] = *option;

    return 0;
}

void option_list_deinit(option_list_t *list)
{
    free(list->options);
    list->options = NULL;
    list->num_options = 0;
}
