/* control_win.h - the control interface window

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

#ifndef CONTROL_WIN_H
#define CONTROL_WIN_H

#include <X11/Xft/Xft.h>
#ifdef USE_PIXMAPS
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#endif

#include "args.h"
#include "action.h"
#include "xlp_win.h"

#define HOLD_DRIFT_THRESHOLD 12

#define MAX_MSG 18

#define INACTIVE_ICON 0
#define ACTIVE_ICON   1

struct control_win
{
    struct xstroke *xstroke;
    xlp_win_t win;
    xlp_win_t root_win;
    XftColor active_color;
    XftColor inactive_color;
    XftColor *current_color;
    Cursor cursor;

    char mode[MAX_MSG+1];
    char mod_str[MAX_MSG+1]; /* Prefix for displaying modifier states */
    char msg[MAX_MSG+1];

    /* XXX: The rest of these really belong in some other module */
    int recognizing;
    int stroking;
    int button;
    int hold_time_ms;
    int stroke_initial_x;
    int stroke_initial_y;
    long stroke_button;
    int stroke_motion_beyond_threshold;
    int auto_disable;
    struct timeval stroke_start_tv;

#ifdef USE_PIXMAPS
    Pixmap icon[2];
    Pixmap icon_mask[2];
    Pixmap backing_mask;
    XftDraw *backing_draw;
   
    GC gc;
    GC gc_mask;
    XpmAttributes xpm_attr;
#endif
};
typedef struct control_win control_win_t;
struct rec_mode;

int control_win_init(control_win_t *control, struct xstroke *xstroke, args_t *args);
void control_win_deinit(control_win_t *control);

void control_win_set_mode(control_win_t *control, struct rec_mode *mode);
void control_win_update_modifiers(control_win_t *control);

#endif
