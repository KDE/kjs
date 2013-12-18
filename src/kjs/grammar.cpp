/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         kjsyyparse
#define yylex           kjsyylex
#define yyerror         kjsyyerror
#define yylval          kjsyylval
#define yychar          kjsyychar
#define yydebug         kjsyydebug
#define yynerrs         kjsyynerrs
#define yylloc          kjsyylloc

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "grammar.y"


/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include "object.h"
#include "types.h"
#include "interpreter.h"
#include "nodes.h"
#include "makenodes.h"
#include "lexer.h"
#include "internal.h"

// Not sure why, but yacc doesn't add this define along with the others.
#define yylloc kjsyylloc

/* default values for bison */
#define YYDEBUG 0 // Set to 1 to debug a parse error.
#define kjsyydebug 0 // Set to 1 to debug a parse error.
#if !PLATFORM(DARWIN)
    // avoid triggering warnings in older bison
#define YYERROR_VERBOSE
#endif

extern int kjsyylex();
int kjsyyerror(const char *);
static bool allowAutomaticSemicolon();

#define AUTO_SEMICOLON do { if (!allowAutomaticSemicolon()) YYABORT; } while (0)
#define DBG(l, s, e) (l)->setLoc((s).first_line, (e).last_line)

#ifndef __GNUC__
#   define  __attribute__(x)
#endif

using namespace KJS;


/* Line 371 of yacc.c  */
#line 139 "grammar.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "grammar.tab.h".  */
#ifndef YY_KJSYY_GRAMMAR_TAB_H_INCLUDED
# define YY_KJSYY_GRAMMAR_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int kjsyydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NULLTOKEN = 258,
     TRUETOKEN = 259,
     FALSETOKEN = 260,
     BREAK = 261,
     CASE = 262,
     DEFAULT = 263,
     FOR = 264,
     NEW = 265,
     VAR = 266,
     CONSTTOKEN = 267,
     CONTINUE = 268,
     FUNCTION = 269,
     RETURN = 270,
     VOIDTOKEN = 271,
     DELETETOKEN = 272,
     IF = 273,
     THISTOKEN = 274,
     DO = 275,
     WHILE = 276,
     INTOKEN = 277,
     INSTANCEOF = 278,
     TYPEOF = 279,
     SWITCH = 280,
     WITH = 281,
     RESERVED = 282,
     THROW = 283,
     TRY = 284,
     CATCH = 285,
     FINALLY = 286,
     DEBUGGER = 287,
     IMPORT = 288,
     IF_WITHOUT_ELSE = 289,
     ELSE = 290,
     EQEQ = 291,
     NE = 292,
     STREQ = 293,
     STRNEQ = 294,
     LE = 295,
     GE = 296,
     OR = 297,
     AND = 298,
     PLUSPLUS = 299,
     MINUSMINUS = 300,
     LSHIFT = 301,
     RSHIFT = 302,
     URSHIFT = 303,
     PLUSEQUAL = 304,
     MINUSEQUAL = 305,
     MULTEQUAL = 306,
     DIVEQUAL = 307,
     LSHIFTEQUAL = 308,
     RSHIFTEQUAL = 309,
     URSHIFTEQUAL = 310,
     ANDEQUAL = 311,
     MODEQUAL = 312,
     XOREQUAL = 313,
     OREQUAL = 314,
     NUMBER = 315,
     STRING = 316,
     IDENT = 317,
     AUTOPLUSPLUS = 318,
     AUTOMINUSMINUS = 319
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 64 "grammar.y"

  int                 ival;
  double              dval;
  UString             *ustr;
  Identifier          *ident;
  Node                *node;
  StatementNode       *stat;
  ParameterNode       *param;
  FunctionBodyNode    *body;
  FuncDeclNode        *func;
  FuncExprNode        *funcExpr;
  ProgramNode         *prog;
  AssignExprNode      *init;
  SourceElementsNode  *srcs;
  ArgumentsNode       *args;
  ArgumentListNode    *alist;
  VarDeclNode         *decl;
  VarDeclListNode     *vlist;
  CaseBlockNode       *cblk;
  ClauseListNode      *clist;
  CaseClauseNode      *ccl;
  ElementNode         *elm;
  Operator            op;
  PropertyListNode   *plist;
  PropertyNode       *pnode;
  PropertyNameNode   *pname;
  PackageNameNode     *pkgn;


