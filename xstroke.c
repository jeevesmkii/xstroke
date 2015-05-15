/* xstroke.c - full-screen gesture recognition for X

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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <X11/X.h>
/* XXX: doesn't seem like this define is the right thing to do... */
#define XK_MISCELLANY
#include <X11/keysymdef.h>
#include <X11/extensions/XTest.h>

#include <math.h>

#include "xstroke.h"
#include "rec.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int xstroke_init(xstroke_t * xstroke, args_t *args);
static void xstroke_deinit(xstroke_t *xstroke);
static void root_configure_notify_cb(XEvent *xev, void *data);
static void key_action_cb(void *action_data, void *data);
static int set_keycode_and_flags(Display *dpy, action_key_data_t *key_data);
static void press_modifiers(xstroke_t *xstroke, int must_press_shift);
static void release_modifiers(xstroke_t *xstroke, int must_release_shift);
static void button_action_cb(void *action_data, void *data);
static void mode_action_cb(void *action_data, void *data);
static void exec_action_cb(void *action_data, void *data);

static void initialize_modifiers(xstroke_t *xstroke);
static void register_xlp_callbacks(xstroke_t *xstroke);
static void register_rec_callbacks(xstroke_t *xstroke);
static void register_signal_handlers(xstroke_t *xstroke);
static void reap_children(int signum);
static int keysym_to_keycode_and_column(Display *dpy, KeySym keysym,
					KeyCode *code_ret, int *col_ret);
static int find_modifier_index(xstroke_t *xstroke, KeyCode keycode);

static void draw_stroke_box(xstroke_t *xstroke);
#ifdef XSTROKE_DEBUG_ROTATION
static void draw_stroke(xstroke_t *xstroke, stroke_t *stroke);
#endif

static args_t args;

int main(int argc, char *argv[])
{
    int optind;
    int err;
    xstroke_t xstroke;

    args_init(&args);
    optind = args_parse(&args, argc, argv);
    if (optind != argc) {
	if (optind > 0) {
	    fprintf(stderr, "%s: Extraneous options:", argv[0]);
	    while (optind < argc) {
		fprintf(stderr, " %s", argv[optind++]);
	    }
	    fprintf(stderr, "\n");
	}
	args_usage(argv);
	exit(1);
    }

    err = xstroke_init(&xstroke, &args);
    if (err) {
	fprintf(stderr, "%s: An error occurred during xstroke_init\n", __FUNCTION__);
	return err;
    }

    if (args.verify) {
	rec_verify(&xstroke.rec, args.verify);
    }

    err = xlp_main_loop_start(&xstroke.xlp);

    xstroke_deinit(&xstroke);
    args_deinit(&args);

    return err;
}

