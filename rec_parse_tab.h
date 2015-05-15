/* A Bison parser, made by GNU Bison 1.875a.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UNKNOWN_CHARACTER = 258,
     KEY = 259,
     BUTTON = 260,
     MODE_SHIFT = 261,
     MODE_LOCK = 262,
     EXEC = 263,
     ORIENTATION_CORRECTION = 264,
     MODE = 265,
     INTEGER = 266,
     IDENTIFIER = 267,
     STRING = 268,
     DOUBLE = 269,
     ENGINE = 270,
     OPTION = 271
   };
#endif
#define UNKNOWN_CHARACTER 258
#define KEY 259
#define BUTTON 260
#define MODE_SHIFT 261
#define MODE_LOCK 262
#define EXEC 263
#define ORIENTATION_CORRECTION 264
#define MODE 265
#define INTEGER 266
#define IDENTIFIER 267
#define STRING 268
#define DOUBLE 269
#define ENGINE 270
#define OPTION 271




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 51 "rec_parse.y"
typedef union YYSTYPE {
  int    ival;
  char  *sval;
  double dval;
  rec_mode_t *rec_mode;
  rec_mode_list_t rec_mode_list;
  gesture_list_t gesture_list;
  gesture_t gesture;
  action_t action;
  action_item_t action_item;
  feature_list_t feature_list;
  feature_t feature;
  rec_engine_t *rec_engine;
  option_t option;
  option_list_t option_list;
} YYSTYPE;
/* Line 1240 of yacc.c.  */
#line 86 "y.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





