/* control_win.c - the control interface window

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
#include <ctype.h>

#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/cursorfont.h>
#include <X11/extensions/XTest.h>

#include "args.h"
#include "xstroke.h"
#include "xlp_win.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifdef USE_SYSTEM_TRAY
#include "tray.h"
#endif

#ifdef USE_PIXMAPS
#include "xstroke_inactive.xpm"
#include "xstroke_active.xpm"
#endif

static void client_message_cb(XEvent *xev, void *data);
static void cwin_button_press_cb(XEvent *xev, void *data);
static void root_button_press_cb(XEvent *xev, void *data);
static void root_motion_notify_cb(XEvent *xev, void *data);
static void root_button_release_cb(XEvent *xev, void *data);
static void expose_cb(XEvent *xev, void *data);
static void hold_timeout_cb(struct timeval start_tv, void *data);

static void control_win_toggle_recognition(control_win_t *control);
static int window_wants_recognition(Display *dpy, Window window);
static Window find_deepest_window(Display *dpy,
				  Window grandfather, Window parent,
				  int x, int y);

static void grab_button(control_win_t *control);
static void ungrab_button(control_win_t *control);
static int mask_for_button(int button);

static void refresh(control_win_t *control);
static void erase_window(control_win_t *control);
static void update_msg(control_win_t *control);
static void draw_msg(control_win_t *control);

static Atom DELETE_WINDOW_ATOM;
static Atom RECOGNIZE_PROTOCOLS_ATOM;
static Atom RECOGNIZE_KEY_ATOM;
static Atom RECOGNIZE_BUTTON_ATOM;
static Atom GPE_INTERACTIVE_HELP_ATOM;

static char* modifier_indicators[8] = {"S","L","C","A","N","M","s","H"};

int control_win_init(control_win_t *control, xstroke_t *xstroke, args_t *args)
{
    int err;
    XRenderColor color_tmp;
    XWMHints wmhints;
    Display *dpy = xstroke->xlp.dpy;
#ifdef USE_PIXMAPS
    XGCValues gv;
#endif
    
    Atom atoms[7], window_type, type_dock;
    static char *atom_names[] = {
	"WM_DELETE_WINDOW", "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_DOCK",
	"RECOGNIZE_PROTOCOLS", "RECOGNIZE_KEY", "RECOGNIZE_BUTTON",
	"_GPE_INTERACTIVE_HELP"
    };
    if (XInternAtoms (dpy, atom_names, 7, False, atoms)) {
	DELETE_WINDOW_ATOM = atoms[0];
	window_type = atoms[1];
	type_dock = atoms[2];
	RECOGNIZE_PROTOCOLS_ATOM = atoms[3];
	RECOGNIZE_KEY_ATOM = atoms[4];
	RECOGNIZE_BUTTON_ATOM = atoms[5];
	GPE_INTERACTIVE_HELP_ATOM = atoms[6];
    } else {
	fprintf(stderr, "%s: An error occurred during XInternAtoms\n", __FUNCTION__);
	return -1;
    }

    control->button = args->button;
    control->hold_time_ms = args->hold_time_ms;
    control->xstroke = xstroke;
    control->mode[0] = '\0';
    control->mod_str[0] = '\0';
    control->msg[0] = '\0';
    control->recognizing = 0;
    control->stroking = 0;
    control->auto_disable = args->auto_disable;
    err = xlp_win_init(&control->win, &xstroke->xlp,
		       args->geometry, ARGS_DEFAULT_GEOMETRY,
		       args->fg, args->bg,
		       args->font,
		       args->name, args->class);
    if (err) {
	fprintf(stderr, "%s: An error occurred during xlp_win_init\n", __FUNCTION__);
	return err;
    }

    err = xlp_win_init_from_window(&control->root_win, control->win.xlp,
				   DefaultRootWindow(dpy),
				   "white", "black");
    if (err) {
	fprintf(stderr, "%s: An error occurred during xlp_win_init_from_window\n", __FUNCTION__);
	return err;
    }

    
#ifdef USE_PIXMAPS
    /* Create pixmaps */
    control->xpm_attr.valuemask = XpmCloseness;
    control->xpm_attr.closeness = 40000;
    
    if (XpmCreatePixmapFromData( dpy, control->win.window,
				 xstroke_inactive_xpm,
				 &control->icon[INACTIVE_ICON],
				 &control->icon_mask[INACTIVE_ICON],
				 &control->xpm_attr) != XpmSuccess) {
	fprintf(stderr, "failed to get inactive icon image\n");
	exit(1);
    }

    if (XpmCreatePixmapFromData(dpy, control->win.window,
				xstroke_active_xpm,
				&control->icon[ACTIVE_ICON],
				&control->icon_mask[ACTIVE_ICON],
				&control->xpm_attr) != XpmSuccess) {
	fprintf(stderr, "failed to get active icon image\n");
	exit(1);
    }

    /* backing mask for shape ext*/
    control->backing_mask = XCreatePixmap(dpy, control->win.window,
					  control->xpm_attr.width,
					  control->xpm_attr.height, 1);

    control->backing_draw = XftDrawCreateBitmap(dpy, control->backing_mask);

    /* GCs for copy and drawing on mask */
    gv.function = GXcopy;
    gv.foreground = BlackPixel(dpy, DefaultScreen(dpy));
    control->gc = XCreateGC(dpy, control->win.window,
			    GCFunction|GCForeground, &gv);
    control->gc_mask = XCreateGC(dpy, control->backing_mask,
				 GCFunction|GCForeground, &gv);

    /* inital set up of shaped window */
    XCopyArea(dpy, control->icon[INACTIVE_ICON], control->win.window,
	      control->gc,
	      0, 0, control->xpm_attr.width, control->xpm_attr.height, 0, 0);

    XShapeCombineMask (dpy, control->win.window, ShapeBounding, 0, 0,
		       control->icon_mask[INACTIVE_ICON], ShapeSet);