/* Line 387 of yacc.c  */
#line 276 "grammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE kjsyylval;
extern YYLTYPE kjsyylloc;
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int kjsyyparse (void *YYPARSE_PARAM);
#else
int kjsyyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int kjsyyparse (void);
#else
int kjsyyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_KJSYY_GRAMMAR_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 317 "grammar.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  212
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1718

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  89
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  352
/* YYNRULES -- Number of states.  */
#define YYNSTATES  589

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,     2,     2,     2,    80,    83,     2,
      67,    68,    79,    75,    69,    76,    74,    65,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    66,    88,
      81,    87,    82,    86,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    72,     2,    73,    84,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    85,    71,    77,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    27,    29,    31,    33,    35,    37,
      39,    41,    43,    45,    47,    49,    51,    53,    55,    57,
      59,    61,    63,    65,    67,    69,    71,    73,    75,    77,
      79,    81,    83,    85,    87,    89,    93,    94,   101,   102,
     110,   112,   116,   118,   121,   125,   130,   132,   134,   136,
     138,   142,   146,   150,   156,   159,   164,   165,   167,   169,
     172,   174,   176,   181,   185,   189,   191,   196,   200,   204,
     206,   209,   211,   214,   217,   220,   225,   229,   232,   235,
     240,   244,   247,   251,   253,   257,   259,   261,   263,   265,
     267,   270,   273,   275,   278,   281,   284,   287,   290,   293,
     296,   299,   302,   305,   308,   311,   314,   316,   318,   320,
     322,   324,   328,   332,   336,   338,   342,   346,   350,   352,
     356,   360,   362,   366,   370,   372,   376,   380,   384,   386,
     390,   394,   398,   400,   404,   408,   412,   416,   420,   424,
     426,   430,   434,   438,   442,   446,   448,   452,   456,   460,
     464,   468,   472,   474,   478,   482,   486,   490,   492,   496,
     500,   504,   508,   510,   514,   518,   522,   526,   528,   532,
     534,   538,   540,   544,   546,   550,   552,   556,   558,   562,
     564,   568,   570,   574,   576,   580,   582,   586,   588,   592,
     594,   598,   600,   604,   606,   610,   612,   616,   618,   624,
     626,   632,   634,   640,   642,   646,   648,   652,   654,   658,
     660,   662,   664,   666,   668,   670,   672,   674,   676,   678,
     680,   682,   684,   688,   690,   694,   696,   700,   702,   704,
     706,   708,   710,   712,   714,   716,   718,   720,   722,   724,
     726,   728,   730,   732,   734,   737,   741,   745,   749,   751,
     755,   757,   761,   763,   766,   768,   771,   775,   779,   781,
     785,   787,   790,   793,   796,   798,   801,   804,   810,   818,
     826,   834,   840,   850,   861,   869,   878,   888,   889,   891,
     892,   894,   897,   900,   904,   908,   911,   914,   918,   922,
     925,   928,   932,   936,   942,   948,   952,   958,   959,   961,
     963,   966,   970,   975,   978,   982,   986,   990,   994,   999,
    1007,  1017,  1020,  1023,  1025,  1029,  1035,  1041,  1045,  1049,
    1055,  1061,  1062,  1069,  1070,  1078,  1079,  1085,  1086,  1093,
    1094,  1101,  1102,  1110,  1112,  1116,  1119,  1123,  1124,  1126,
    1128,  1131,  1133
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     200,     0,    -1,     6,    -1,     7,    -1,     8,    -1,     9,
      -1,    10,    -1,    11,    -1,    12,    -1,    13,    -1,    14,
      -1,    15,    -1,    16,    -1,    17,    -1,    18,    -1,    19,
      -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
      -1,    25,    -1,    26,    -1,    28,    -1,    29,    -1,    30,
      -1,    31,    -1,    32,    -1,    33,    -1,     3,    -1,     4,
      -1,     5,    -1,    35,    -1,    62,    -1,    90,    -1,     3,
      -1,     4,    -1,     5,    -1,    60,    -1,    61,    -1,    65,
      -1,    52,    -1,    91,    -1,    61,    -1,    60,    -1,    93,
      66,   149,    -1,    -1,    62,    91,    67,    68,    95,   199,
      -1,    -1,    62,    91,    67,   198,    68,    96,   199,    -1,
      94,    -1,    97,    69,    94,    -1,    99,    -1,    70,    71,
      -1,    70,    97,    71,    -1,    70,    97,    69,    71,    -1,
      19,    -1,    92,    -1,   100,    -1,    62,    -1,    67,   153,
      68,    -1,    72,   102,    73,    -1,    72,   101,    73,    -1,
      72,   101,    69,   102,    73,    -1,   102,   149,    -1,   101,
      69,   102,   149,    -1,    -1,   103,    -1,    69,    -1,   103,
      69,    -1,    98,    -1,   193,    -1,   104,    72,   153,    73,
      -1,   104,    74,    91,    -1,    10,   104,   110,    -1,    99,
      -1,   105,    72,   153,    73,    -1,   105,    74,    91,    -1,
      10,   104,   110,    -1,   104,    -1,    10,   106,    -1,   105,
      -1,    10,   106,    -1,   104,   110,    -1,   108,   110,    -1,
     108,    72,   153,    73,    -1,   108,    74,    91,    -1,   105,
     110,    -1,   109,   110,    -1,   109,    72,   153,    73,    -1,
     109,    74,    91,    -1,    67,    68,    -1,    67,   111,    68,
      -1,   149,    -1,   111,    69,   149,    -1,   106,    -1,   108,
      -1,   107,    -1,   109,    -1,   112,    -1,   112,    44,    -1,
     112,    45,    -1,   113,    -1,   113,    44,    -1,   113,    45,
      -1,    17,   117,    -1,    16,   117,    -1,    24,   117,    -1,
      44,   117,    -1,    63,   117,    -1,    45,   117,    -1,    64,
     117,    -1,    75,   117,    -1,    76,   117,    -1,    77,   117,
      -1,    78,   117,    -1,   114,    -1,   116,    -1,   115,    -1,
     116,    -1,   117,    -1,   119,    79,   117,    -1,   119,    65,
     117,    -1,   119,    80,   117,    -1,   118,    -1,   120,    79,
     117,    -1,   120,    65,   117,    -1,   120,    80,   117,    -1,
     119,    -1,   121,    75,   119,    -1,   121,    76,   119,    -1,
     120,    -1,   122,    75,   119,    -1,   122,    76,   119,    -1,
     121,    -1,   123,    46,   121,    -1,   123,    47,   121,    -1,
     123,    48,   121,    -1,   122,    -1,   124,    46,   121,    -1,
     124,    47,   121,    -1,   124,    48,   121,    -1,   123,    -1,
     125,    81,   123,    -1,   125,    82,   123,    -1,   125,    40,
     123,    -1,   125,    41,   123,    -1,   125,    23,   123,    -1,
     125,    22,   123,    -1,   123,    -1,   126,    81,   123,    -1,
     126,    82,   123,    -1,   126,    40,   123,    -1,   126,    41,
     123,    -1,   126,    23,   123,    -1,   124,    -1,   127,    81,
     123,    -1,   127,    82,   123,    -1,   127,    40,   123,    -1,
     127,    41,   123,    -1,   127,    23,   123,    -1,   127,    22,
     123,    -1,   125,    -1,   128,    36,   125,    -1,   128,    37,
     125,    -1,   128,    38,   125,    -1,   128,    39,   125,    -1,
     126,    -1,   129,    36,   126,    -1,   129,    37,   126,    -1,
     129,    38,   126,    -1,   129,    39,   126,    -1,   127,    -1,
     130,    36,   125,    -1,   130,    37,   125,    -1,   130,    38,
     125,    -1,   130,    39,   125,    -1,   128,    -1,   131,    83,
     128,    -1,   129,    -1,   132,    83,   129,    -1,   130,    -1,
     133,    83,   128,    -1,   131,    -1,   134,    84,   131,    -1,
     132,    -1,   135,    84,   132,    -1,   133,    -1,   136,    84,
     131,    -1,   134,    -1,   137,    85,   134,    -1,   135,    -1,
     138,    85,   135,    -1,   136,    -1,   139,    85,   134,    -1,
     137,    -1,   140,    43,   137,    -1,   138,    -1,   141,    43,
     138,    -1,   139,    -1,   142,    43,   137,    -1,   140,    -1,
     143,    42,   140,    -1,   141,    -1,   144,    42,   141,    -1,
     142,    -1,   145,    42,   140,    -1,   143,    -1,   143,    86,
     149,    66,   149,    -1,   144,    -1,   144,    86,   150,    66,
     150,    -1,   145,    -1,   145,    86,   149,    66,   149,    -1,
     146,    -1,   112,   152,   149,    -1,   147,    -1,   112,   152,
     150,    -1,   148,    -1,   113,   152,   149,    -1,    87,    -1,
      49,    -1,    50,    -1,    51,    -1,    52,    -1,    53,    -1,
      54,    -1,    55,    -1,    56,    -1,    58,    -1,    59,    -1,
      57,    -1,   149,    -1,   153,    69,   149,    -1,   150,    -1,
     154,    69,   150,    -1,   151,    -1,   155,    69,   149,    -1,
     157,    -1,   158,    -1,   163,    -1,   168,    -1,   169,    -1,
     170,    -1,   171,    -1,   174,    -1,   175,    -1,   176,    -1,
     177,    -1,   178,    -1,   184,    -1,   185,    -1,   186,    -1,
     187,    -1,   189,    -1,    70,    71,    -1,    70,   201,    71,
      -1,    11,   159,    88,    -1,    11,   159,     1,    -1,   161,
      -1,   159,    69,   161,    -1,   162,    -1,   160,    69,   162,
      -1,    62,    -1,    62,   166,    -1,    62,    -1,    62,   167,
      -1,    12,   164,    88,    -1,    12,   164,     1,    -1,   165,
      -1,   164,    69,   165,    -1,    62,    -1,    62,   166,    -1,
      87,   149,    -1,    87,   150,    -1,    88,    -1,   155,    88,
      -1,   155,     1,    -1,    18,    67,   153,    68,   156,    -1,
      18,    67,   153,    68,   156,    35,   156,    -1,    20,   156,
      21,    67,   153,    68,    88,    -1,    20,   156,    21,    67,
     153,    68,     1,    -1,    21,    67,   153,    68,   156,    -1,
       9,    67,   173,    88,   172,    88,   172,    68,   156,    -1,
       9,    67,    11,   160,    88,   172,    88,   172,    68,   156,
      -1,     9,    67,   112,    22,   153,    68,   156,    -1,     9,
      67,    11,    62,    22,   153,    68,   156,    -1,     9,    67,
      11,    62,   167,    22,   153,    68,   156,    -1,    -1,   153,
      -1,    -1,   154,    -1,    13,    88,    -1,    13,     1,    -1,
      13,    62,    88,    -1,    13,    62,     1,    -1,     6,    88,
      -1,     6,     1,    -1,     6,    62,    88,    -1,     6,    62,
       1,    -1,    15,    88,    -1,    15,     1,    -1,    15,   153,
      88,    -1,    15,   153,     1,    -1,    26,    67,   153,    68,
     156,    -1,    25,    67,   153,    68,   179,    -1,    70,   180,
      71,    -1,    70,   180,   183,   180,    71,    -1,    -1,   181,
      -1,   182,    -1,   181,   182,    -1,     7,   153,    66,    -1,
       7,   153,    66,   201,    -1,     8,    66,    -1,     8,    66,
     201,    -1,    62,    66,   156,    -1,    28,   153,    88,    -1,
      28,   153,     1,    -1,    29,   157,    31,   157,    -1,    29,
     157,    30,    67,    62,    68,   157,    -1,    29,   157,    30,
      67,    62,    68,   157,    31,   157,    -1,    32,    88,    -1,
      32,     1,    -1,    62,    -1,   188,    74,    62,    -1,    33,
     188,    74,    79,    88,    -1,    33,   188,    74,    79,     1,
      -1,    33,   188,    88,    -1,    33,   188,     1,    -1,    33,
      62,    87,   188,    88,    -1,    33,    62,    87,   188,     1,
      -1,    -1,    14,    62,    67,    68,   191,   199,    -1,    -1,
      14,    62,    67,   198,    68,   192,   199,    -1,    -1,    14,
      67,    68,   194,   199,    -1,    -1,    14,    67,   198,    68,
     195,   199,    -1,    -1,    14,    62,    67,    68,   196,   199,
      -1,    -1,    14,    62,    67,   198,    68,   197,   199,    -1,
      62,    -1,   198,    69,    62,    -1,    70,    71,    -1,    70,
     201,    71,    -1,    -1,   201,    -1,   202,    -1,   201,   202,
      -1,   190,    -1,   156,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   192,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   226,   227,   231,   232,   233,   234,   235,
     236,   242,   251,   252,   253,   257,   258,   258,   262,   262,
     269,   270,   274,   275,   276,   278,   282,   283,   284,   285,
     286,   291,   292,   293,   297,   298,   303,   304,   308,   309,
     313,   314,   315,   316,   317,   321,   322,   323,   324,   328,
     329,   333,   334,   338,   339,   340,   341,   345,   346,   347,
     348,   352,   353,   357,   358,   362,   363,   367,   368,   372,
     373,   374,   378,   379,   380,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   397,   398,   402,   403,
     407,   408,   409,   410,   414,   415,   417,   419,   424,   425,
     426,   430,   431,   433,   438,   439,   440,   441,   445,   446,
     447,   448,   452,   453,   454,   455,   456,   457,   458,   462,
     463,   464,   465,   466,   467,   472,   473,   474,   475,   476,
     477,   479,   483,   484,   485,   486,   487,   491,   492,   494,
     496,   498,   503,   504,   506,   507,   509,   514,   515,   519,
     520,   525,   526,   530,   531,   535,   536,   541,   542,   547,
     548,   552,   553,   558,   559,   564,   565,   569,   570,   575,
     576,   581,   582,   586,   587,   592,   593,   597,   598,   603,
     604,   609,   610,   615,   616,   621,   622,   627,   628,   633,
     634,   635,   636,   637,   638,   639,   640,   641,   642,   643,
     644,   648,   649,   653,   654,   658,   659,   663,   664,   665,
     666,   667,   668,   669,   670,   671,   672,   673,   674,   675,
     676,   677,   678,   679,   683,   684,   688,   689,   693,   694,
     699,   700,   705,   706,   710,   711,   715,   716,   721,   722,
     727,   728,   732,   736,   740,   744,   745,   749,   751,   756,
     757,   758,   759,   761,   763,   771,   773,   778,   779,   783,
     784,   788,   789,   790,   791,   795,   796,   797,   798,   802,
     803,   804,   805,   809,   813,   817,   818,   823,   824,   828,
     829,   833,   834,   838,   839,   843,   847,   848,   852,   853,
     854,   859,   860,   864,   865,   869,   871,   873,   875,   877,
     879,   884,   884,   885,   885,   890,   890,   893,   893,   896,
     896,   897,   897,   903,   904,   908,   909,   913,   914,   918,
     919,   923,   924
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NULLTOKEN", "TRUETOKEN", "FALSETOKEN",
  "BREAK", "CASE", "DEFAULT", "FOR", "NEW", "VAR", "CONSTTOKEN",
  "CONTINUE", "FUNCTION", "RETURN", "VOIDTOKEN", "DELETETOKEN", "IF",
  "THISTOKEN", "DO", "WHILE", "INTOKEN", "INSTANCEOF", "TYPEOF", "SWITCH",
  "WITH", "RESERVED", "THROW", "TRY", "CATCH", "FINALLY", "DEBUGGER",
  "IMPORT", "IF_WITHOUT_ELSE", "ELSE", "EQEQ", "NE", "STREQ", "STRNEQ",
  "LE", "GE", "OR", "AND", "PLUSPLUS", "MINUSMINUS", "LSHIFT", "RSHIFT",
  "URSHIFT", "PLUSEQUAL", "MINUSEQUAL", "MULTEQUAL", "DIVEQUAL",
  "LSHIFTEQUAL", "RSHIFTEQUAL", "URSHIFTEQUAL", "ANDEQUAL", "MODEQUAL",
  "XOREQUAL", "OREQUAL", "NUMBER", "STRING", "IDENT", "AUTOPLUSPLUS",
  "AUTOMINUSMINUS", "'/'", "':'", "'('", "')'", "','", "'{'", "'}'", "'['",
  "']'", "'.'", "'+'", "'-'", "'~'", "'!'", "'*'", "'%'", "'<'", "'>'",
  "'&'", "'^'", "'|'", "'?'", "'='", "';'", "$accept", "Keywords",
  "IdentifierName", "Literal", "PropertyName", "Property", "$@1", "$@2",
  "PropertyList", "PrimaryExpr", "PrimaryExprNoBrace", "ArrayLiteral",
  "ElementList", "ElisionOpt", "Elision", "MemberExpr", "MemberExprNoBF",
  "NewExpr", "NewExprNoBF", "CallExpr", "CallExprNoBF", "Arguments",
  "ArgumentList", "LeftHandSideExpr", "LeftHandSideExprNoBF",
  "PostfixExpr", "PostfixExprNoBF", "UnaryExprCommon", "UnaryExpr",
  "UnaryExprNoBF", "MultiplicativeExpr", "MultiplicativeExprNoBF",
  "AdditiveExpr", "AdditiveExprNoBF", "ShiftExpr", "ShiftExprNoBF",
  "RelationalExpr", "RelationalExprNoIn", "RelationalExprNoBF",
  "EqualityExpr", "EqualityExprNoIn", "EqualityExprNoBF", "BitwiseANDExpr",
  "BitwiseANDExprNoIn", "BitwiseANDExprNoBF", "BitwiseXORExpr",
  "BitwiseXORExprNoIn", "BitwiseXORExprNoBF", "BitwiseORExpr",
  "BitwiseORExprNoIn", "BitwiseORExprNoBF", "LogicalANDExpr",
  "LogicalANDExprNoIn", "LogicalANDExprNoBF", "LogicalORExpr",
  "LogicalORExprNoIn", "LogicalORExprNoBF", "ConditionalExpr",
  "ConditionalExprNoIn", "ConditionalExprNoBF", "AssignmentExpr",
  "AssignmentExprNoIn", "AssignmentExprNoBF", "AssignmentOperator", "Expr",
  "ExprNoIn", "ExprNoBF", "Statement", "Block", "VariableStatement",
  "VariableDeclarationList", "VariableDeclarationListNoIn",
  "VariableDeclaration", "VariableDeclarationNoIn", "ConstStatement",
  "ConstDeclarationList", "ConstDeclaration", "Initializer",
  "InitializerNoIn", "EmptyStatement", "ExprStatement", "IfStatement",
  "IterationStatement", "ExprOpt", "ExprNoInOpt", "ContinueStatement",
  "BreakStatement", "ReturnStatement", "WithStatement", "SwitchStatement",
  "CaseBlock", "CaseClausesOpt", "CaseClauses", "CaseClause",
  "DefaultClause", "LabelledStatement", "ThrowStatement", "TryStatement",
  "DebuggerStatement", "PackageName", "ImportStatement",
  "FunctionDeclaration", "$@3", "$@4", "FunctionExpr", "$@5", "$@6", "$@7",
  "$@8", "FormalParameterList", "FunctionBody", "Program",
  "SourceElements", "SourceElement", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,    47,    58,    40,    41,    44,
     123,   125,    91,    93,    46,    43,    45,   126,    33,    42,
      37,    60,    62,    38,    94,   124,    63,    61,    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    89,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    92,    92,    92,    92,    92,
      92,    92,    93,    93,    93,    94,    95,    94,    96,    94,
      97,    97,    98,    98,    98,    98,    99,    99,    99,    99,
      99,   100,   100,   100,   101,   101,   102,   102,   103,   103,
     104,   104,   104,   104,   104,   105,   105,   105,   105,   106,
     106,   107,   107,   108,   108,   108,   108,   109,   109,   109,
     109,   110,   110,   111,   111,   112,   112,   113,   113,   114,
     114,   114,   115,   115,   115,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   117,   117,   118,   118,
     119,   119,   119,   119,   120,   120,   120,   120,   121,   121,
     121,   122,   122,   122,   123,   123,   123,   123,   124,   124,
     124,   124,   125,   125,   125,   125,   125,   125,   125,   126,
     126,   126,   126,   126,   126,   127,   127,   127,   127,   127,
     127,   127,   128,   128,   128,   128,   128,   129,   129,   129,
     129,   129,   130,   130,   130,   130,   130,   131,   131,   132,
     132,   133,   133,   134,   134,   135,   135,   136,   136,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   141,   142,
     142,   143,   143,   144,   144,   145,   145,   146,   146,   147,
     147,   148,   148,   149,   149,   150,   150,   151,   151,   152,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   153,   153,   154,   154,   155,   155,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   157,   157,   158,   158,   159,   159,
     160,   160,   161,   161,   162,   162,   163,   163,   164,   164,
     165,   165,   166,   167,   168,   169,   169,   170,   170,   171,
     171,   171,   171,   171,   171,   171,   171,   172,   172,   173,
     173,   174,   174,   174,   174,   175,   175,   175,   175,   176,
     176,   176,   176,   177,   178,   179,   179,   180,   180,   181,
     181,   182,   182,   183,   183,   184,   185,   185,   186,   186,
     186,   187,   187,   188,   188,   189,   189,   189,   189,   189,
     189,   191,   190,   192,   190,   194,   193,   195,   193,   196,
     193,   197,   193,   198,   198,   199,   199,   200,   200,   201,
     201,   202,   202
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     0,     6,     0,     7,
       1,     3,     1,     2,     3,     4,     1,     1,     1,     1,
       3,     3,     3,     5,     2,     4,     0,     1,     1,     2,
       1,     1,     4,     3,     3,     1,     4,     3,     3,     1,
       2,     1,     2,     2,     2,     4,     3,     2,     2,     4,
       3,     2,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     3,     3,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     1,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     5,     1,
       5,     1,     5,     1,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     3,     3,     1,     3,
       1,     3,     1,     2,     1,     2,     3,     3,     1,     3,
       1,     2,     2,     2,     1,     2,     2,     5,     7,     7,
       7,     5,     9,    10,     7,     8,     9,     0,     1,     0,
       1,     2,     2,     3,     3,     2,     2,     3,     3,     2,
       2,     3,     3,     5,     5,     3,     5,     0,     1,     1,
       2,     3,     4,     2,     3,     3,     3,     3,     4,     7,
       9,     2,     2,     1,     3,     5,     5,     3,     3,     5,
       5,     0,     6,     0,     7,     0,     5,     0,     6,     0,
       6,     0,     7,     1,     3,     2,     3,     0,     1,     1,
       2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     347,    35,    36,    37,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    56,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    41,    38,    39,
      59,     0,     0,    40,     0,     0,    66,     0,     0,     0,
       0,   274,    57,    75,    58,    81,    97,    98,   102,   118,
     119,   124,   131,   138,   155,   172,   181,   187,   193,   199,
     205,   211,   217,   235,     0,   352,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   351,     0,   348,   349,   296,     0,   295,
     289,     0,     0,    59,     0,    70,    52,    79,    82,    71,
     262,     0,   258,   270,     0,   268,   292,     0,   291,     0,
     300,   299,    79,    95,    96,    99,   116,   117,   120,   128,
     134,   142,   162,   177,   183,   189,   195,   201,   207,   213,
     231,     0,    99,   106,   105,     0,     0,     0,   107,     0,
       0,     0,     0,   322,   321,   323,     0,   108,   110,     0,
     109,   111,     0,   254,     0,    68,     0,     0,    67,   112,
     113,   114,   115,     0,     0,     0,    87,     0,     0,    88,
     103,   104,   220,   221,   222,   223,   224,   225,   226,   227,
     230,   228,   229,   219,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   276,
       0,   275,     1,   350,   298,   297,     0,    99,   149,   167,
     179,   185,   191,   197,   203,   209,   215,   233,   290,     0,
      79,    80,     0,     0,    29,    30,    31,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    32,    44,    43,    33,    53,    34,
      42,     0,    50,     0,     0,     0,    78,     0,   263,   257,
       0,   256,   271,   267,     0,   266,   294,   293,     0,    83,
       0,     0,    84,   100,   101,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     302,     0,   301,     0,     0,     0,     0,     0,   317,   316,
       0,     0,     0,   328,     0,   327,   315,    60,   255,    66,
      62,    61,    64,    69,    91,     0,    93,     0,    33,    77,
       0,    90,   218,   126,   125,   127,   132,   133,   139,   140,
     141,   161,   160,   158,   159,   156,   157,   173,   174,   175,
     176,   182,   188,   194,   200,   206,     0,   236,   264,     0,
     260,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   287,
      74,     0,   343,   335,     0,     0,     0,     0,    54,     0,
      73,   272,   259,   269,   331,     0,     0,    86,   214,   122,
     121,   123,   129,   130,   135,   136,   137,   148,   147,   145,
     146,   143,   144,   163,   164,   165,   166,   178,   184,   190,
     196,   202,     0,   232,     0,     0,     0,     0,     0,     0,
     318,   323,     0,   324,     0,     0,    92,     0,    76,    89,
       0,     0,     0,   265,     0,   287,     0,    99,   216,   154,
     152,   153,   150,   151,   168,   169,   170,   171,   180,   186,
     192,   198,   204,     0,   234,   288,     0,   339,     0,     0,
     337,     0,     0,    45,    55,    51,    72,     0,   333,    85,
       0,   277,     0,   281,   307,   304,   303,     0,   330,     0,
     329,   326,   325,    63,    65,    94,   212,     0,   273,     0,
     264,   261,     0,     0,     0,   287,     0,   341,     0,   336,
       0,   344,    46,     0,   332,     0,   208,     0,     0,     0,
       0,   308,   309,     0,     0,     0,   265,   287,   284,   210,
       0,   340,     0,   345,     0,   338,     0,    48,   334,   278,
     280,   279,     0,     0,   305,   307,   310,   319,   285,     0,
       0,     0,   342,   346,    47,     0,   311,   313,     0,     0,
     286,     0,   282,    49,   312,   314,   306,   320,   283
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   269,   270,    42,   271,   272,   556,   575,   273,    95,
      96,    44,   156,   157,   158,   112,    45,   113,    46,   114,
      47,   166,   345,   132,    48,   116,    49,   117,   118,    51,
     119,    52,   120,    53,   121,    54,   122,   219,    55,   123,
     220,    56,   124,   221,    57,   125,   222,    58,   126,   223,
      59,   127,   224,    60,   128,   225,    61,   129,   226,    62,
     130,   227,    63,   382,   485,   228,    64,    65,    66,    67,
     101,   379,   102,   380,    68,   104,   105,   278,   463,    69,
      70,    71,    72,   486,   229,    73,    74,    75,    76,    77,
     505,   540,   541,   542,   565,    78,    79,    80,    81,   146,
      82,    83,   497,   535,    99,   489,   530,   526,   552,   404,
     529,    84,    85,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -424