static int xstroke_init(xstroke_t * xstroke, args_t *args)
{
    int err;
    Display *dpy;
    struct stat stat_buf;
    int scr;

    err = xlp_init(&xstroke->xlp, args->display);
    if (err) {
	fprintf(stderr, "%s: An error occurred during xlp_init\n", __FUNCTION__);
	return err;
    }
    dpy = xstroke->xlp.dpy;

#ifdef XSTROKE_SYNCHRONOUS
    XSynchronize(dpy, True);
#endif
#ifdef XSTROKE_DEBUG
    fprintf(stderr, "WARNING: xstroke compiled with XSTROKE_DEBUG defined.\n\tThis may have a negative impact on performance.\n\tOh no!! (Emphasis by Stacy)\n");
#endif

    scr = DefaultScreen(dpy);
    xstroke->modifier_map = XGetModifierMapping(dpy);

    err = backing_init(&xstroke->backing, dpy, DefaultRootWindow(dpy),
		       DisplayWidth(dpy, scr), DisplayHeight(dpy, scr),
		       DefaultDepth(dpy, scr));
    if (err) {
	fprintf(stderr, "%s: An error occurred during backing_init\n", __FUNCTION__);
	return err;
    }

    err = control_win_init(&xstroke->control_win, xstroke, args);
    if (err) {
	fprintf(stderr, "%s: An error occurred during control_win_init\n", __FUNCTION__);
	return err;
    }

    err = brush_init(&xstroke->brush, &xstroke->backing);
    if (err) {
	fprintf(stderr, "%s: An error occurred during brush_init\n", __FUNCTION__);
	return err;
    }

    err = stat(args->alphabet, &stat_buf);
    if (err) {
	if (errno != ENOENT) {
	    fprintf(stderr, "%s: Warning: failed to stat %s: %s. Falling back to %s\n",
		    __FUNCTION__, args->alphabet, strerror(errno),
		    ARGS_FALLBACK_ALPHABET);
	}
	free(args->alphabet);
	args->alphabet = strdup(ARGS_FALLBACK_ALPHABET);
    }
    err = rec_init(&xstroke->rec, args,
		   DisplayWidth(dpy, scr), DisplayHeight(dpy, scr));
    if (err) {
	fprintf(stderr, "%s: An error occurred during rec_init\n", __FUNCTION__);
	return err;
    }

    initialize_modifiers(xstroke);
    register_xlp_callbacks(xstroke);
    register_rec_callbacks(xstroke);
    register_signal_handlers(xstroke);

    control_win_set_mode(&xstroke->control_win, xstroke->rec.current_mode);

    return 0;
}

static void xstroke_deinit(xstroke_t *xstroke)
{
    rec_deinit(&xstroke->rec);
    brush_deinit(&xstroke->brush);
    control_win_deinit(&xstroke->control_win);
    backing_deinit(&xstroke->backing);
    XFreeModifiermap(xstroke->modifier_map);
    xlp_deinit(&xstroke->xlp);
}

void xstroke_reload(xstroke_t *xstroke)
{
    int err;
    Display *dpy = xstroke->xlp.dpy;
    int scr = DefaultScreen(dpy);

    rec_deinit(&xstroke->rec);
    err = rec_init(&xstroke->rec, &args,
		   DisplayWidth(dpy, scr), DisplayHeight(dpy, scr));
    if (err) {
	fprintf(stderr, "%s: An error occurred during rec_init\n",
		__FUNCTION__);
	xstroke_exit(xstroke, 1);
    }
}

void xstroke_exit(xstroke_t *xstroke, int exit_code)
{
    xlp_main_loop_stop(&xstroke->xlp, exit_code);
}

void xstroke_button_press(xstroke_t *xstroke, XButtonEvent *bev)
{
    if (args.nograb == 0) {
	XGrabServer(xstroke->xlp.dpy);
    }
    backing_save(&xstroke->backing,
		 bev->x_root - xstroke->brush.image_width,
		 bev->y_root - xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 bev->x_root + xstroke->brush.image_width,
		 bev->y_root + xstroke->brush.image_height);
    if (args.rotation
	&& fabs(xstroke->rec.history.orientation) > ARGS_DEFAULT_ROTATION_LIMIT) {
	draw_stroke_box(xstroke);
    }
    brush_draw(&xstroke->brush, bev->x_root, bev->y_root);
    
    rec_begin_stroke(&xstroke->rec, bev->x_root, bev->y_root, bev->time);
}

void xstroke_motion_notify(xstroke_t *xstroke, XMotionEvent *mev)
{
    backing_save(&xstroke->backing,
		 mev->x_root - xstroke->brush.image_width,
		 mev->y_root - xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 mev->x_root + xstroke->brush.image_width,
		 mev->y_root + xstroke->brush.image_height);
    brush_line_to(&xstroke->brush, mev->x_root, mev->y_root);
    
    rec_extend_stroke(&xstroke->rec, mev->x_root, mev->y_root, mev->time);
}