#endif
    
    control->cursor = XCreateFontCursor(dpy, XC_pencil);

    color_tmp = control->win.fg.color;
    color_tmp.red >>= 3;
    color_tmp.green >>= 3;
    color_tmp.blue >>= 3;
    color_tmp.alpha >>= 3;
    XftColorAllocValue(dpy,
		       DefaultVisual(dpy, DefaultScreen(dpy)),
		       DefaultColormap(dpy, DefaultScreen(dpy)),
		       &color_tmp, &control->inactive_color);
    control->active_color = control->win.fg.xft_color;
    control->current_color = &control->active_color;
    
    XSetWMProtocols(dpy, control->win.window, &DELETE_WINDOW_ATOM, 1);
    XChangeProperty(dpy, control->win.window, window_type, XA_ATOM, 32,
		    PropModeReplace, (unsigned char *) &type_dock, 1);
    XChangeProperty(dpy, control->win.window,
		    RECOGNIZE_PROTOCOLS_ATOM, XA_ATOM, 32,
		    PropModeReplace,
		    (unsigned char *) &RECOGNIZE_KEY_ATOM, 1);

    XSetCommand(dpy, control->win.window, args->argv, args->argc);

    XChangeProperty(control->win.dpy, control->win.window, GPE_INTERACTIVE_HELP_ATOM,
		    XA_STRING, 8, PropModeReplace, NULL, 0);

#ifdef USE_SYSTEM_TRAY
    tray_init(dpy, control->win.window);
#endif

    wmhints.flags = 0L;
    wmhints.flags |= InputHint; wmhints.input = False;
    XSetWMHints(dpy, control->win.window, &wmhints);

    xlp_win_register_callback(&control->win, ClientMessage,
			      client_message_cb, control);
    xlp_win_register_callback(&control->win, ButtonPress,
			      cwin_button_press_cb, control);
    xlp_win_register_callback(&control->win, MotionNotify,
			      root_motion_notify_cb, control);
    xlp_win_register_callback(&control->win, ButtonRelease,
			      root_button_release_cb, control);
    xlp_win_register_window_callback(&control->win, 
				     DefaultRootWindow(dpy),
				     ButtonPress,
				     root_button_press_cb, control);
    xlp_win_register_window_callback(&control->win,
				     DefaultRootWindow(dpy),
				     MotionNotify,
				     root_motion_notify_cb, control);
    xlp_win_register_window_callback(&control->win,
				     DefaultRootWindow(dpy),
				     ButtonRelease,
				     root_button_release_cb, control);
    xlp_win_register_callback(&control->win, Expose,
			      expose_cb, control);
    
#ifndef USE_SYSTEM_TRAY
    xlp_win_map(&control->win);
#endif

    return 0;
}

void control_win_deinit(control_win_t *control)
{
    xlp_win_unmap(&control->win);
    xlp_win_deinit(&control->win);
}