static const yytype_int16 yypact[] =
{
    1118,  -424,  -424,  -424,     7,    15,   320,    17,    44,    25,
      58,   718,  1502,  1502,    37,  -424,  1198,    64,  1502,   116,
     137,  1502,   152,    67,   145,  1502,  1502,  -424,  -424,  -424,
     175,  1502,  1502,  -424,  1502,   798,   189,  1502,  1502,  1502,
    1502,  -424,  -424,  -424,  -424,    40,  -424,   117,   555,  -424,
    -424,  -424,   115,    71,    46,   165,   217,   161,   193,   209,
     242,     5,  -424,  -424,    39,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,   295,  1118,  -424,  -424,    74,  -424,
     226,   320,    -4,  -424,  1578,  -424,  -424,   149,  -424,  -424,
     210,    63,  -424,   210,    69,  -424,  -424,    75,  -424,   232,
    -424,  -424,   149,  -424,   153,   699,  -424,  -424,  -424,   135,
     157,   233,   170,   228,   224,   216,   241,   268,    13,  -424,
    -424,    87,   204,  -424,  -424,  1502,   301,  1502,  -424,  1502,
    1502,    89,   231,  -424,  -424,   246,    27,  -424,  -424,  1198,
    -424,  -424,   200,  -424,   878,  -424,    36,  1274,   259,  -424,
    -424,  -424,  -424,  1350,  1502,   656,  -424,  1502,   656,  -424,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,  1502,  1502,  1502,  1502,  1502,  1502,
    1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,
    1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  -424,
    1502,  -424,  -424,  -424,  -424,  -424,   286,   362,   233,   178,
     236,   271,   280,   273,   322,    14,  -424,  -424,   297,   279,
     149,  -424,   302,    16,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,   656,  -424,  -424,
    -424,   304,  -424,    90,  1502,   656,  -424,  1502,  -424,  -424,
      17,  -424,  -424,  -424,    44,  -424,  -424,  -424,    35,  -424,
    1502,   656,  -424,  -424,  -424,  1502,  1502,  1502,  1502,  1502,
    1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,
    1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,
    -424,  1502,  -424,   215,   306,   237,   247,   267,  -424,  -424,
     307,   152,   309,  -424,     4,  -424,  -424,  -424,  -424,   189,
    -424,  -424,  -424,  -424,  -424,   269,  -424,   112,  -424,  -424,
     113,  -424,  -424,  -424,  -424,  -424,   135,   135,   157,   157,
     157,   233,   233,   233,   233,   233,   233,   170,   170,   170,
     170,   228,   224,   216,   241,   268,   312,  -424,    23,   -15,
    -424,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,
    1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,  1502,
    -424,    55,  -424,  -424,   272,   308,  1502,  1647,  -424,   155,
    -424,  -424,  -424,  -424,  -424,   274,   166,  -424,  -424,  -424,
    -424,  -424,   135,   135,   157,   157,   157,   233,   233,   233,
     233,   233,   233,   170,   170,   170,   170,   228,   224,   216,
     241,   268,   313,  -424,  1198,  1502,  1198,   298,  1198,   315,
    -424,  -424,    79,  -424,    76,  1426,  -424,  1502,  -424,  -424,
    1502,  1502,  1502,   364,   326,  1502,   276,   699,  -424,   233,
     233,   233,   233,   233,   178,   178,   178,   178,   236,   271,
     280,   273,   322,   323,  -424,   324,   303,  -424,   278,   325,
    -424,   332,   134,  -424,  -424,  -424,  -424,   325,  -424,  -424,
    1502,   361,   282,  -424,   390,  -424,  -424,   330,  -424,   337,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,   284,  -424,  1502,
     316,  -424,   317,  1198,  1502,  1502,   325,  -424,   958,  -424,
     325,  -424,  -424,   292,  -424,   325,  -424,  1198,    80,  1502,
      45,   390,  -424,   152,  1198,   294,  -424,  1502,  -424,  -424,
     336,  -424,   325,  -424,  1038,  -424,   325,  -424,  -424,  -424,
    -424,  -424,    56,   342,  -424,   390,  -424,   378,  -424,  1198,
     354,  1198,  -424,  -424,  -424,   325,  1118,  1118,   339,   152,
    -424,  1198,  -424,  -424,  1118,  1118,  -424,  -424,  -424
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -424,  -424,  -156,  -424,  -424,    18,  -424,  -424,  -424,  -424,
       0,  -424,  -424,    84,  -424,    42,  -424,    83,  -424,  -424,
    -424,   -40,  -424,   192,  -424,  -424,  -424,    11,    12,  -424,
    -155,  -424,  -173,  -424,   311,  -424,  -140,   -70,  -424,  -193,
      32,  -424,  -189,    33,  -424,  -176,    34,  -424,  -174,    41,
    -424,  -168,    31,  -424,  -424,  -424,  -424,  -424,  -424,  -424,
    -143,  -376,  -424,   -17,     2,  -424,  -424,   -13,   -18,  -424,
    -424,  -424,   150,   -35,  -424,  -424,   148,   331,   -82,  -424,
    -424,  -424,  -424,  -423,  -424,  -424,  -424,  -424,  -424,  -424,
    -424,  -128,  -424,  -102,  -424,  -424,  -424,  -424,  -424,   108,
    -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -424,  -283,
    -318,  -424,   -34,   -83
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      43,   154,   213,   136,   142,   415,   468,   169,    87,   349,
     371,    50,   351,   131,   342,   372,    43,   358,   359,   360,
     346,   483,   484,   141,   133,   134,   106,    50,   333,   373,
     138,   184,   374,   356,   357,    43,   152,   147,   148,   375,
     209,   352,   522,   150,   151,   461,    50,   207,    97,   159,
     160,   161,   162,   563,   464,   318,   396,   276,   232,   367,
     368,   369,   370,   233,   279,   376,   453,   377,   143,    88,
     283,   213,   289,   465,   292,   214,   286,   511,   402,   100,
     508,   560,    90,   454,   403,    43,   518,   107,   320,    98,
     328,   208,   190,   191,   192,    89,    50,   402,   295,   319,
     397,   334,   550,   414,   135,   339,   103,   163,   210,   340,
     462,   405,   164,   108,   165,   335,   564,   402,   488,   410,
     109,   437,   576,   487,   570,   321,   438,   211,   424,   425,
     426,   137,   280,   230,   411,   417,   336,   323,   284,   325,
     439,   326,   327,   440,   422,   423,   188,   189,   549,    43,
     441,   281,   418,   509,    43,   144,   321,   285,   321,   407,
      50,   408,   215,   287,   512,    50,   347,   510,   561,   350,
     433,   434,   435,   436,   231,   322,   442,   329,   443,   534,
     185,   321,   321,   139,   163,   458,   459,   193,   194,   167,
     400,   168,   304,   305,   186,   187,   402,   353,   354,   355,
     296,   383,   532,   115,   140,   195,   196,   145,   551,   533,
     306,   307,   555,   115,   297,   298,   163,   558,   384,   385,
     163,   274,    35,   275,   321,   290,   115,   291,   496,     1,
       2,     3,   299,   300,   572,   321,    91,   216,   574,   499,
      92,   149,    12,    13,   203,    15,   197,   198,   293,   294,
      18,   308,   309,   199,   200,   201,   202,   583,   155,   386,
     387,   330,   331,   493,   310,   311,   312,   313,   337,   321,
      25,    26,   388,   389,   390,   391,   409,   204,    27,   301,
     302,   303,   217,   444,   321,   206,    28,    29,    93,    31,
      32,    33,   416,    34,   205,   212,    94,   277,    36,   288,
     315,    37,    38,    39,    40,   446,   321,   314,   419,   420,
     421,   317,   514,   450,   515,   447,   321,   516,   474,   475,
     476,   477,   324,     1,     2,     3,   316,   115,   343,   115,
      91,   115,   115,   332,    92,   448,   321,   456,   457,    15,
     490,   491,   498,   491,   523,   321,   527,   491,   378,   115,
     538,   321,   544,   321,   392,   115,   115,   536,   394,   115,
     557,   491,   569,   321,   393,   395,   398,   399,   504,   401,
     406,   451,    27,   445,   449,   492,   115,   507,   460,   500,
      28,    29,    93,   466,   381,    33,   519,    34,   520,   524,
      94,   525,    36,   321,   531,   528,   537,   539,   543,   453,
     115,   218,   115,   462,   571,   547,   293,   294,   577,   579,
     586,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   581,   455,   478,   495,   479,   482,   480,   521,
     412,   501,   413,   503,   282,   506,   481,   578,   546,   566,
     452,     0,     0,     0,    43,     0,    43,   502,    43,   183,
       0,     0,     0,     0,     0,    50,     0,    50,     0,    50,
       0,     0,     0,   517,     0,     0,   115,     0,     0,   115,
       0,   213,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   115,     0,     0,     0,     0,   115,     0,     0,
       0,     0,     0,     0,   554,     0,     0,     0,     0,     0,
       0,   213,   213,     0,   361,   362,   363,   364,   365,   366,
     548,   115,     0,   115,     0,     0,     0,     0,     0,     0,
       0,   545,     0,    43,   559,   567,     0,     0,    43,     0,
       0,   568,     0,     0,    50,     0,     0,    43,     0,    50,
       0,   562,   584,   585,    43,     0,     0,     0,    50,     0,
       0,     0,     0,     0,    43,    50,   580,     0,   582,     0,
       0,   587,     0,     0,     0,    50,     0,     0,   588,    43,
       0,    43,     0,   115,   467,     0,    43,    43,     0,     0,
      50,    43,    50,     0,    43,    43,     0,    50,    50,   467,
     467,   115,    50,     0,     0,    50,    50,     0,   115,   170,
     171,     0,     0,     0,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   427,   428,   429,   430,   431,
     432,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   115,     0,     0,
       0,     0,   183,     0,     0,     0,     0,   115,     0,   115,
       0,     0,   115,   115,   467,     0,     0,   115,     0,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,     0,   258,   259,   260,   261,   262,   263,
       0,   264,   115,   218,   469,   470,   471,   472,   473,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
       0,   115,     0,     0,     0,     0,   467,   115,   348,   110,
       0,     1,     2,     3,     0,     0,     0,     0,    91,     0,
       0,   115,    92,     0,    12,    13,     0,    15,     0,   115,
       0,     0,    18,   293,   294,     0,     0,     0,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,   218,     0,     0,     0,     0,    28,    29,
      93,    31,    32,    33,     0,    34,   183,     0,    94,     0,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,   111,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,   218,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,   153,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    41,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,   338,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    41,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,   553,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    41,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,   573,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    41,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,     0,
      36,     0,     0,    37,    38,    39,    40,     0,     0,     0,
       0,     1,     2,     3,     4,     0,    41,     5,     6,     7,
       8,     9,     0,    11,    12,    13,    14,    15,    16,    17,
       0,     0,    18,    19,    20,     0,    21,    22,     0,     0,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,     0,    28,    29,
      30,    31,    32,    33,     0,    34,     0,     0,    35,     0,
      36,     0,     0,    37,    38,    39,    40,     1,     2,     3,
       0,     0,     0,     0,    91,     0,    41,     0,    92,     0,
      12,    13,     0,    15,     0,     0,     0,     0,    18,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,    26,
       0,     0,     0,     0,     0,     0,    27,     0,     0,     0,
       0,     0,     0,     0,    28,    29,    93,    31,    32,    33,
       0,    34,     0,     0,    94,     0,    36,   341,     0,    37,
      38,    39,    40,     1,     2,     3,     0,     0,     0,     0,
      91,     0,     0,     0,    92,     0,    12,    13,     0,    15,
       0,     0,     0,     0,    18,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,    26,     0,     0,     0,     0,
       0,     0,    27,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    93,    31,    32,    33,     0,    34,   344,     0,
      94,     0,    36,     0,     0,    37,    38,    39,    40,     1,
       2,     3,     0,     0,     0,     0,    91,     0,     0,     0,
      92,     0,    12,    13,     0,    15,     0,     0,     0,     0,
      18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,    26,     0,     0,     0,     0,     0,     0,    27,     0,
       0,     0,     0,     0,     0,     0,    28,    29,    93,    31,
      32,    33,     0,    34,     0,     0,    94,     0,    36,   513,
       0,    37,    38,    39,    40,     1,     2,     3,     0,     0,
       0,     0,    91,     0,     0,     0,    92,     0,    12,    13,
       0,    15,     0,     0,     0,     0,    18,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    25,    26,     0,     0,
       0,     0,     0,     0,    27,     0,     0,     0,     0,     0,
       0,     0,    28,    29,    93,    31,    32,    33,     0,    34,
       0,     0,    94,     0,    36,     0,     0,    37,    38,    39,
      40,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,     0,   258,   259,   260,   261,
     262,   263,     0,   264,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   265,   266,
     267,     0,     0,     0,     0,     0,     0,     0,     0,   268,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,     0,   258,   259,   260,   261,   262,
     263,     0,   264,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   265,   266,   267,
       0,     0,     0,     0,     0,     0,     0,     0,   494
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-424)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,    35,    85,    16,    22,   288,   382,    47,     1,   165,
     203,     0,   168,    11,   157,   204,    16,   190,   191,   192,
     163,   397,   398,    21,    12,    13,     1,    16,     1,   205,
      18,    48,   206,   188,   189,    35,    34,    25,    26,   207,
       1,   184,   465,    31,    32,    22,    35,    42,     6,    37,
      38,    39,    40,     8,    69,    42,    42,    97,    62,   199,
     200,   201,   202,    67,     1,   208,    62,   210,     1,    62,
       1,   154,   112,    88,   114,     1,     1,     1,    62,    62,
       1,     1,    67,    79,    68,    85,   462,    62,     1,     6,
       1,    86,    46,    47,    48,    88,    85,    62,   115,    86,
      86,    74,   525,    68,    67,    69,    62,    67,    69,    73,
      87,   267,    72,    88,    74,    88,    71,    62,   401,   275,
      62,   314,    66,    68,   547,    69,   315,    88,   301,   302,
     303,    67,    69,    91,   277,   291,   149,   135,    69,   137,
     316,   139,   140,   317,   299,   300,    75,    76,   524,   149,
     318,    88,   295,    74,   154,    88,    69,    88,    69,    69,
     149,    71,    88,    88,    88,   154,   164,    88,    88,   167,
     310,   311,   312,   313,    91,    88,   319,    88,   321,   497,
      65,    69,    69,    67,    67,    73,    73,    22,    23,    72,
     230,    74,    22,    23,    79,    80,    62,   185,   186,   187,
      65,    23,    68,    11,    67,    40,    41,    62,   526,   492,
      40,    41,   530,    21,    79,    80,    67,   535,    40,    41,
      67,    72,    70,    74,    69,    72,    34,    74,    73,     3,
       4,     5,    75,    76,   552,    69,    10,    11,   556,    73,
      14,    66,    16,    17,    83,    19,    81,    82,    44,    45,
      24,    81,    82,    36,    37,    38,    39,   575,    69,    81,
      82,    30,    31,   406,    36,    37,    38,    39,    68,    69,
      44,    45,    36,    37,    38,    39,   274,    84,    52,    46,
      47,    48,    90,    68,    69,    43,    60,    61,    62,    63,
      64,    65,   290,    67,    85,     0,    70,    87,    72,    67,
      84,    75,    76,    77,    78,    68,    69,    83,   296,   297,
     298,    43,   455,   331,   457,    68,    69,   460,   388,   389,
     390,   391,    21,     3,     4,     5,    85,   135,    69,   137,
      10,   139,   140,    87,    14,    68,    69,    68,    69,    19,
      68,    69,    68,    69,    68,    69,    68,    69,    62,   157,
      68,    69,    68,    69,    83,   163,   164,   500,    85,   167,
      68,    69,    68,    69,    84,    43,    69,    88,    70,    67,
      66,    62,    52,    67,    67,    67,   184,    62,    66,    66,
      60,    61,    62,   381,    22,    65,    22,    67,    62,    66,
      70,    88,    72,    69,    62,    70,    35,     7,    68,    62,
     208,    90,   210,    87,    68,    88,    44,    45,    66,    31,
      71,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    68,   339,   392,   407,   393,   396,   394,   464,
     280,   444,   284,   446,   103,   448,   395,   565,   520,   541,
     332,    -1,    -1,    -1,   444,    -1,   446,   445,   448,    87,
      -1,    -1,    -1,    -1,    -1,   444,    -1,   446,    -1,   448,
      -1,    -1,    -1,   461,    -1,    -1,   274,    -1,    -1,   277,
      -1,   554,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   290,    -1,    -1,    -1,    -1,   295,    -1,    -1,
      -1,    -1,    -1,    -1,   528,    -1,    -1,    -1,    -1,    -1,
      -1,   584,   585,    -1,   193,   194,   195,   196,   197,   198,
     523,   319,    -1,   321,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   519,    -1,   523,   537,   543,    -1,    -1,   528,    -1,
      -1,   544,    -1,    -1,   523,    -1,    -1,   537,    -1,   528,
      -1,   539,   576,   577,   544,    -1,    -1,    -1,   537,    -1,
      -1,    -1,    -1,    -1,   554,   544,   569,    -1,   571,    -1,
      -1,   579,    -1,    -1,    -1,   554,    -1,    -1,   581,   569,
      -1,   571,    -1,   381,   382,    -1,   576,   577,    -1,    -1,
     569,   581,   571,    -1,   584,   585,    -1,   576,   577,   397,
     398,   399,   581,    -1,    -1,   584,   585,    -1,   406,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,   304,   305,   306,   307,   308,
     309,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   445,    -1,    -1,
      -1,    -1,    87,    -1,    -1,    -1,    -1,   455,    -1,   457,
      -1,    -1,   460,   461,   462,    -1,    -1,   465,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    28,    29,    30,    31,    32,    33,
      -1,    35,   500,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
      -1,   519,    -1,    -1,    -1,    -1,   524,   525,    62,     1,
      -1,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,
      -1,   539,    14,    -1,    16,    17,    -1,    19,    -1,   547,
      -1,    -1,    24,    44,    45,    -1,    -1,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,   462,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    87,    -1,    70,    -1,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,   524,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    71,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    71,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    71,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    71,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    -1,
      72,    -1,    -1,    75,    76,    77,    78,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,    88,     9,    10,    11,
      12,    13,    -1,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    65,    -1,    67,    -1,    -1,    70,    -1,
      72,    -1,    -1,    75,    76,    77,    78,     3,     4,     5,
      -1,    -1,    -1,    -1,    10,    -1,    88,    -1,    14,    -1,
      16,    17,    -1,    19,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,    65,
      -1,    67,    -1,    -1,    70,    -1,    72,    73,    -1,    75,
      76,    77,    78,     3,     4,     5,    -1,    -1,    -1,    -1,
      10,    -1,    -1,    -1,    14,    -1,    16,    17,    -1,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    61,    62,    63,    64,    65,    -1,    67,    68,    -1,
      70,    -1,    72,    -1,    -1,    75,    76,    77,    78,     3,
       4,     5,    -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,
      14,    -1,    16,    17,    -1,    19,    -1,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,
      64,    65,    -1,    67,    -1,    -1,    70,    -1,    72,    73,
      -1,    75,    76,    77,    78,     3,     4,     5,    -1,    -1,
      -1,    -1,    10,    -1,    -1,    -1,    14,    -1,    16,    17,
      -1,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    -1,    -1,
      -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    60,    61,    62,    63,    64,    65,    -1,    67,
      -1,    -1,    70,    -1,    72,    -1,    -1,    75,    76,    77,
      78,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    28,    29,    30,    31,
      32,    33,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    -1,    28,    29,    30,    31,    32,
      33,    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    24,    25,
      26,    28,    29,    32,    33,    44,    45,    52,    60,    61,
      62,    63,    64,    65,    67,    70,    72,    75,    76,    77,
      78,    88,    92,    99,   100,   105,   107,   109,   113,   115,
     116,   118,   120,   122,   124,   127,   130,   133,   136,   139,
     142,   145,   148,   151,   155,   156,   157,   158,   163,   168,
     169,   170,   171,   174,   175,   176,   177,   178,   184,   185,
     186,   187,   189,   190,   200,   201,   202,     1,    62,    88,
      67,    10,    14,    62,    70,    98,    99,   104,   106,   193,
      62,   159,   161,    62,   164,   165,     1,    62,    88,    62,
       1,    88,   104,   106,   108,   112,   114,   116,   117,   119,
     121,   123,   125,   128,   131,   134,   137,   140,   143,   146,
     149,   153,   112,   117,   117,    67,   156,    67,   117,    67,
      67,   153,   157,     1,    88,    62,   188,   117,   117,    66,
     117,   117,   153,    71,   201,    69,   101,   102,   103,   117,
     117,   117,   117,    67,    72,    74,   110,    72,    74,   110,
      44,    45,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    87,   152,    65,    79,    80,    75,    76,
      46,    47,    48,    22,    23,    40,    41,    81,    82,    36,
      37,    38,    39,    83,    84,    85,    43,    42,    86,     1,
      69,    88,     0,   202,     1,    88,    11,   112,   123,   126,
     129,   132,   135,   138,   141,   144,   147,   150,   154,   173,
     104,   106,    62,    67,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    28,    29,
      30,    31,    32,    33,    35,    60,    61,    62,    71,    90,
      91,    93,    94,    97,    72,    74,   110,    87,   166,     1,
      69,    88,   166,     1,    69,    88,     1,    88,    67,   110,
      72,    74,   110,    44,    45,   152,    65,    79,    80,    75,
      76,    46,    47,    48,    22,    23,    40,    41,    81,    82,
      36,    37,    38,    39,    83,    84,    85,    43,    42,    86,
       1,    69,    88,   153,    21,   153,   153,   153,     1,    88,
      30,    31,    87,     1,    74,    88,   156,    68,    71,    69,
      73,    73,   149,    69,    68,   111,   149,   153,    62,    91,
     153,    91,   149,   117,   117,   117,   119,   119,   121,   121,
     121,   123,   123,   123,   123,   123,   123,   125,   125,   125,
     125,   128,   131,   134,   137,   140,   149,   149,    62,   160,
     162,    22,   152,    23,    40,    41,    81,    82,    36,    37,
      38,    39,    83,    84,    85,    43,    42,    86,    69,    88,
     110,    67,    62,    68,   198,    91,    66,    69,    71,   153,
      91,   149,   161,   165,    68,   198,   153,    91,   149,   117,
     117,   117,   119,   119,   121,   121,   121,   123,   123,   123,
     123,   123,   123,   125,   125,   125,   125,   128,   131,   134,
     137,   140,   149,   149,    68,    67,    68,    68,    68,    67,
     157,    62,   188,    62,    79,   102,    68,    69,    73,    73,
      66,    22,    87,   167,    69,    88,   153,   112,   150,   123,
     123,   123,   123,   123,   126,   126,   126,   126,   129,   132,
     135,   138,   141,   150,   150,   153,   172,    68,   198,   194,
      68,    69,    67,   149,    71,    94,    73,   191,    68,    73,
      66,   156,   153,   156,    70,   179,   156,    62,     1,    74,
      88,     1,    88,    73,   149,   149,   149,   153,   150,    22,
      62,   162,   172,    68,    66,    88,   196,    68,    70,   199,
     195,    62,    68,   198,   199,   192,   149,    35,    68,     7,
     180,   181,   182,    68,    68,   153,   167,    88,   156,   150,
     172,   199,   197,    71,   201,   199,    95,    68,   199,   156,
       1,    88,   153,     8,    71,   183,   182,   157,   156,    68,
     172,    68,   199,    71,   199,    96,    66,    66,   180,    31,
     156,    68,   156,   199,   201,   201,    71,   157,   156
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

