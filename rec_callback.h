/* rec_callback.h - callbacks for recognizer actions

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

#ifndef REC_CALLBACK_H
#define REC_CALLBACK_H

#include "action.h"

typedef void (*rec_cb_fun_t)(void *action_item, void *data);
struct rec_callback
{
    rec_cb_fun_t cb_fun;
    void *data;
    struct rec_callback *next;
};
typedef struct rec_callback rec_callback_t;

int rec_callback_init(rec_callback_t *callback, rec_cb_fun_t cb_fun, void *data);
void rec_callback_deinit(rec_callback_t *callback);

#endif
