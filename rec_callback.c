/* rec_callback.c - callbacks for recognizer actions

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
#include "rec_callback.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int rec_callback_init(rec_callback_t *callback, rec_cb_fun_t cb_fun, void *data)
{
    callback->cb_fun = cb_fun;
    callback->data = data;
    callback->next = NULL;

    return 0;
}

void rec_callback_deinit(rec_callback_t *callback)
{
    callback->next = NULL;
    callback->data = NULL;
    callback->cb_fun = NULL;
}