void xstroke_button_release(xstroke_t *xstroke, XButtonEvent *bev)
{
    backing_restore(&xstroke->backing);
    rec_end_stroke(&xstroke->rec, bev->x_root, bev->y_root, bev->time);
    if (args.nograb == 0) {
	/* Force the restoration of root to complete before ungrabbing */
	XSync(xstroke->xlp.dpy, False);
	XUngrabServer(xstroke->xlp.dpy);
    }
    
#ifdef XSTROKE_DEBUG_ROTATION
    if (args.rotation) {
	draw_stroke(xstroke, &xstroke->rec.stroke);
    }
#endif
}

static void root_configure_notify_cb(XEvent *xev, void *data)
{
    XConfigureEvent *cev = &xev->xconfigure;
    xstroke_t *xstroke = (xstroke_t *) data;
    Display *dpy = xstroke->xlp.dpy;
    int scr = DefaultScreen(dpy);

    backing_reconfigure(&xstroke->backing, cev->width, cev->height,
			DefaultDepth(dpy, scr));
    rec_set_size(&xstroke->rec, cev->width, cev->height);
}

static void key_action_cb(void *action_data, void *data)
{
    int err;

    action_key_data_t *key_data = (action_key_data_t *) action_data;
    xstroke_t *xstroke = (xstroke_t *) data;
    Display *dpy = xstroke->xlp.dpy;

    if (key_data->keycode == 0) {
	err = set_keycode_and_flags(dpy, key_data);
	if (err) {
	    fprintf(stderr, "%s: ERROR: not sending keystroke\n", __FUNCTION__);
	    return;
	}
    }

    if (key_data->flags & ACTION_KEY_IS_MODIFIER
	&& key_data->flags & ACTION_KEY_IS_PRESS) {

	int modifier_index = find_modifier_index(xstroke, key_data->keycode);
	action_key_data_t *mod = &xstroke->modifiers[modifier_index];

	mod->flags ^= ACTION_KEY_IS_PRESS;

	control_win_update_modifiers(&xstroke->control_win);
    } else {
	int must_have_shift = key_data->flags & ACTION_KEY_REQUIRES_SHIFT;

	press_modifiers(xstroke, must_have_shift);
	XTestFakeKeyEvent(dpy, key_data->keycode, True, CurrentTime);
	XTestFakeKeyEvent(dpy, key_data->keycode, False, CurrentTime);
	release_modifiers(xstroke, must_have_shift);

	control_win_update_modifiers(&xstroke->control_win);
    }
}

static int set_keycode_and_flags(Display *dpy, action_key_data_t *key_data)
{
    int col;
    int err;

    err = (keysym_to_keycode_and_column(dpy, key_data->keysym,
					&key_data->keycode, &col));
    if (err) {
	fprintf(stderr, "%s: No keycode found for keysym %ld\n",
		__FUNCTION__, key_data->keysym);
	return EINVAL;
    } else {
	if (col & 1) {
	    key_data->flags |= ACTION_KEY_REQUIRES_SHIFT;
	} else {
	    key_data->flags &= ~ACTION_KEY_REQUIRES_SHIFT;
	}
    }

    return 0;
}

static void press_modifiers(xstroke_t *xstroke, int must_press_shift)
{
    int i;
    Display *dpy = xstroke->xlp.dpy;

    for (i=0; i < 8; i++) {
	if (xstroke->modifiers[i].flags & ACTION_KEY_IS_PRESS
	    || (i == ShiftMapIndex && must_press_shift)) {
	    XTestFakeKeyEvent(dpy, xstroke->modifiers[i].keycode,
			      True, CurrentTime);
	    /* XXX: The server doesn't really expect a release of
	       Caps_Lock here, (but with xkb it wants one). So,
	       with an xkb server we actually need the following
	       release of Caps_Lock
	       if (i == LockMapIndex) {
	       XTestFakeKeyEvent(dpy, xstroke->modifiers[i].keycode,
	       False, CurrentTime);
	       }
	    */
	}
    }
}

