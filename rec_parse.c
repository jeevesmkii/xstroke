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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "rec_parse.y"

/* rec_parse.y -- Grammar for xstroke recognition control file

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
#include <errno.h>

#include "rec.h"
#include "rec_mode.h"
#include "gesture.h"
#include "action.h"
#include "feature.h"
#include "option.h"

#include "rec_lex.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int yyerror(char *err);

#define YYPARSE_PARAM rec
#define YYERROR_VERBOSE

#ifdef REC_PARSE_DEBUG
#define YYDEBUG 1
#endif

static feature_list_t FEATURE_ERROR = { -1, NULL };
#define IS_FEATURE_ERROR(f) ( (f).num_features == FEATURE_ERROR.num_features )



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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
/* Line 191 of yacc.c.  */
#line 173 "y.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 68 "rec_parse.y"

int yylex(YYSTYPE *lvalp);


/* Line 214 of yacc.c.  */
#line 188 "y.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   87

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  21
/* YYNRULES -- Number of rules. */
#define YYNRULES  44
/* YYNRULES -- Number of states. */
#define YYNSTATES  74

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   271

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      22,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      24,    25,     2,     2,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    19,    23,
       2,    21,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    17,     2,    18,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    15,    17,
      21,    26,    29,    30,    36,    38,    42,    44,    46,    49,
      53,    55,    59,    62,    65,    68,    71,    74,    77,    80,
      83,    84,    88,    89,    93,    97,   101,   103,   106,   109,
     114,   119,   125,   127,   130
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      27,     0,    -1,    -1,    28,    -1,    29,    -1,    28,    29,
      -1,    30,    -1,    36,    -1,    44,    -1,    31,    17,    18,
      -1,    31,    17,    35,    18,    -1,    10,    13,    -1,    -1,
      10,    13,    32,    19,    33,    -1,    34,    -1,    33,    20,
      34,    -1,    13,    -1,    36,    -1,    35,    36,    -1,    37,
      21,    39,    -1,    38,    -1,    37,    20,    38,    -1,     4,
      12,    -1,     4,    11,    -1,     5,    11,    -1,     6,    13,
      -1,     7,    13,    -1,     8,    13,    -1,     9,    11,    -1,
       9,    14,    -1,    -1,    43,    40,    22,    -1,    -1,     1,
      41,    22,    -1,    17,    42,    18,    -1,    17,     1,    18,
      -1,    43,    -1,    42,    23,    -1,    42,    43,    -1,    12,
      24,    13,    25,    -1,    15,    12,    17,    18,    -1,    15,
      12,    17,    45,    18,    -1,    46,    -1,    45,    46,    -1,
      16,    13,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   108,   108,   109,   112,   113,   116,   118,   123,   127,
     129,   136,   142,   141,   155,   160,   167,   174,   179,   186,
     201,   206,   213,   218,   225,   229,   236,   243,   248,   254,
     263,   262,   271,   270,   278,   280,   284,   289,   293,   300,
     308,   313,   329,   334,   341
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "UNKNOWN_CHARACTER", "KEY", "BUTTON", 
  "MODE_SHIFT", "MODE_LOCK", "EXEC", "ORIENTATION_CORRECTION", "MODE", 
  "INTEGER", "IDENTIFIER", "STRING", "DOUBLE", "ENGINE", "OPTION", "'{'", 
  "'}'", "':'", "','", "'='", "'\\n'", "';'", "'('", "')'", "$accept", 
  "alphabet", "definition_list", "definition", "mode", "mode_decl", "@1", 
  "mode_id_list", "mode_id", "gesture_list", "gesture", "action", 
  "action_item", "feature_group", "@2", "@3", "feature_list", "feature", 
  "engine", "option_list", "option", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   123,   125,    58,
      44,    61,    10,    59,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    26,    27,    27,    28,    28,    29,    29,    29,    30,
      30,    31,    32,    31,    33,    33,    34,    35,    35,    36,
      37,    37,    38,    38,    38,    38,    38,    38,    38,    38,
      40,    39,    41,    39,    39,    39,    42,    42,    42,    43,
      44,    44,    45,    45,    46
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     1,     1,     3,
       4,     2,     0,     5,     1,     3,     1,     1,     2,     3,
       1,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       0,     3,     0,     3,     3,     3,     1,     2,     2,     4,
       4,     5,     1,     2,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     4,     6,     0,     7,     0,    20,     8,    23,    22,
      24,    25,    26,    27,    28,    29,    11,     0,     1,     5,
       0,     0,     0,     0,     0,     9,     0,    17,    21,    32,
       0,     0,    19,    30,     0,     0,    40,     0,    42,    10,
      18,     0,     0,     0,     0,    36,     0,    16,    13,    14,
       0,    41,    43,    33,     0,    35,    34,    37,    38,    31,
       0,    44,    39,    15
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     9,    10,    11,    12,    13,    33,    58,    59,    36,
      14,    15,    16,    42,    56,    51,    54,    43,    17,    47,
      48
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -32
static const yysigned_char yypact[] =
{
      20,    30,     3,    -5,    24,    38,    -2,    42,    44,    54,
      20,   -32,   -32,    40,   -32,    32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,    39,    43,   -32,   -32,
      -3,    41,    -1,    45,    18,   -32,    13,   -32,   -32,   -32,
      35,    31,   -32,   -32,    48,    49,   -32,    22,   -32,   -32,
     -32,    46,    50,    47,    21,   -32,    51,   -32,    52,   -32,
      53,   -32,   -32,   -32,    55,   -32,   -32,   -32,   -32,   -32,
      48,   -32,   -32,   -32
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -32,   -32,   -32,    57,   -32,   -32,   -32,   -32,     0,   -32,
     -23,   -32,    56,   -32,   -32,   -32,   -32,   -31,   -32,   -32,
      27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -13
static const yysigned_char yytable[] =
{
      39,     1,     2,     3,     4,     5,     6,    37,    21,    24,
      55,    40,    25,    50,    20,    35,    41,     1,     2,     3,
       4,     5,     6,    68,     1,     2,     3,     4,     5,     6,
       7,    49,    53,    40,    45,     8,    46,    22,    45,    66,
      61,    18,    19,    40,    67,     1,     2,     3,     4,     5,
       6,    23,    31,    32,    28,    26,    27,    30,   -12,    52,
      34,    57,    60,    64,    44,    65,    71,    29,    63,     0,
      73,     0,    70,    69,    62,     0,     0,     0,     0,     0,
      72,     0,     0,     0,     0,     0,     0,    38
};

static const yysigned_char yycheck[] =
{
       1,     4,     5,     6,     7,     8,     9,    30,    13,    11,
      41,    12,    14,    36,    11,    18,    17,     4,     5,     6,
       7,     8,     9,    54,     4,     5,     6,     7,     8,     9,
      10,    18,     1,    12,    16,    15,    18,    13,    16,    18,
      18,    11,    12,    12,    23,     4,     5,     6,     7,     8,
       9,    13,    20,    21,     0,    13,    12,    17,    19,    24,
      17,    13,    13,    13,    19,    18,    13,    10,    22,    -1,
      70,    -1,    20,    22,    47,    -1,    -1,    -1,    -1,    -1,
      25,    -1,    -1,    -1,    -1,    -1,    -1,    31
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     4,     5,     6,     7,     8,     9,    10,    15,    27,
      28,    29,    30,    31,    36,    37,    38,    44,    11,    12,
      11,    13,    13,    13,    11,    14,    13,    12,     0,    29,
      17,    20,    21,    32,    17,    18,    35,    36,    38,     1,
      12,    17,    39,    43,    19,    16,    18,    45,    46,    18,
      36,    41,    24,     1,    42,    43,    40,    13,    33,    34,
      13,    18,    46,    22,    13,    18,    18,    23,    43,    22,
      20,    13,    25,    34
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 108 "rec_parse.y"
    { yyval.ival = 0; }
    break;

  case 6:
#line 117 "rec_parse.y"
    { yyval.ival = 0; }
    break;

  case 7:
#line 119 "rec_parse.y"
    {
			  rec_mode_add_gesture(((rec_t *) rec)->global_mode,
					       &yyvsp[0].gesture);
			}
    break;

  case 8:
#line 124 "rec_parse.y"
    { yyval.ival = 0; }
    break;

  case 9:
#line 128 "rec_parse.y"
    { yyval.rec_mode = yyvsp[-2].rec_mode; }
    break;

  case 10:
#line 130 "rec_parse.y"
    {
			  yyval.rec_mode = yyvsp[-3].rec_mode;
			  rec_mode_add_gestures(yyval.rec_mode, &yyvsp[-1].gesture_list);
			}
    break;

  case 11:
#line 137 "rec_parse.y"
    {
			  yyval.rec_mode = rec_get_mode((rec_t *) rec, yyvsp[0].sval);
			  free(yyvsp[0].sval);
			}
    break;

  case 12:
#line 142 "rec_parse.y"
    {
			  /* Do this first so the default mode gets set correctly*/
			  yyval.rec_mode = rec_get_mode((rec_t *) rec, yyvsp[0].sval);
			}
    break;

  case 13:
#line 147 "rec_parse.y"
    {
			  yyval.rec_mode = rec_get_mode((rec_t *) rec, yyvsp[-3].sval);
			  rec_mode_add_parents(yyval.rec_mode, &yyvsp[0].rec_mode_list);
			  rec_mode_list_deinit_shallow(&yyvsp[0].rec_mode_list);
			  free(yyvsp[-3].sval);
			}
    break;

  case 14:
#line 156 "rec_parse.y"
    {
			  rec_mode_list_init(&yyval.rec_mode_list);
			  rec_mode_list_append(&yyval.rec_mode_list, yyvsp[0].rec_mode);
			}
    break;

  case 15:
#line 161 "rec_parse.y"
    {
			  yyval.rec_mode_list = yyvsp[-2].rec_mode_list;
			  rec_mode_list_append(&yyval.rec_mode_list, yyvsp[0].rec_mode);
			}
    break;

  case 16:
#line 168 "rec_parse.y"
    {
			  yyval.rec_mode = rec_get_mode((rec_t *) rec, yyvsp[0].sval);
			  free(yyvsp[0].sval);
			}
    break;

  case 17:
#line 175 "rec_parse.y"
    {
			  gesture_list_init(&yyval.gesture_list);
			  gesture_list_append(&yyval.gesture_list, &yyvsp[0].gesture);
			}
    break;

  case 18:
#line 180 "rec_parse.y"
    {
			  yyval.gesture_list = yyvsp[-1].gesture_list;
			  gesture_list_append(&yyval.gesture_list, &yyvsp[0].gesture);
			}
    break;

  case 19:
#line 187 "rec_parse.y"
    {
			  if (IS_FEATURE_ERROR(yyvsp[0].feature_list)) {
			    char *loc = rec_lex_location_alloc();
			    char *a_str = action_str_alloc(&yyvsp[-2].action);
			    fprintf(stderr, "%s: Not adding stroke for "
				    "`%s' to database.\n", loc, a_str);
			    free(loc);
			    free(a_str);
			  } else {
			    gesture_init(&yyval.gesture, yyvsp[-2].action, yyvsp[0].feature_list);
			  }
			}
    break;

  case 20:
#line 202 "rec_parse.y"
    {
			  action_init(&yyval.action);
			  action_add_item(&yyval.action, yyvsp[0].action_item);
			}
    break;

  case 21:
#line 207 "rec_parse.y"
    {
			  yyval.action = yyvsp[-2].action;
			  action_add_item(&yyval.action, yyvsp[0].action_item);
			}
    break;

  case 22:
#line 214 "rec_parse.y"
    {
			  action_item_key_init(&yyval.action_item, yyvsp[0].sval, 1);
			  free(yyvsp[0].sval);
			}
    break;

  case 23:
#line 219 "rec_parse.y"
    {
			  char key[2];
			  key[0] = '0' + yyvsp[0].ival;
			  key[1] = '\0';
			  action_item_key_init(&yyval.action_item, key, 1);
			}
    break;

  case 24:
#line 226 "rec_parse.y"
    {
			  action_item_button_init(&yyval.action_item, yyvsp[0].ival);
			}
    break;

  case 25:
#line 230 "rec_parse.y"
    {
			  action_item_mode_init(&yyval.action_item,
						rec_get_mode((rec_t *) rec, yyvsp[0].sval),
						0);
			  free(yyvsp[0].sval);
			}
    break;

  case 26:
#line 237 "rec_parse.y"
    {
			  action_item_mode_init(&yyval.action_item,
						rec_get_mode((rec_t *) rec, yyvsp[0].sval),
						1);
			  free(yyvsp[0].sval);
			}
    break;

  case 27:
#line 244 "rec_parse.y"
    {
			  action_item_exec_init(&yyval.action_item, yyvsp[0].sval);
			  free(yyvsp[0].sval);
			}
    break;

  case 28:
#line 249 "rec_parse.y"
    {
			  /* Convert from degrees to radians */
			  double orientation = (M_PI / 180.0) * yyvsp[0].ival;
			  action_item_orient_init(&yyval.action_item, orientation);
			}
    break;

  case 29:
#line 255 "rec_parse.y"
    {
			  /* Convert from degrees to radians */
			  double orientation = (M_PI / 180.0) * yyvsp[0].dval;
			  action_item_orient_init(&yyval.action_item, orientation);
			}
    break;

  case 30:
#line 263 "rec_parse.y"
    { rec_lex_newlines_wanted(1); }
    break;

  case 31:
#line 265 "rec_parse.y"
    { 
				  rec_lex_newlines_wanted(0);
				  feature_list_init(&yyval.feature_list);
				  feature_list_append(&yyval.feature_list, &yyvsp[-2].feature);
				}
    break;

  case 32:
#line 271 "rec_parse.y"
    { rec_lex_newlines_wanted(1); }
    break;

  case 33:
#line 273 "rec_parse.y"
    { 
				  rec_lex_newlines_wanted(0);
				  yyval.feature_list = FEATURE_ERROR;
				  yyerrok;
				}
    break;

  case 34:
#line 279 "rec_parse.y"
    { yyval.feature_list = yyvsp[-1].feature_list; }
    break;

  case 35:
#line 281 "rec_parse.y"
    { yyval.feature_list = FEATURE_ERROR; yyerrok; }
    break;

  case 36:
#line 285 "rec_parse.y"
    {
				  feature_list_init(&yyval.feature_list);
				  feature_list_append(&yyval.feature_list, &yyvsp[0].feature);
				}
    break;

  case 37:
#line 290 "rec_parse.y"
    {
				  yyval.feature_list = yyvsp[-1].feature_list;
				}
    break;

  case 38:
#line 294 "rec_parse.y"
    {
				  yyval.feature_list = yyvsp[-1].feature_list;
				  feature_list_append(&yyval.feature_list, &yyvsp[0].feature);
				}
    break;

  case 39:
#line 301 "rec_parse.y"
    {
				  feature_init(&yyval.feature, (rec_t *) rec, yyvsp[-3].sval, yyvsp[-1].sval);
				  free(yyvsp[-3].sval);
				  free(yyvsp[-1].sval);
				}
    break;

  case 40:
#line 309 "rec_parse.y"
    {
			  yyval.rec_engine = rec_get_engine((rec_t *) rec, yyvsp[-2].sval);
			  free(yyvsp[-2].sval);
			}
    break;

  case 41:
#line 314 "rec_parse.y"
    {
			  int i;
			  yyval.rec_engine = rec_get_engine((rec_t *) rec, yyvsp[-3].sval);
			  for (i=0; i < yyvsp[-1].option_list.num_options; i++) {
			    rec_engine_set_option(yyval.rec_engine,
						  yyvsp[-1].option_list.options[i].name,
						  yyvsp[-1].option_list.options[i].value);
			    free(yyvsp[-1].option_list.options[i].name);
			    free(yyvsp[-1].option_list.options[i].value);
			  }
			  option_list_deinit(&yyvsp[-1].option_list);
			  free(yyvsp[-3].sval);
			}
    break;

  case 42:
#line 330 "rec_parse.y"
    {
			  option_list_init(&yyval.option_list);
			  option_list_append(&yyval.option_list, &yyvsp[0].option);
			}
    break;

  case 43:
#line 335 "rec_parse.y"
    {
			  yyval.option_list = yyvsp[-1].option_list;
			  option_list_append(&yyval.option_list, &yyvsp[0].option);
			}
    break;

  case 44:
#line 342 "rec_parse.y"
    { option_init(&yyval.option, yyvsp[-1].sval, yyvsp[0].sval); }
    break;


    }

/* Line 999 of yacc.c.  */
#line 1450 "y.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 345 "rec_parse.y"


static int yyerror(char *err)
{
    char *loc = rec_lex_location_alloc();
    fprintf(stderr, "%s: %s\n", loc, err);
    free(loc);
    return 0;
}

int rec_parse(rec_t *rec, char *filename)
{
    int ret_val;

#ifdef REC_PARSE_DEBUG
    yydebug = 1;
#endif

    rec_lex_initial_file(filename);
    ret_val = yyparse(rec);

    return ret_val;
}

