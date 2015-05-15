bin_PROGRAMS = xstroke

xstroke_SOURCES = \
	action.c action.h \
	action_item.c action_item.h \
	anchor_engine.c anchor_engine.h \
	args.c args.h \
	backing.c backing.h \
	bresenham.c bresenham.h \
	brush.c brush.h	brush-image.h brush-shadow.h \
	control_win.c control_win.h \
	dir_engine.c dir_engine.h \
	feature.c feature.h \
	fixed.c fixed.h \
	gesture.c gesture.h \
	grid.c grid.h \
	grid_engine.c grid_engine.h \
	log.c log.h \
	matrix.c matrix.h \
	option.c option.h \
	raw_engine.c raw_engine.h \
	rec.c rec.h \
	rec_callback.c rec_callback.h \
	rec_engine.c rec_engine.h \
	rec_history.c rec_history.h \
	rec_lex.l rec_lex.h \
	rec_mode.c rec_mode.h \
	rec_parse.y rec_parse.h \
	regex_feature.c regex_feature.h \
	sprintf_alloc.c sprintf_alloc.h \
	stroke.c stroke.h \
	tray.c tray.h \
	xlp.c xlp.h \
	xlp_callback.c xlp_callback.h \
	xlp_color.c xlp_color.h \
	xlp_timeout.c xlp_timeout.h \
	xlp_win.c xlp_win.h \
	xstroke.c xstroke.h

BUILT_SOURCES = rec_parse.h rec_lex.h rec_lex.c
AM_YFLAGS = -d

xstroke_LDADD = @XSTROKE_LIBS@ -lm -lpthread $(X_LIBS) -lXpm -lXtst
AM_CFLAGS = @XSTROKE_CFLAGS@ -DXSTROKE_CONF_DIR='"$(sysconfdir)/xstroke"' $(X_CFLAGS)

EXTRA_DIST = xstroke_active.xpm xstroke_inactive.xpm etc/alphabet

configdir=$(sysconfdir)/xstroke
config_DATA=etc/alphabet