void control_win_set_mode(control_win_t *control, rec_mode_t *mode)
{
    strncpy(control->mode, mode->name, MAX_MSG);
    control->mode[MAX_MSG] = '\0';
    control_win_update_modifiers(control);
}

static void update_msg(control_win_t *control)
{
    control->msg[0] = '\0';
    strncat(control->msg, control->mod_str, MAX_MSG);
    strncat(control->msg, control->mode, MAX_MSG - strlen(control->msg));
    refresh(control);
}

void control_win_update_modifiers(control_win_t *control)
{
    int i;
    int is_prefixed = 0;
    action_key_data_t *modifiers = control->xstroke->modifiers;

    control->mod_str[0] = '\0';
    for (i=0; i<8; i++) {
	if ((modifiers[i].flags & ACTION_KEY_IS_PRESS)
	    && modifier_indicators[i]) {
	    /* XXX: hack: don't display shift or Caps_Lock, meaning these modifiers
	       are only useful if they always change the mode name too. */
	    if (i==ShiftMapIndex || i==LockMapIndex)
		continue;

/*
	    if (i == ShiftMapIndex) {
		if (modifiers[LockMapIndex].flags & ACTION_KEY_IS_PRESS) continue;
		if (isalpha(control->mode[0])) {
		    control->mode[0] = toupper(control->mode[0]);
		    continue;
		}
	    }
	    if (i == LockMapIndex) {
		if (isalpha(control->mode[0])) {
		    p = control->mode;
		    while (*p) {
			*p = toupper(*p);
			p++;
		    }
		    continue;
		}
	    }
*/
	    strcat(control->mod_str, modifier_indicators[i]);
	    is_prefixed = 1;
	}
    }
    if (is_prefixed)
	strcat(control->mod_str, "-");
    update_msg(control);
}

static void refresh(control_win_t *control)
{
    erase_window(control);
    draw_msg(control); 
}

static void erase_window(control_win_t *control)
{
    XClearWindow(control->win.dpy, control->win.window);
}

static void draw_msg(control_win_t *control)
{
#ifdef USE_PIXMAPS
   /* clear mask */
   XSetForeground(control->win.dpy, control->gc_mask, 0);
   XFillRectangle(control->win.dpy, control->backing_mask,
		  control->gc_mask, 0, 0,
		  control->xpm_attr.width, control->xpm_attr.height);
   XSetForeground(control->win.dpy, control->gc_mask, 1);

   if (!control->recognizing) {
       XCopyArea(control->win.dpy, control->icon_mask[INACTIVE_ICON],
		 control->backing_mask, control->gc_mask,
		 0, 0, control->xpm_attr.width, control->xpm_attr.height,
		 0, 0 );
       
       XCopyArea(control->win.dpy, control->icon[INACTIVE_ICON],
		 control->win.window, control->gc,
		 0, 0, control->xpm_attr.width, control->xpm_attr.height,
		 0, 0);
   } else {
       /* paint to mask for shape*/
       XCopyArea(control->win.dpy, control->icon_mask[ACTIVE_ICON],
		 control->backing_mask, control->gc_mask,
		 0, 0, control->xpm_attr.width, control->xpm_attr.height,
		 0, 0 );

       XftDrawString8(control->backing_draw, control->current_color,
		      control->win.font, 
		      1, 1+control->win.font->ascent,
		      (unsigned char *)control->msg, strlen(control->msg));
       
       /* paint to actual window */
       XCopyArea(control->win.dpy, control->icon[ACTIVE_ICON],
		 control->win.window, control->gc,
		 0, 0, control->xpm_attr.width, control->xpm_attr.height,
		 0, 0);

       XftDrawString8(control->win.draw, control->current_color,
		      control->win.font, 
		      1, 1+control->win.font->ascent,
		      (unsigned char *)control->msg, strlen(control->msg));
       
   }
   XShapeCombineMask(control->win.dpy, control->win.window, ShapeBounding,
		     0, 0, control->backing_mask, ShapeSet);
#else    
   XftDrawString8(control->win.draw, control->current_color,
		  control->win.font,
		  1, 1 + control->win.font->ascent,
		  (unsigned char *) control->msg, strlen(control->msg));
#endif
}

static void control_win_toggle_recognition(control_win_t *control)
{
    if (control->recognizing) {
       ungrab_button(control);
       control->recognizing = 0;
       control->current_color = &control->inactive_color;
       refresh(control);
    } else {
       grab_button(control);
       control->recognizing = 1;
       control->current_color = &control->active_color;
       refresh(control);
    }
}