static void release_modifiers(xstroke_t *xstroke, int must_release_shift)
{
    int i;
    Display *dpy = xstroke->xlp.dpy;
    
    for (i=0; i < 8; i++) {
	if (xstroke->modifiers[i].flags & ACTION_KEY_IS_PRESS
	    || (i == ShiftMapIndex && must_release_shift)) {
	    /* XXX: The server doesn't really expect a press of
	       Caps_Lock here, (but with xkb it wants one). So,
	       with an xkb server we actually need the following
	       release of Caps_Lock
	       if (i == LockMapIndex) {
	       XTestFakeKeyEvent(dpy, xstroke->modifiers[i].keycode,
	       True, CurrentTime);
	       }
	    */
	    XTestFakeKeyEvent(dpy, xstroke->modifiers[i].keycode,
			      False, CurrentTime);
	}
	if (i != LockMapIndex)
	    xstroke->modifiers[i].flags &= ~ACTION_KEY_IS_PRESS;
    }
}

static void button_action_cb(void *action_data, void *data)
{
    action_button_data_t *button_data = (action_button_data_t *) action_data;
    xstroke_t *xstroke = (xstroke_t *) data;
    Display *dpy = xstroke->xlp.dpy;
    XTestFakeButtonEvent(dpy, button_data->button, True, CurrentTime);
    XTestFakeButtonEvent(dpy, button_data->button, False, CurrentTime);
}

static void mode_action_cb(void *action_data, void *data)
{
    action_mode_data_t *mode_data = (action_mode_data_t *) action_data;
    xstroke_t *xstroke = (xstroke_t *) data;
    int i;

    /* Reset all modifiers on any mode change */
    for (i=0; i < 8; i++) {
	xstroke->modifiers[i].flags &= ~ACTION_KEY_IS_PRESS;
    }
    control_win_set_mode(&xstroke->control_win, mode_data->mode);
}

static void exec_action_cb(void *action_data, void *data)
{
    pid_t pid;
    action_exec_data_t *exec_data = (action_exec_data_t *) action_data;

    pid = fork();
    if (pid == -1) {
	fprintf(stderr, "%s: error forking before executing `%s': %s\n",
		__FUNCTION__, exec_data->exec, strerror(errno));
	return;
    }

    if (pid == 0) {
	/* XXX: Rewrite this to not call /bin/sh? (to avoid extra
           shell invocations) */
	execlp("/bin/sh", "sh", "-c", exec_data->exec, (char *) NULL);
	fprintf(stderr, "%s: ERROR: failed to exec `%s': %s\n",
		__FUNCTION__, exec_data->exec, strerror(errno));
	exit(1);
    }
}

static int keysym_to_keycode_and_column(Display *dpy, KeySym keysym,
					KeyCode *code_ret, int *col_ret)
{
    int col;
    int keycode;
    KeySym k;
    int min_kc, max_kc;
    
    XDisplayKeycodes(dpy, &min_kc, &max_kc);
    
    for (keycode = min_kc; keycode <= max_kc; keycode++) {
	col = 0;
	while (1) {
	    k = XKeycodeToKeysym (dpy, keycode, col);
	    if (k == NoSymbol)
		break;
	    if (k == keysym) {
		*code_ret = keycode;
		if (col_ret)
		    *col_ret = col;
		return 0;
	    }
	    col++;
	}
    }
    return 1;
}

static int find_modifier_index(xstroke_t *xstroke, KeyCode keycode)
{
    XModifierKeymap *map = xstroke->modifier_map;
    int key;
    int index;
    KeyCode kc;
    
    for (index=0; index<8; index++) {
	for (key=0; key < map->max_keypermod; key++) {
	    kc = map->modifiermap[index * map->max_keypermod + key]; 
	    if (kc == keycode) {
		return index;
	    }
	}
    }
    return -1;
}

