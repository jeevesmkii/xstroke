/* xstroke.h - toplevel data for xstroke

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

#ifndef XSTROKE_H
#define XSTROKE_H

/*
#define XSTROKE_DEBUG
*/

#ifdef XSTROKE_DEBUG
# define XSTROKE_SYNCHRONOUS
# define XSTROKE_NOGRAB_SERVER
/*
# define REC_PARSE_DEBUG
# define XSTROKE_DEBUG_ROTATION
*/
#endif

/* Undefine for standalone control window */
#define USE_SYSTEM_TRAY
/* Undefine to not use pixmaps in control window */
#define USE_PIXMAPS

#include <X11/Xlib.h>

#include "backing.h"
#include "brush.h"
#include "control_win.h"
#include "rec.h"
#include "args.h"
#include "action.h"
#include "xlp_win.h"

struct xstroke
{
    xlp_t xlp;

    XModifierKeymap *modifier_map;

    control_win_t control_win;
    backing_t backing;
    brush_t brush;

    rec_t rec;
    action_key_data_t modifiers[8];
};
typedef struct xstroke xstroke_t;

void xstroke_exit(xstroke_t *xstroke, int exit_code);
void xstroke_reload(xstroke_t *xstroke);
void xstroke_button_press(xstroke_t *xstroke, XButtonEvent *bev);
void xstroke_motion_notify(xstroke_t *xstroke, XMotionEvent *mev);
void xstroke_button_release(xstroke_t *xstroke, XButtonEvent *bev);

#endif /* XSTROKE_H */