static void grab_button(control_win_t *control)
{
    XGrabButton(control->win.dpy,
		control->button, AnyModifier,
		DefaultRootWindow(control->win.dpy),
		True, /* owner_events */
		ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
		GrabModeSync, /* synchronous pointer grab */
		GrabModeAsync, /* asynchronous keyboard grab */
		None, /* Don't confine to a window */
		control->cursor);
}

static void ungrab_button(control_win_t *control)
{
    XUngrabButton(control->win.dpy, 
		  control->button, AnyModifier,
		  DefaultRootWindow(control->win.dpy));
}

static void send_interactive_help(control_win_t *control, XClientMessageEvent *cmev)
{
    const char *text;

    if (!control->recognizing)
	text = "This is xstroke. Tap here to enable full-screen gesture recognition.";
    else
	text = "This is xstroke. Tap here to disable full-screen gesture recognition.";

    XChangeProperty(control->win.dpy, cmev->data.l[0], cmev->data.l[3],
		    XA_STRING, 8, PropModeReplace, (unsigned char *)text, strlen(text));
}

/* event callbacks */
static void client_message_cb(XEvent *xev, void *data)
{
    XClientMessageEvent *cmev = &xev->xclient;
    control_win_t *control = (control_win_t *) data;

    if (cmev->data.l[0] == DELETE_WINDOW_ATOM) {
	xstroke_exit(control->xstroke, 0);
    } else if (cmev->message_type == GPE_INTERACTIVE_HELP_ATOM) {
	send_interactive_help(control, cmev);
    }
#ifdef USE_SYSTEM_TRAY 
    else
	tray_handle_client_message(control->win.dpy, 
				   control->win.window, xev);
#endif
}

static void cwin_button_press_cb(XEvent *xev, void *data)
{
    control_win_t *control = (control_win_t *) data;

    control_win_toggle_recognition(control);
}

static void root_button_press_cb(XEvent *xev, void *data)
{
    XButtonEvent *bev = &xev->xbutton;
    control_win_t *control = (control_win_t *) data;
    Display *dpy = control->win.dpy;
    Window deepest;

    deepest = find_deepest_window(dpy,
				  DefaultRootWindow(dpy), DefaultRootWindow(dpy),
				  bev->x_root, bev->y_root);
    if ((control->auto_disable == 0)
	|| window_wants_recognition(dpy, deepest)) { 
	control->stroking = 1;
	control->stroke_motion_beyond_threshold = 0;
	control->stroke_initial_x = bev->x_root;
	control->stroke_initial_y = bev->y_root;
	control->stroke_button = bev->button;
	xlp_win_register_timeout(&control->win, control->hold_time_ms,
				 hold_timeout_cb, control,
				 &control->stroke_start_tv);
	XAllowEvents(dpy, AsyncPointer, CurrentTime);

	xstroke_button_press(control->xstroke, bev);
    } else {
	XAllowEvents(control->win.dpy, ReplayPointer, CurrentTime);
    }
}

static void hold_timeout_cb(struct timeval start_tv, void *data)
{
    control_win_t *control = (control_win_t *) data;

    if (start_tv.tv_sec != control->stroke_start_tv.tv_sec
	|| start_tv.tv_usec != control->stroke_start_tv.tv_usec) {
	/* stale timeout. ignore. */
	return;
    }

    if (control->stroking && !control->stroke_motion_beyond_threshold) {
	backing_restore(&control->xstroke->backing);

	XUngrabPointer(control->win.dpy, CurrentTime);
	ungrab_button(control);
	XTestFakeButtonEvent(control->win.dpy,
	                     control->stroke_button, False, CurrentTime);
	XTestFakeButtonEvent(control->win.dpy,
	                     control->stroke_button, True, CurrentTime);
	grab_button(control);
	XSync(control->win.dpy, False);
	XUngrabServer(control->win.dpy);

	control->stroking = 0;
    }
}

static void root_motion_notify_cb(XEvent *xev, void *data)
{
    XMotionEvent *mev = &xev->xmotion;
    control_win_t *control = (control_win_t *) data;
    if (control->stroking) {
	if (! control->stroke_motion_beyond_threshold) {
	    int drift = (abs(mev->x_root - control->stroke_initial_x)
			 + abs(mev->y_root - control->stroke_initial_y));
	    if (drift > HOLD_DRIFT_THRESHOLD) {
		control->stroke_motion_beyond_threshold = 1;
	    }
	}
	xstroke_motion_notify(control->xstroke, mev);
    }
}