static void initialize_modifiers(xstroke_t *xstroke)
{
    XModifierKeymap *map;
    int index;
    KeyCode keycode;
    KeySym keysym;
    Display *dpy = xstroke->xlp.dpy;

    map = XGetModifierMapping(dpy);
    for (index=0; index<8; index++) {
	keycode = map->modifiermap[index * map->max_keypermod];
	keysym = XKeycodeToKeysym(dpy, keycode, 0);
	action_keysym_data_init(&xstroke->modifiers[index], keysym, 0);
	xstroke->modifiers[index].keycode = keycode;
    }
    XFreeModifiermap(map);
}

static void register_xlp_callbacks(xstroke_t *xstroke)
{
    xlp_win_register_callback(&xstroke->control_win.root_win, ConfigureNotify,
			      root_configure_notify_cb, xstroke);
}

static void register_rec_callbacks(xstroke_t *xstroke)
{
    rec_register_callback(&xstroke->rec, ACTION_KEY,
			  key_action_cb, xstroke);
    rec_register_callback(&xstroke->rec, ACTION_BUTTON,
			  button_action_cb, xstroke);
    rec_register_callback(&xstroke->rec, ACTION_MODE,
			  mode_action_cb, xstroke);
    rec_register_callback(&xstroke->rec, ACTION_EXEC,
			  exec_action_cb, xstroke);
}

static void register_signal_handlers(xstroke_t *xstroke)
{
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = reap_children;
    action.sa_flags = SA_NOCLDSTOP|SA_RESTART;
    sigaction(SIGCHLD, &action, NULL);
}

static void reap_children(int signum)
{
    pid_t pid;

    do {
	pid = waitpid(-1, NULL, WNOHANG);
    } while (pid > 0);
}

static void draw_stroke_box(xstroke_t *xstroke)
{
    int x = xstroke->rec.history.x;
    int y = xstroke->rec.history.y;
    int width = xstroke->rec.history.width * 1.2;
    int height = xstroke->rec.history.height * 1.2;
    double theta = xstroke->rec.history.orientation;
    double phi = theta + M_PI_2;
    double xm, ym;
    double rad_cos_theta = (width / 2.0) * cos(theta);
    double rad_sin_theta = (width / 2.0) * sin(theta);
    int x1, y1, x2, y2;

    xm = x + (height / 2.0) * cos(phi);
    ym = y + (height / 2.0) * sin(phi);

    x1 = xm - rad_cos_theta;
    y1 = ym - rad_sin_theta;
    x2 = xm + rad_cos_theta;
    y2 = ym + rad_sin_theta;

    backing_save(&xstroke->backing,
		 x1 - xstroke->brush.image_width,
		 y1 - xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 x1 + xstroke->brush.image_width,
		 y1 + xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 x2 - xstroke->brush.image_width,
		 y2 - xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 x2 + xstroke->brush.image_width,
		 y2 + xstroke->brush.image_height);
    brush_draw(&xstroke->brush, x1, y1);
    brush_line_to(&xstroke->brush, x2, y2);
}

#ifdef XSTROKE_DEBUG_ROTATION
static void draw_stroke(xstroke_t *xstroke, stroke_t *stroke)
{
    int i;
    int x, y;

    fprintf(stderr, "%s: Drawing rotated stroke\n", __FUNCTION__);

    x = stroke->pts[0].x;
    y = stroke->pts[0].y;
    backing_save(&xstroke->backing,
		 x - xstroke->brush.image_width,
		 y - xstroke->brush.image_height);
    backing_save(&xstroke->backing,
		 x + xstroke->brush.image_width,
		 y + xstroke->brush.image_height);
    brush_draw(&xstroke->brush, x, y);

    for (i=1; i < stroke->num_pts; i++) {
	x = stroke->pts[i].x;
	y = stroke->pts[i].y;
	backing_save(&xstroke->backing,
		     x - xstroke->brush.image_width,
		     y - xstroke->brush.image_height);
	backing_save(&xstroke->backing,
		     x + xstroke->brush.image_width,
		     y + xstroke->brush.image_height);
	brush_line_to(&xstroke->brush, x, y);
    }
}
#endif
