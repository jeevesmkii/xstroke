/* option.h -- Simple data structure for storing a name/value pair

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

#ifndef OPTION_H
#define OPTION_H

struct option
{
    char *name;
    char *value;
};
typedef struct option option_t;

struct option_list
{
    int num_options;
    option_t *options;
};
typedef struct option_list option_list_t;

int option_init(option_t *option, char *name, char *value);
void option_deinit(option_t *option);

int option_list_init(option_list_t *list);
void option_list_deinit(option_list_t *list);

int option_list_append(option_list_t *list, option_t *option);

#endif