__attribute__((__unused__))
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
#else
static unsigned
yy_location_print_ (yyo, yylocp)
    FILE *yyo;
    YYLTYPE const * const yylocp;
#endif
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += fprintf (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += fprintf (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += fprintf (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += fprintf (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += fprintf (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;


/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
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
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 192 "grammar.y"
    { (yyval.ident) = new Identifier("break"); }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 193 "grammar.y"
    { (yyval.ident) = new Identifier("case"); }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 194 "grammar.y"
    { (yyval.ident) = new Identifier("default"); }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 195 "grammar.y"
    { (yyval.ident) = new Identifier("for"); }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 196 "grammar.y"
    { (yyval.ident) = new Identifier("new"); }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 197 "grammar.y"
    { (yyval.ident) = new Identifier("var"); }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 198 "grammar.y"
    { (yyval.ident) = new Identifier("const"); }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 199 "grammar.y"
    { (yyval.ident) = new Identifier("continue"); }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 200 "grammar.y"
    { (yyval.ident) = new Identifier("function"); }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 201 "grammar.y"
    { (yyval.ident) = new Identifier("return"); }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 202 "grammar.y"
    { (yyval.ident) = new Identifier("void"); }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 203 "grammar.y"
    { (yyval.ident) = new Identifier("delete"); }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 204 "grammar.y"
    { (yyval.ident) = new Identifier("if"); }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 205 "grammar.y"
    { (yyval.ident) = new Identifier("this"); }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 206 "grammar.y"
    { (yyval.ident) = new Identifier("do"); }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 207 "grammar.y"
    { (yyval.ident) = new Identifier("while"); }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 208 "grammar.y"
    { (yyval.ident) = new Identifier("in"); }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 209 "grammar.y"
    { (yyval.ident) = new Identifier("instanceof"); }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 210 "grammar.y"
    { (yyval.ident) = new Identifier("typeof"); }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 211 "grammar.y"
    { (yyval.ident) = new Identifier("switch"); }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 212 "grammar.y"
    { (yyval.ident) = new Identifier("with"); }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 213 "grammar.y"
    { (yyval.ident) = new Identifier("throw"); }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 214 "grammar.y"
    { (yyval.ident) = new Identifier("try"); }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 215 "grammar.y"
    { (yyval.ident) = new Identifier("catch"); }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 216 "grammar.y"
    { (yyval.ident) = new Identifier("finally"); }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 217 "grammar.y"
    { (yyval.ident) = new Identifier("debugger"); }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 218 "grammar.y"
    { (yyval.ident) = new Identifier("import"); }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 219 "grammar.y"
    { (yyval.ident) = new Identifier("null"); }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 220 "grammar.y"
    { (yyval.ident) = new Identifier("true"); }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 221 "grammar.y"
    { (yyval.ident) = new Identifier("false"); }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 222 "grammar.y"
    { (yyval.ident) = new Identifier("else"); }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 226 "grammar.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].ident); }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 227 "grammar.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].ident); }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 231 "grammar.y"
    { (yyval.node) = new NullNode(); }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 232 "grammar.y"
    { (yyval.node) = new BooleanNode(true); }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 233 "grammar.y"
    { (yyval.node) = new BooleanNode(false); }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 234 "grammar.y"
    { (yyval.node) = new NumberNode((yyvsp[(1) - (1)].dval)); }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 235 "grammar.y"
    { (yyval.node) = new StringNode((yyvsp[(1) - (1)].ustr)); }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 236 "grammar.y"
    {
                                            Lexer& l = lexer();
                                            if (!l.scanRegExp())
                                                YYABORT;
                                            (yyval.node) = new RegExpNode(l.pattern(), l.flags());
                                        }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 242 "grammar.y"
    {
                                            Lexer& l = lexer();
                                            if (!l.scanRegExp())
                                                YYABORT;
                                            (yyval.node) = new RegExpNode("=" + l.pattern(), l.flags());
                                        }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 251 "grammar.y"
    { (yyval.pname) = new PropertyNameNode(*(yyvsp[(1) - (1)].ident)); }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 252 "grammar.y"
    { (yyval.pname) = new PropertyNameNode(Identifier(*(yyvsp[(1) - (1)].ustr))); }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 253 "grammar.y"
    { (yyval.pname) = new PropertyNameNode(Identifier(UString::from((yyvsp[(1) - (1)].dval)))); }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 257 "grammar.y"
    { (yyval.pnode) = new PropertyNode((yyvsp[(1) - (3)].pname), (yyvsp[(3) - (3)].node), PropertyNode::Constant); }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 258 "grammar.y"
    {inFuncExpr();}
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 258 "grammar.y"
    {
          if (!makeGetterOrSetterPropertyNode((yyval.pnode), *(yyvsp[(1) - (6)].ident), *(yyvsp[(2) - (6)].ident), 0, (yyvsp[(6) - (6)].body)))
            YYABORT;
        }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 262 "grammar.y"
    {inFuncExpr();}
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 262 "grammar.y"
    {
          if (!makeGetterOrSetterPropertyNode((yyval.pnode), *(yyvsp[(1) - (7)].ident), *(yyvsp[(2) - (7)].ident), (yyvsp[(4) - (7)].param), (yyvsp[(7) - (7)].body)))
            YYABORT;
        }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 269 "grammar.y"
    { (yyval.plist) = new PropertyListNode((yyvsp[(1) - (1)].pnode)); }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 270 "grammar.y"
    { (yyval.plist) = new PropertyListNode((yyvsp[(3) - (3)].pnode), (yyvsp[(1) - (3)].plist)); }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 275 "grammar.y"
    { (yyval.node) = new ObjectLiteralNode(); }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 276 "grammar.y"
    { (yyval.node) = new ObjectLiteralNode((yyvsp[(2) - (3)].plist)); }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 278 "grammar.y"
    { (yyval.node) = new ObjectLiteralNode((yyvsp[(2) - (4)].plist)); }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 282 "grammar.y"
    { (yyval.node) = new ThisNode(); }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 285 "grammar.y"
    { (yyval.node) = new VarAccessNode(*(yyvsp[(1) - (1)].ident)); }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 286 "grammar.y"
    { (yyval.node) = makeGroupNode((yyvsp[(2) - (3)].node)); }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 291 "grammar.y"
    { (yyval.node) = new ArrayNode((yyvsp[(2) - (3)].ival)); }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 292 "grammar.y"
    { (yyval.node) = new ArrayNode((yyvsp[(2) - (3)].elm)); }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 293 "grammar.y"
    { (yyval.node) = new ArrayNode((yyvsp[(4) - (5)].ival), (yyvsp[(2) - (5)].elm)); }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 297 "grammar.y"
    { (yyval.elm) = new ElementNode((yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].node)); }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 299 "grammar.y"
    { (yyval.elm) = new ElementNode((yyvsp[(1) - (4)].elm), (yyvsp[(3) - (4)].ival), (yyvsp[(4) - (4)].node)); }
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 303 "grammar.y"
    { (yyval.ival) = 0; }
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 308 "grammar.y"
    { (yyval.ival) = 1; }
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 309 "grammar.y"
    { (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1; }
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 314 "grammar.y"
    { (yyval.node) = (yyvsp[(1) - (1)].funcExpr); }
    break;

  case 72:
/* Line 1792 of yacc.c  */
#line 315 "grammar.y"
    { (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); }
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 316 "grammar.y"
    { (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 317 "grammar.y"
    { (yyval.node) = new NewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); }
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 322 "grammar.y"
    { (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); }
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 323 "grammar.y"
    { (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 324 "grammar.y"
    { (yyval.node) = new NewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args)); }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 329 "grammar.y"
    { (yyval.node) = new NewExprNode((yyvsp[(2) - (2)].node)); }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 334 "grammar.y"
    { (yyval.node) = new NewExprNode((yyvsp[(2) - (2)].node)); }
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 338 "grammar.y"
    { (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 339 "grammar.y"
    { (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); }
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 340 "grammar.y"
    { (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); }
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 341 "grammar.y"
    { (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 345 "grammar.y"
    { (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); }
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 346 "grammar.y"
    { (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args)); }
    break;

  case 89:
/* Line 1792 of yacc.c  */
#line 347 "grammar.y"
    { (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node)); }
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 348 "grammar.y"
    { (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 352 "grammar.y"
    { (yyval.args) = new ArgumentsNode(); }
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 353 "grammar.y"
    { (yyval.args) = new ArgumentsNode((yyvsp[(2) - (3)].alist)); }
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 357 "grammar.y"
    { (yyval.alist) = new ArgumentListNode((yyvsp[(1) - (1)].node)); }
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 358 "grammar.y"
    { (yyval.alist) = new ArgumentListNode((yyvsp[(1) - (3)].alist), (yyvsp[(3) - (3)].node)); }
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 373 "grammar.y"
    { (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); }
    break;

  case 101:
/* Line 1792 of yacc.c  */
#line 374 "grammar.y"
    { (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); }
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 379 "grammar.y"
    { (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus); }
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 380 "grammar.y"
    { (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus); }
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 384 "grammar.y"
    { (yyval.node) = makeDeleteNode((yyvsp[(2) - (2)].node)); }
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 385 "grammar.y"
    { (yyval.node) = new VoidNode((yyvsp[(2) - (2)].node)); }
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 386 "grammar.y"
    { (yyval.node) = makeTypeOfNode((yyvsp[(2) - (2)].node)); }
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 387 "grammar.y"
    { (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpPlusPlus); }
    break;

  case 109:
/* Line 1792 of yacc.c  */
#line 388 "grammar.y"
    { (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpPlusPlus); }
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 389 "grammar.y"
    { (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpMinusMinus); }
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 390 "grammar.y"
    { (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpMinusMinus); }
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 391 "grammar.y"
    { (yyval.node) = makeUnaryPlusNode((yyvsp[(2) - (2)].node)); }
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 392 "grammar.y"
    { (yyval.node) = makeNegateNode((yyvsp[(2) - (2)].node)); }
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 393 "grammar.y"
    { (yyval.node) = makeBitwiseNotNode((yyvsp[(2) - (2)].node)); }
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 394 "grammar.y"
    { (yyval.node) = makeLogicalNotNode((yyvsp[(2) - (2)].node)); }
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 408 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMult); }
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 409 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpDiv); }
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 410 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMod); }
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 416 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMult); }
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 418 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpDiv); }
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 420 "grammar.y"
    { (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMod); }
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 425 "grammar.y"
    { (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpPlus); }
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 426 "grammar.y"
    { (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMinus); }
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 432 "grammar.y"
    { (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpPlus); }
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 434 "grammar.y"
    { (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMinus); }
    break;

  case 135:
/* Line 1792 of yacc.c  */
#line 439 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpLShift); }
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 440 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpRShift); }
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 441 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpURShift); }
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 446 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpLShift); }
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 447 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpRShift); }
    break;

  case 141:
/* Line 1792 of yacc.c  */
#line 448 "grammar.y"
    { (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpURShift); }
    break;

  case 143:
/* Line 1792 of yacc.c  */
#line 453 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); }
    break;

  case 144:
/* Line 1792 of yacc.c  */
#line 454 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); }
    break;

  case 145:
/* Line 1792 of yacc.c  */
#line 455 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 146:
/* Line 1792 of yacc.c  */
#line 456 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 147:
/* Line 1792 of yacc.c  */
#line 457 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); }
    break;

  case 148:
/* Line 1792 of yacc.c  */
#line 458 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); }
    break;

  case 150:
/* Line 1792 of yacc.c  */
#line 463 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); }
    break;

  case 151:
/* Line 1792 of yacc.c  */
#line 464 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); }
    break;

  case 152:
/* Line 1792 of yacc.c  */
#line 465 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 153:
/* Line 1792 of yacc.c  */
#line 466 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 154:
/* Line 1792 of yacc.c  */
#line 468 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); }
    break;

  case 156:
/* Line 1792 of yacc.c  */
#line 473 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node)); }
    break;

  case 157:
/* Line 1792 of yacc.c  */
#line 474 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node)); }
    break;

  case 158:
/* Line 1792 of yacc.c  */
#line 475 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 159:
/* Line 1792 of yacc.c  */
#line 476 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 160:
/* Line 1792 of yacc.c  */
#line 478 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node)); }
    break;

  case 161:
/* Line 1792 of yacc.c  */
#line 479 "grammar.y"
    { (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node)); }
    break;

  case 163:
/* Line 1792 of yacc.c  */
#line 484 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 164:
/* Line 1792 of yacc.c  */
#line 485 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 165:
/* Line 1792 of yacc.c  */
#line 486 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 166:
/* Line 1792 of yacc.c  */
#line 487 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));}
    break;

  case 168:
/* Line 1792 of yacc.c  */
#line 493 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 169:
/* Line 1792 of yacc.c  */
#line 495 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 170:
/* Line 1792 of yacc.c  */
#line 497 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 171:
/* Line 1792 of yacc.c  */
#line 499 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));}
    break;

  case 173:
/* Line 1792 of yacc.c  */
#line 505 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 174:
/* Line 1792 of yacc.c  */
#line 506 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 175:
/* Line 1792 of yacc.c  */
#line 508 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node)); }
    break;

  case 176:
/* Line 1792 of yacc.c  */
#line 510 "grammar.y"
    { (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));}
    break;

  case 178:
/* Line 1792 of yacc.c  */
#line 515 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 180:
/* Line 1792 of yacc.c  */
#line 521 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 182:
/* Line 1792 of yacc.c  */
#line 526 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 184:
/* Line 1792 of yacc.c  */
#line 531 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 186:
/* Line 1792 of yacc.c  */
#line 537 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 188:
/* Line 1792 of yacc.c  */
#line 543 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 190:
/* Line 1792 of yacc.c  */
#line 548 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 192:
/* Line 1792 of yacc.c  */
#line 554 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 194:
/* Line 1792 of yacc.c  */
#line 560 "grammar.y"
    { (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 196:
/* Line 1792 of yacc.c  */
#line 565 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 198:
/* Line 1792 of yacc.c  */
#line 571 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 200:
/* Line 1792 of yacc.c  */
#line 577 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node)); }
    break;

  case 202:
/* Line 1792 of yacc.c  */
#line 582 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 204:
/* Line 1792 of yacc.c  */
#line 588 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 206:
/* Line 1792 of yacc.c  */
#line 593 "grammar.y"
    { (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node)); }
    break;

  case 208:
/* Line 1792 of yacc.c  */
#line 599 "grammar.y"
    { (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); }
    break;

  case 210:
/* Line 1792 of yacc.c  */
#line 605 "grammar.y"
    { (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); }
    break;

  case 212:
/* Line 1792 of yacc.c  */
#line 611 "grammar.y"
    { (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node)); }
    break;

  case 214:
/* Line 1792 of yacc.c  */
#line 617 "grammar.y"
    { (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); }
    break;

  case 216:
/* Line 1792 of yacc.c  */
#line 623 "grammar.y"
    { (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); }
    break;

  case 218:
/* Line 1792 of yacc.c  */
#line 629 "grammar.y"
    { (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node)); }
    break;

  case 219:
/* Line 1792 of yacc.c  */
#line 633 "grammar.y"
    { (yyval.op) = OpEqual; }
    break;

  case 220:
/* Line 1792 of yacc.c  */
#line 634 "grammar.y"
    { (yyval.op) = OpPlusEq; }
    break;

  case 221:
/* Line 1792 of yacc.c  */
#line 635 "grammar.y"
    { (yyval.op) = OpMinusEq; }
    break;

  case 222:
/* Line 1792 of yacc.c  */
#line 636 "grammar.y"
    { (yyval.op) = OpMultEq; }
    break;

  case 223:
/* Line 1792 of yacc.c  */
#line 637 "grammar.y"
    { (yyval.op) = OpDivEq; }
    break;

  case 224:
/* Line 1792 of yacc.c  */
#line 638 "grammar.y"
    { (yyval.op) = OpLShift; }
    break;

  case 225:
/* Line 1792 of yacc.c  */
#line 639 "grammar.y"
    { (yyval.op) = OpRShift; }
    break;

  case 226:
/* Line 1792 of yacc.c  */
#line 640 "grammar.y"
    { (yyval.op) = OpURShift; }
    break;

  case 227:
/* Line 1792 of yacc.c  */
#line 641 "grammar.y"
    { (yyval.op) = OpAndEq; }
    break;

  case 228:
/* Line 1792 of yacc.c  */
#line 642 "grammar.y"
    { (yyval.op) = OpXOrEq; }
    break;

  case 229:
/* Line 1792 of yacc.c  */
#line 643 "grammar.y"
    { (yyval.op) = OpOrEq; }
    break;

  case 230:
/* Line 1792 of yacc.c  */
#line 644 "grammar.y"
    { (yyval.op) = OpModEq; }
    break;

  case 232:
/* Line 1792 of yacc.c  */
#line 649 "grammar.y"
    { (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); }
    break;

  case 234:
/* Line 1792 of yacc.c  */
#line 654 "grammar.y"
    { (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); }
    break;

  case 236:
/* Line 1792 of yacc.c  */
#line 659 "grammar.y"
    { (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node)); }
    break;

  case 254:
/* Line 1792 of yacc.c  */
#line 683 "grammar.y"
    { (yyval.stat) = new BlockNode(0); DBG((yyval.stat), (yylsp[(2) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 255:
/* Line 1792 of yacc.c  */
#line 684 "grammar.y"
    { (yyval.stat) = new BlockNode((yyvsp[(2) - (3)].srcs)); DBG((yyval.stat), (yylsp[(3) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 256:
/* Line 1792 of yacc.c  */
#line 688 "grammar.y"
    { (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 257:
/* Line 1792 of yacc.c  */
#line 689 "grammar.y"
    { (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 258:
/* Line 1792 of yacc.c  */
#line 693 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl)); }
    break;

  case 259:
/* Line 1792 of yacc.c  */
#line 695 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl)); }
    break;

  case 260:
/* Line 1792 of yacc.c  */
#line 699 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl)); }
    break;

  case 261:
/* Line 1792 of yacc.c  */
#line 701 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl)); }
    break;

  case 262:
/* Line 1792 of yacc.c  */
#line 705 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), 0, VarDeclNode::Variable); }
    break;

  case 263:
/* Line 1792 of yacc.c  */
#line 706 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Variable); }
    break;

  case 264:
/* Line 1792 of yacc.c  */
#line 710 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), 0, VarDeclNode::Variable); }
    break;

  case 265:
/* Line 1792 of yacc.c  */
#line 711 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Variable); }
    break;

  case 266:
/* Line 1792 of yacc.c  */
#line 715 "grammar.y"
    { (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 267:
/* Line 1792 of yacc.c  */
#line 717 "grammar.y"
    { (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 268:
/* Line 1792 of yacc.c  */
#line 721 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl)); }
    break;

  case 269:
/* Line 1792 of yacc.c  */
#line 723 "grammar.y"
    { (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl)); }
    break;

  case 270:
/* Line 1792 of yacc.c  */
#line 727 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), 0, VarDeclNode::Constant); }
    break;

  case 271:
/* Line 1792 of yacc.c  */
#line 728 "grammar.y"
    { (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Constant); }
    break;

  case 272:
/* Line 1792 of yacc.c  */
#line 732 "grammar.y"
    { (yyval.init) = new AssignExprNode((yyvsp[(2) - (2)].node)); }
    break;

  case 273:
/* Line 1792 of yacc.c  */
#line 736 "grammar.y"
    { (yyval.init) = new AssignExprNode((yyvsp[(2) - (2)].node)); }
    break;

  case 274:
/* Line 1792 of yacc.c  */
#line 740 "grammar.y"
    { (yyval.stat) = new EmptyStatementNode(); }
    break;

  case 275:
/* Line 1792 of yacc.c  */
#line 744 "grammar.y"
    { (yyval.stat) = new ExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 276:
/* Line 1792 of yacc.c  */
#line 745 "grammar.y"
    { (yyval.stat) = new ExprStatementNode((yyvsp[(1) - (2)].node)); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; }
    break;

  case 277:
/* Line 1792 of yacc.c  */
#line 750 "grammar.y"
    { (yyval.stat) = makeIfNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat), 0); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); }
    break;

  case 278:
/* Line 1792 of yacc.c  */
#line 752 "grammar.y"
    { (yyval.stat) = makeIfNode((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].stat), (yyvsp[(7) - (7)].stat)); DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(4) - (7)])); }
    break;

  case 279:
/* Line 1792 of yacc.c  */
#line 756 "grammar.y"
    { (yyval.stat) = new DoWhileNode((yyvsp[(2) - (7)].stat), (yyvsp[(5) - (7)].node)); DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(3) - (7)]));}
    break;

  case 280:
/* Line 1792 of yacc.c  */
#line 757 "grammar.y"
    { (yyval.stat) = new DoWhileNode((yyvsp[(2) - (7)].stat), (yyvsp[(5) - (7)].node)); DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(3) - (7)])); AUTO_SEMICOLON; }
    break;

  case 281:
/* Line 1792 of yacc.c  */
#line 758 "grammar.y"
    { (yyval.stat) = new WhileNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); }
    break;

  case 282:
/* Line 1792 of yacc.c  */
#line 760 "grammar.y"
    { (yyval.stat) = new ForNode((yyvsp[(3) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(7) - (9)].node), (yyvsp[(9) - (9)].stat)); DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(8) - (9)])); }
    break;

  case 283:
/* Line 1792 of yacc.c  */
#line 762 "grammar.y"
    { (yyval.stat) = new ForNode((yyvsp[(4) - (10)].vlist), (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].node), (yyvsp[(10) - (10)].stat)); DBG((yyval.stat), (yylsp[(1) - (10)]), (yylsp[(9) - (10)])); }
    break;

  case 284:
/* Line 1792 of yacc.c  */
#line 764 "grammar.y"
    {
                                            Node *n = (yyvsp[(3) - (7)].node)->nodeInsideAllParens();
                                            if (!n->isLocation())
                                                YYABORT;
                                            (yyval.stat) = new ForInNode(n, (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].stat));
                                            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(6) - (7)]));
                                        }
    break;

  case 285:
/* Line 1792 of yacc.c  */
#line 772 "grammar.y"
    { (yyval.stat) = new ForInNode(*(yyvsp[(4) - (8)].ident), 0, (yyvsp[(6) - (8)].node), (yyvsp[(8) - (8)].stat)); DBG((yyval.stat), (yylsp[(1) - (8)]), (yylsp[(7) - (8)])); }
    break;

  case 286:
/* Line 1792 of yacc.c  */
#line 774 "grammar.y"
    { (yyval.stat) = new ForInNode(*(yyvsp[(4) - (9)].ident), (yyvsp[(5) - (9)].init), (yyvsp[(7) - (9)].node), (yyvsp[(9) - (9)].stat)); DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(8) - (9)])); }
    break;

  case 287:
/* Line 1792 of yacc.c  */
#line 778 "grammar.y"
    { (yyval.node) = 0; }
    break;

  case 289:
/* Line 1792 of yacc.c  */
#line 783 "grammar.y"
    { (yyval.node) = 0; }
    break;

  case 291:
/* Line 1792 of yacc.c  */
#line 788 "grammar.y"
    { (yyval.stat) = new ContinueNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 292:
/* Line 1792 of yacc.c  */
#line 789 "grammar.y"
    { (yyval.stat) = new ContinueNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; }
    break;

  case 293:
/* Line 1792 of yacc.c  */
#line 790 "grammar.y"
    { (yyval.stat) = new ContinueNode(*(yyvsp[(2) - (3)].ident)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 294:
/* Line 1792 of yacc.c  */
#line 791 "grammar.y"
    { (yyval.stat) = new ContinueNode(*(yyvsp[(2) - (3)].ident)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 295:
/* Line 1792 of yacc.c  */
#line 795 "grammar.y"
    { (yyval.stat) = new BreakNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 296:
/* Line 1792 of yacc.c  */
#line 796 "grammar.y"
    { (yyval.stat) = new BreakNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; }
    break;

  case 297:
/* Line 1792 of yacc.c  */
#line 797 "grammar.y"
    { (yyval.stat) = new BreakNode(*(yyvsp[(2) - (3)].ident)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 298:
/* Line 1792 of yacc.c  */
#line 798 "grammar.y"
    { (yyval.stat) = new BreakNode(*(yyvsp[(2) - (3)].ident)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 299:
/* Line 1792 of yacc.c  */
#line 802 "grammar.y"
    { (yyval.stat) = new ReturnNode(0); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 300:
/* Line 1792 of yacc.c  */
#line 803 "grammar.y"
    { (yyval.stat) = new ReturnNode(0); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; }
    break;

  case 301:
/* Line 1792 of yacc.c  */
#line 804 "grammar.y"
    { (yyval.stat) = new ReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 302:
/* Line 1792 of yacc.c  */
#line 805 "grammar.y"
    { (yyval.stat) = new ReturnNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 303:
/* Line 1792 of yacc.c  */
#line 809 "grammar.y"
    { (yyval.stat) = new WithNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); }
    break;

  case 304:
/* Line 1792 of yacc.c  */
#line 813 "grammar.y"
    { (yyval.stat) = new SwitchNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].cblk)); DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)])); }
    break;

  case 305:
/* Line 1792 of yacc.c  */
#line 817 "grammar.y"
    { (yyval.cblk) = new CaseBlockNode((yyvsp[(2) - (3)].clist), 0, 0); }
    break;

  case 306:
/* Line 1792 of yacc.c  */
#line 819 "grammar.y"
    { (yyval.cblk) = new CaseBlockNode((yyvsp[(2) - (5)].clist), (yyvsp[(3) - (5)].ccl), (yyvsp[(4) - (5)].clist)); }
    break;

  case 307:
/* Line 1792 of yacc.c  */
#line 823 "grammar.y"
    { (yyval.clist) = 0; }
    break;

  case 309:
/* Line 1792 of yacc.c  */
#line 828 "grammar.y"
    { (yyval.clist) = new ClauseListNode((yyvsp[(1) - (1)].ccl)); }
    break;

  case 310:
/* Line 1792 of yacc.c  */
#line 829 "grammar.y"
    { (yyval.clist) = new ClauseListNode((yyvsp[(1) - (2)].clist), (yyvsp[(2) - (2)].ccl)); }
    break;

  case 311:
/* Line 1792 of yacc.c  */
#line 833 "grammar.y"
    { (yyval.ccl) = new CaseClauseNode((yyvsp[(2) - (3)].node)); }
    break;

  case 312:
/* Line 1792 of yacc.c  */
#line 834 "grammar.y"
    { (yyval.ccl) = new CaseClauseNode((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].srcs)); }
    break;

  case 313:
/* Line 1792 of yacc.c  */
#line 838 "grammar.y"
    { (yyval.ccl) = new CaseClauseNode(0); }
    break;

  case 314:
/* Line 1792 of yacc.c  */
#line 839 "grammar.y"
    { (yyval.ccl) = new CaseClauseNode(0, (yyvsp[(3) - (3)].srcs)); }
    break;

  case 315:
/* Line 1792 of yacc.c  */
#line 843 "grammar.y"
    { (yyval.stat) = makeLabelNode(*(yyvsp[(1) - (3)].ident), (yyvsp[(3) - (3)].stat)); }
    break;

  case 316:
/* Line 1792 of yacc.c  */
#line 847 "grammar.y"
    { (yyval.stat) = new ThrowNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 317:
/* Line 1792 of yacc.c  */
#line 848 "grammar.y"
    { (yyval.stat) = new ThrowNode((yyvsp[(2) - (3)].node)); DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)])); AUTO_SEMICOLON; }
    break;

  case 318:
/* Line 1792 of yacc.c  */
#line 852 "grammar.y"
    { (yyval.stat) = new TryNode((yyvsp[(2) - (4)].stat), CommonIdentifiers::shared()->nullIdentifier, 0, (yyvsp[(4) - (4)].stat)); DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(2) - (4)])); }
    break;

  case 319:
/* Line 1792 of yacc.c  */
#line 853 "grammar.y"
    { (yyval.stat) = new TryNode((yyvsp[(2) - (7)].stat), *(yyvsp[(5) - (7)].ident), (yyvsp[(7) - (7)].stat), 0); DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(2) - (7)])); }
    break;

  case 320:
/* Line 1792 of yacc.c  */
#line 855 "grammar.y"
    { (yyval.stat) = new TryNode((yyvsp[(2) - (9)].stat), *(yyvsp[(5) - (9)].ident), (yyvsp[(7) - (9)].stat), (yyvsp[(9) - (9)].stat)); DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(2) - (9)])); }
    break;

  case 321:
/* Line 1792 of yacc.c  */
#line 859 "grammar.y"
    { (yyval.stat) = new EmptyStatementNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 322:
/* Line 1792 of yacc.c  */
#line 860 "grammar.y"
    { (yyval.stat) = new EmptyStatementNode(); DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)])); AUTO_SEMICOLON; }
    break;

  case 323:
/* Line 1792 of yacc.c  */
#line 864 "grammar.y"
    { (yyval.pkgn) = new PackageNameNode(*(yyvsp[(1) - (1)].ident)); }
    break;

  case 324:
/* Line 1792 of yacc.c  */
#line 865 "grammar.y"
    { (yyval.pkgn) = new PackageNameNode((yyvsp[(1) - (3)].pkgn), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 325:
/* Line 1792 of yacc.c  */
#line 869 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(2) - (5)].pkgn), true, 0);
                                          DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); }
    break;

  case 326:
/* Line 1792 of yacc.c  */
#line 871 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(2) - (5)].pkgn), true, 0);
                                          DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); AUTO_SEMICOLON; }
    break;

  case 327:
/* Line 1792 of yacc.c  */
#line 873 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(2) - (3)].pkgn), false, 0);
                                          DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 328:
/* Line 1792 of yacc.c  */
#line 875 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(2) - (3)].pkgn), false, 0);
                                          DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); AUTO_SEMICOLON; }
    break;

  case 329:
/* Line 1792 of yacc.c  */
#line 877 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(4) - (5)].pkgn), false, *(yyvsp[(2) - (5)].ident));
                                          DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); }
    break;

  case 330:
/* Line 1792 of yacc.c  */
#line 879 "grammar.y"
    { (yyval.stat) = makeImportNode((yyvsp[(4) - (5)].pkgn), false, *(yyvsp[(2) - (5)].ident));
                                          DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); AUTO_SEMICOLON; }
    break;

  case 331:
/* Line 1792 of yacc.c  */
#line 884 "grammar.y"
    {inFuncDecl();}
    break;

  case 332:
/* Line 1792 of yacc.c  */
#line 884 "grammar.y"
    { (yyval.func) = new FuncDeclNode(*(yyvsp[(2) - (6)].ident), (yyvsp[(6) - (6)].body)); }
    break;

  case 333:
/* Line 1792 of yacc.c  */
#line 885 "grammar.y"
    {inFuncDecl();}
    break;

  case 334:
/* Line 1792 of yacc.c  */
#line 886 "grammar.y"
    { (yyval.func) = new FuncDeclNode(*(yyvsp[(2) - (7)].ident), (yyvsp[(4) - (7)].param), (yyvsp[(7) - (7)].body)); }
    break;

  case 335:
/* Line 1792 of yacc.c  */
#line 890 "grammar.y"
    {inFuncExpr();}
    break;

  case 336:
/* Line 1792 of yacc.c  */
#line 890 "grammar.y"
    {
      (yyval.funcExpr) = new FuncExprNode(CommonIdentifiers::shared()->nullIdentifier, (yyvsp[(5) - (5)].body));
    }
    break;

  case 337:
/* Line 1792 of yacc.c  */
#line 893 "grammar.y"
    {inFuncExpr();}
    break;

  case 338:
/* Line 1792 of yacc.c  */
#line 893 "grammar.y"
    {
      (yyval.funcExpr) = new FuncExprNode(CommonIdentifiers::shared()->nullIdentifier, (yyvsp[(6) - (6)].body), (yyvsp[(3) - (6)].param));
    }
    break;

  case 339:
/* Line 1792 of yacc.c  */
#line 896 "grammar.y"
    {inFuncExpr();}
    break;

  case 340:
/* Line 1792 of yacc.c  */
#line 896 "grammar.y"
    { (yyval.funcExpr) = new FuncExprNode(*(yyvsp[(2) - (6)].ident), (yyvsp[(6) - (6)].body)); }
    break;

  case 341:
/* Line 1792 of yacc.c  */
#line 897 "grammar.y"
    {inFuncExpr();}
    break;

  case 342:
/* Line 1792 of yacc.c  */
#line 897 "grammar.y"
    {
      (yyval.funcExpr) = new FuncExprNode(*(yyvsp[(2) - (7)].ident), (yyvsp[(7) - (7)].body), (yyvsp[(4) - (7)].param));
    }
    break;

  case 343:
/* Line 1792 of yacc.c  */
#line 903 "grammar.y"
    { (yyval.param) = new ParameterNode(*(yyvsp[(1) - (1)].ident)); }
    break;

  case 344:
/* Line 1792 of yacc.c  */
#line 904 "grammar.y"
    { (yyval.param) = new ParameterNode((yyvsp[(1) - (3)].param), *(yyvsp[(3) - (3)].ident)); }
    break;

  case 345:
/* Line 1792 of yacc.c  */
#line 908 "grammar.y"
    { (yyval.body) = new FunctionBodyNode(0); DBG((yyval.body), (yylsp[(1) - (2)]), (yylsp[(2) - (2)])); }
    break;

  case 346:
/* Line 1792 of yacc.c  */
#line 909 "grammar.y"
    { (yyval.body) = new FunctionBodyNode((yyvsp[(2) - (3)].srcs)); DBG((yyval.body), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); }
    break;

  case 347:
/* Line 1792 of yacc.c  */
#line 913 "grammar.y"
    { parser().didFinishParsing(new ProgramNode(0)); }
    break;

  case 348:
/* Line 1792 of yacc.c  */
#line 914 "grammar.y"
    { parser().didFinishParsing(new ProgramNode((yyvsp[(1) - (1)].srcs))); }
    break;

  case 349:
/* Line 1792 of yacc.c  */
#line 918 "grammar.y"
    { (yyval.srcs) = new SourceElementsNode((yyvsp[(1) - (1)].stat)); }
    break;

  case 350:
/* Line 1792 of yacc.c  */
#line 919 "grammar.y"
    { (yyval.srcs) = new SourceElementsNode((yyvsp[(1) - (2)].srcs), (yyvsp[(2) - (2)].stat)); }
    break;

  case 351:
/* Line 1792 of yacc.c  */
#line 923 "grammar.y"
    { (yyval.stat) = (yyvsp[(1) - (1)].func); }
    break;

  case 352:
/* Line 1792 of yacc.c  */
#line 924 "grammar.y"
    { (yyval.stat) = (yyvsp[(1) - (1)].stat); }
    break;


/* Line 1792 of yacc.c  */
#line 4143 "grammar.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 927 "grammar.y"


/* called by yyparse on error */
int yyerror(const char *)
{
// fprintf(stderr, "ERROR: %s at line %d\n", s, KJS::Lexer::curr()->lineNo());
    return 1;
}

/* may we automatically insert a semicolon ? */
static bool allowAutomaticSemicolon()
{
    return yychar == '}' || yychar == 0 || lexer().prevTerminator();
}

// kate: indent-width 2; replace-tabs on; tab-width 4; space-indent on;