static void root_button_release_cb(XEvent *xev, void *data)
{
#define BUTTONS_MASK (Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask)
    XButtonEvent *bev = &xev->xbutton;
    control_win_t *control = (control_win_t *) data;

    if (control->stroking) {
	/* Were all buttons released? */
	if ((bev->state & BUTTONS_MASK & ~mask_for_button(bev->button)) == 0) {
	    ungrab_button(control);
	    xstroke_button_release(control->xstroke, bev);
	    grab_button(control);
	    control->stroking = 0;
	}
    }
}

static int mask_for_button(int button)
{
    switch(button) {
    case 1:
	return Button1Mask;
    case 2:
	return Button2Mask;
    case 3:
	return Button3Mask;
    case 4:
	return Button4Mask;
    case 5:
	return Button5Mask;
    default:
	fprintf(stderr, "%s: ERROR: Unknown button: %d\n", __FUNCTION__, button);
	return -1;
    }
}

static void expose_cb(XEvent *xev, void *data)
{
/*    XExposeEvent *eev = &xev->xexpose; */
    control_win_t *control = (control_win_t *) data;

    refresh(control);
}

/*
static void find_some_windows(Display *dpy, Window grandfather, Window parent,
			      int x, int y,
			      Window *deepest, Window *deepest_hinted)
{
    int dest_x, dest_y;
    Window child;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    Atom *prop;

    XTranslateCoordinates(dpy, grandfather, parent, x, y,
			  &dest_x, &dest_y, &child);

    if (child == None) {
	*deepest = parent;
	*deepest_hinted = None;
	return;
    } else {
	find_some_windows(dpy, parent, child, dest_x, dest_y,
			  deepest, deepest_hinted);
	if (*deepest_hinted == None) {
	    XGetWindowProperty(dpy, child, XSTROKE_RECOGNITION_ATOM,
			       0L, 1L, False,
			       XA_ATOM,
			       &actual_type, &actual_format,
			       &nitems, &bytes_after, (unsigned char **) &prop);
	    if (actual_type != None) {
		*deepest_hinted = child;
		XFree(prop);
	    }
	}
    }
}
*/

static Window find_deepest_window(Display *dpy, Window grandfather, Window parent,
				  int x, int y)
{
    int dest_x, dest_y;
    Window child;

    XTranslateCoordinates(dpy, grandfather, parent, x, y,
			  &dest_x, &dest_y, &child);

    if (child == None) {
	return parent;
    } else {
	return find_deepest_window(dpy, parent, child, dest_x, dest_y);
    }
}

static int window_wants_recognition(Display *dpy, Window window)
{
    XWindowAttributes attr;
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    Atom *prop;

    if (
	XGetWindowProperty(dpy, window, RECOGNIZE_PROTOCOLS_ATOM,
			   0L, 10000L, False,
			   XA_ATOM,
			   &actual_type, &actual_format,
			   &nitems, &bytes_after, (unsigned char **) &prop)
	== Success && actual_type == XA_ATOM) {
	int i;
	int desired_protocol_supported = 0;

	for (i=0; i < nitems; i++) {
	    if (prop[i] == RECOGNIZE_KEY_ATOM
		|| prop[i] == RECOGNIZE_BUTTON_ATOM
		) {
		desired_protocol_supported = 1;
	    }
	}

	XFree(prop);
	return desired_protocol_supported;
    }

    /* Hmm... no RECOGNIZE_PROTOCOLS property, use event_mask as a last resort */
    if (XGetWindowAttributes(dpy, window, &attr)) {
	if (attr.all_event_masks & (KeyPressMask | KeyReleaseMask)) {
	    return 1;
	} else if (attr.all_event_masks & PointerMotionMask) {
	    return 0;
	}
    }
    return 1;
}

/*
static Window find_deepest_key_listener_window(Display *dpy,
					       Window grandfather,
					       Window parent,
					       int x, int y)
{
    int dest_x, dest_y;
    XWindowAttributes attr;
    Window child;

    XTranslateCoordinates(dpy, grandfather, parent, x, y,
			  &dest_x, &dest_y, &child);

    if (child == None) {
	return None;
    } else {
	child = find_deepest_key_listener_window(dpy, parent, child,
						 dest_x, dest_y);
	if (child == None) {
	    XGetWindowAttributes(dpy, parent, &attr);
	    if (attr.all_event_masks & (KeyPressMask | KeyReleaseMask)) {
		return parent;
	    }
	}
	return child;
    }
}
*/
