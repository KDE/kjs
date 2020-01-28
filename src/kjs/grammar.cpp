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
 *  Copyright (C) 2019 froglogic GmbH (contact@froglogic.com)
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
#line 138 "grammar.cpp"

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
   by #include "grammar.h".  */
#ifndef YY_KJSYY_GRAMMAR_H_INCLUDED
# define YY_KJSYY_GRAMMAR_H_INCLUDED
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
    T_EXP = 304,
    PLUSEQUAL = 305,
    MINUSEQUAL = 306,
    MULTEQUAL = 307,
    DIVEQUAL = 308,
    EXPEQUAL = 309,
    LSHIFTEQUAL = 310,
    RSHIFTEQUAL = 311,
    URSHIFTEQUAL = 312,
    ANDEQUAL = 313,
    MODEQUAL = 314,
    XOREQUAL = 315,
    OREQUAL = 316,
    NUMBER = 317,
    STRING = 318,
    IDENT = 319,
    AUTOPLUSPLUS = 320,
    AUTOMINUSMINUS = 321
};
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE {
    /* Line 387 of yacc.c  */
#line 63 "grammar.y"

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
#line 277 "grammar.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE {
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
int kjsyyparse(void *YYPARSE_PARAM);
#else
int kjsyyparse();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int kjsyyparse(void);
#else
int kjsyyparse();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_KJSYY_GRAMMAR_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 318 "grammar.cpp"

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
YYID(int yyi)
#else
static int
YYID(yyi)
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
void *malloc(YYSIZE_T);  /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free(void *);  /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
     || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
         && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc {
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)               \
    do                                  \
    {                                 \
        YYSIZE_T yynewbytes;                        \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);            \
        Stack = &yyptr->Stack_alloc;                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);              \
    }                                 \
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
#define YYFINAL  219
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1750

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  91
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  117
/* YYNRULES -- Number of rules.  */
#define YYNRULES  359
/* YYNRULES -- Number of states.  */
#define YYNSTATES  599

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   321

#define YYTRANSLATE(YYX)                        \
    ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] = {
    0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,    80,     2,     2,     2,    82,    85,     2,
    69,    70,    81,    77,    71,    78,    76,    67,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,    68,    90,
    83,    89,    84,    88,     2,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,    74,     2,    75,    86,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    2,     2,     2,    72,    87,    73,    79,     2,     2,     2,
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
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] = {
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
    268,   271,   274,   277,   279,   282,   285,   287,   289,   292,
    295,   297,   300,   303,   306,   309,   312,   315,   318,   320,
    322,   324,   326,   328,   332,   334,   338,   340,   344,   348,
    352,   354,   358,   362,   366,   368,   372,   376,   378,   382,
    386,   388,   392,   396,   400,   402,   406,   410,   414,   416,
    420,   424,   428,   432,   436,   440,   442,   446,   450,   454,
    458,   462,   464,   468,   472,   476,   480,   484,   488,   490,
    494,   498,   502,   506,   508,   512,   516,   520,   524,   526,
    530,   534,   538,   542,   544,   548,   550,   554,   556,   560,
    562,   566,   568,   572,   574,   578,   580,   584,   586,   590,
    592,   596,   598,   602,   604,   608,   610,   614,   616,   620,
    622,   626,   628,   632,   634,   640,   642,   648,   650,   656,
    658,   662,   664,   668,   670,   674,   676,   678,   680,   682,
    684,   686,   688,   690,   692,   694,   696,   698,   700,   702,
    706,   708,   712,   714,   718,   720,   722,   724,   726,   728,
    730,   732,   734,   736,   738,   740,   742,   744,   746,   748,
    750,   752,   755,   759,   763,   767,   769,   773,   775,   779,
    781,   784,   786,   789,   793,   797,   799,   803,   805,   808,
    811,   814,   816,   819,   822,   828,   836,   844,   852,   858,
    868,   879,   887,   896,   906,   907,   909,   910,   912,   915,
    918,   922,   926,   929,   932,   936,   940,   943,   946,   950,
    954,   960,   966,   970,   976,   977,   979,   981,   984,   988,
    993,   996,  1000,  1004,  1008,  1012,  1017,  1025,  1035,  1038,
    1041,  1043,  1047,  1053,  1059,  1063,  1067,  1073,  1079,  1080,
    1087,  1088,  1096,  1097,  1103,  1104,  1111,  1112,  1119,  1120,
    1128,  1130,  1134,  1137,  1141,  1142,  1144,  1146,  1149,  1151
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] = {
    205,     0,    -1,     6,    -1,     7,    -1,     8,    -1,     9,
    -1,    10,    -1,    11,    -1,    12,    -1,    13,    -1,    14,
    -1,    15,    -1,    16,    -1,    17,    -1,    18,    -1,    19,
    -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
    -1,    25,    -1,    26,    -1,    28,    -1,    29,    -1,    30,
    -1,    31,    -1,    32,    -1,    33,    -1,     3,    -1,     4,
    -1,     5,    -1,    35,    -1,    64,    -1,    92,    -1,     3,
    -1,     4,    -1,     5,    -1,    62,    -1,    63,    -1,    67,
    -1,    53,    -1,    93,    -1,    63,    -1,    62,    -1,    95,
    68,   154,    -1,    -1,    64,    93,    69,    70,    97,   204,
    -1,    -1,    64,    93,    69,   203,    70,    98,   204,    -1,
    96,    -1,    99,    71,    96,    -1,   101,    -1,    72,    73,
    -1,    72,    99,    73,    -1,    72,    99,    71,    73,    -1,
    19,    -1,    94,    -1,   102,    -1,    64,    -1,    69,   158,
    70,    -1,    74,   104,    75,    -1,    74,   103,    75,    -1,
    74,   103,    71,   104,    75,    -1,   104,   154,    -1,   103,
    71,   104,   154,    -1,    -1,   105,    -1,    71,    -1,   105,
    71,    -1,   100,    -1,   198,    -1,   106,    74,   158,    75,
    -1,   106,    76,    93,    -1,    10,   106,   112,    -1,   101,
    -1,   107,    74,   158,    75,    -1,   107,    76,    93,    -1,
    10,   106,   112,    -1,   106,    -1,    10,   108,    -1,   107,
    -1,    10,   108,    -1,   106,   112,    -1,   110,   112,    -1,
    110,    74,   158,    75,    -1,   110,    76,    93,    -1,   107,
    112,    -1,   111,   112,    -1,   111,    74,   158,    75,    -1,
    111,    76,    93,    -1,    69,    70,    -1,    69,   113,    70,
    -1,   154,    -1,   113,    71,   154,    -1,   108,    -1,   110,
    -1,   109,    -1,   111,    -1,    44,   120,    -1,    65,   120,
    -1,    45,   120,    -1,    66,   120,    -1,   114,    -1,   114,
    44,    -1,   114,    45,    -1,   116,    -1,   115,    -1,   115,
    44,    -1,   115,    45,    -1,   116,    -1,    17,   120,    -1,
    16,   120,    -1,    24,   120,    -1,    77,   120,    -1,    78,
    120,    -1,    79,   120,    -1,    80,   120,    -1,   117,    -1,
    119,    -1,   118,    -1,   119,    -1,   120,    -1,   117,    49,
    122,    -1,   121,    -1,   118,    49,   122,    -1,   122,    -1,
    124,    81,   122,    -1,   124,    67,   122,    -1,   124,    82,
    122,    -1,   123,    -1,   125,    81,   122,    -1,   125,    67,
    122,    -1,   125,    82,   122,    -1,   124,    -1,   126,    77,
    124,    -1,   126,    78,   124,    -1,   125,    -1,   127,    77,
    124,    -1,   127,    78,   124,    -1,   126,    -1,   128,    46,
    126,    -1,   128,    47,   126,    -1,   128,    48,   126,    -1,
    127,    -1,   129,    46,   126,    -1,   129,    47,   126,    -1,
    129,    48,   126,    -1,   128,    -1,   130,    83,   128,    -1,
    130,    84,   128,    -1,   130,    40,   128,    -1,   130,    41,
    128,    -1,   130,    23,   128,    -1,   130,    22,   128,    -1,
    128,    -1,   131,    83,   128,    -1,   131,    84,   128,    -1,
    131,    40,   128,    -1,   131,    41,   128,    -1,   131,    23,
    128,    -1,   129,    -1,   132,    83,   128,    -1,   132,    84,
    128,    -1,   132,    40,   128,    -1,   132,    41,   128,    -1,
    132,    23,   128,    -1,   132,    22,   128,    -1,   130,    -1,
    133,    36,   130,    -1,   133,    37,   130,    -1,   133,    38,
    130,    -1,   133,    39,   130,    -1,   131,    -1,   134,    36,
    131,    -1,   134,    37,   131,    -1,   134,    38,   131,    -1,
    134,    39,   131,    -1,   132,    -1,   135,    36,   130,    -1,
    135,    37,   130,    -1,   135,    38,   130,    -1,   135,    39,
    130,    -1,   133,    -1,   136,    85,   133,    -1,   134,    -1,
    137,    85,   134,    -1,   135,    -1,   138,    85,   133,    -1,
    136,    -1,   139,    86,   136,    -1,   137,    -1,   140,    86,
    137,    -1,   138,    -1,   141,    86,   136,    -1,   139,    -1,
    142,    87,   139,    -1,   140,    -1,   143,    87,   140,    -1,
    141,    -1,   144,    87,   139,    -1,   142,    -1,   145,    43,
    142,    -1,   143,    -1,   146,    43,   143,    -1,   144,    -1,
    147,    43,   142,    -1,   145,    -1,   148,    42,   145,    -1,
    146,    -1,   149,    42,   146,    -1,   147,    -1,   150,    42,
    145,    -1,   148,    -1,   148,    88,   154,    68,   154,    -1,
    149,    -1,   149,    88,   155,    68,   155,    -1,   150,    -1,
    150,    88,   154,    68,   154,    -1,   151,    -1,   114,   157,
    154,    -1,   152,    -1,   114,   157,   155,    -1,   153,    -1,
    115,   157,   154,    -1,    89,    -1,    50,    -1,    51,    -1,
    52,    -1,    53,    -1,    54,    -1,    55,    -1,    56,    -1,
    57,    -1,    58,    -1,    60,    -1,    61,    -1,    59,    -1,
    154,    -1,   158,    71,   154,    -1,   155,    -1,   159,    71,
    155,    -1,   156,    -1,   160,    71,   154,    -1,   162,    -1,
    163,    -1,   168,    -1,   173,    -1,   174,    -1,   175,    -1,
    176,    -1,   179,    -1,   180,    -1,   181,    -1,   182,    -1,
    183,    -1,   189,    -1,   190,    -1,   191,    -1,   192,    -1,
    194,    -1,    72,    73,    -1,    72,   206,    73,    -1,    11,
    164,    90,    -1,    11,   164,     1,    -1,   166,    -1,   164,
    71,   166,    -1,   167,    -1,   165,    71,   167,    -1,    64,
    -1,    64,   171,    -1,    64,    -1,    64,   172,    -1,    12,
    169,    90,    -1,    12,   169,     1,    -1,   170,    -1,   169,
    71,   170,    -1,    64,    -1,    64,   171,    -1,    89,   154,
    -1,    89,   155,    -1,    90,    -1,   160,    90,    -1,   160,
    1,    -1,    18,    69,   158,    70,   161,    -1,    18,    69,
    158,    70,   161,    35,   161,    -1,    20,   161,    21,    69,
    158,    70,    90,    -1,    20,   161,    21,    69,   158,    70,
    1,    -1,    21,    69,   158,    70,   161,    -1,     9,    69,
    178,    90,   177,    90,   177,    70,   161,    -1,     9,    69,
    11,   165,    90,   177,    90,   177,    70,   161,    -1,     9,
    69,   114,    22,   158,    70,   161,    -1,     9,    69,    11,
    64,    22,   158,    70,   161,    -1,     9,    69,    11,    64,
    172,    22,   158,    70,   161,    -1,    -1,   158,    -1,    -1,
    159,    -1,    13,    90,    -1,    13,     1,    -1,    13,    64,
    90,    -1,    13,    64,     1,    -1,     6,    90,    -1,     6,
    1,    -1,     6,    64,    90,    -1,     6,    64,     1,    -1,
    15,    90,    -1,    15,     1,    -1,    15,   158,    90,    -1,
    15,   158,     1,    -1,    26,    69,   158,    70,   161,    -1,
    25,    69,   158,    70,   184,    -1,    72,   185,    73,    -1,
    72,   185,   188,   185,    73,    -1,    -1,   186,    -1,   187,
    -1,   186,   187,    -1,     7,   158,    68,    -1,     7,   158,
    68,   206,    -1,     8,    68,    -1,     8,    68,   206,    -1,
    64,    68,   161,    -1,    28,   158,    90,    -1,    28,   158,
    1,    -1,    29,   162,    31,   162,    -1,    29,   162,    30,
    69,    64,    70,   162,    -1,    29,   162,    30,    69,    64,
    70,   162,    31,   162,    -1,    32,    90,    -1,    32,     1,
    -1,    64,    -1,   193,    76,    64,    -1,    33,   193,    76,
    81,    90,    -1,    33,   193,    76,    81,     1,    -1,    33,
    193,    90,    -1,    33,   193,     1,    -1,    33,    64,    89,
    193,    90,    -1,    33,    64,    89,   193,     1,    -1,    -1,
    14,    64,    69,    70,   196,   204,    -1,    -1,    14,    64,
    69,   203,    70,   197,   204,    -1,    -1,    14,    69,    70,
    199,   204,    -1,    -1,    14,    69,   203,    70,   200,   204,
    -1,    -1,    14,    64,    69,    70,   201,   204,    -1,    -1,
    14,    64,    69,   203,    70,   202,   204,    -1,    64,    -1,
    203,    71,    64,    -1,    72,    73,    -1,    72,   206,    73,
    -1,    -1,   206,    -1,   207,    -1,   206,   207,    -1,   195,
    -1,   161,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] = {
    0,   194,   194,   195,   196,   197,   198,   199,   200,   201,
    202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
    212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
    222,   223,   224,   228,   229,   233,   234,   235,   236,   237,
    238,   244,   253,   254,   255,   259,   260,   260,   264,   264,
    271,   272,   276,   277,   278,   280,   284,   285,   286,   287,
    288,   293,   294,   295,   299,   300,   305,   306,   310,   311,
    315,   316,   317,   318,   319,   323,   324,   325,   326,   330,
    331,   335,   336,   340,   341,   342,   343,   347,   348,   349,
    350,   354,   355,   359,   360,   364,   365,   369,   370,   374,
    375,   376,   377,   381,   382,   383,   384,   388,   389,   390,
    391,   395,   396,   397,   398,   399,   400,   401,   404,   405,
    409,   410,   414,   415,   419,   420,   424,   425,   426,   427,
    431,   432,   434,   436,   441,   442,   443,   447,   448,   450,
    455,   456,   457,   458,   462,   463,   464,   465,   469,   470,
    471,   472,   473,   474,   475,   479,   480,   481,   482,   483,
    484,   489,   490,   491,   492,   493,   494,   496,   500,   501,
    502,   503,   504,   508,   509,   511,   513,   515,   520,   521,
    523,   524,   526,   531,   532,   536,   537,   542,   543,   547,
    548,   552,   553,   558,   559,   564,   565,   569,   570,   575,
    576,   581,   582,   586,   587,   592,   593,   598,   599,   603,
    604,   609,   610,   614,   615,   620,   621,   626,   627,   632,
    633,   638,   639,   644,   645,   650,   651,   652,   653,   654,
    655,   656,   657,   658,   659,   660,   661,   662,   666,   667,
    671,   672,   676,   677,   681,   682,   683,   684,   685,   686,
    687,   688,   689,   690,   691,   692,   693,   694,   695,   696,
    697,   701,   702,   706,   707,   711,   712,   717,   718,   723,
    724,   728,   729,   733,   734,   739,   740,   745,   746,   750,
    754,   758,   762,   763,   767,   769,   774,   775,   776,   777,
    779,   781,   789,   791,   796,   797,   801,   802,   806,   807,
    808,   809,   813,   814,   815,   816,   820,   821,   822,   823,
    827,   831,   835,   836,   841,   842,   846,   847,   851,   852,
    856,   857,   861,   865,   866,   870,   871,   872,   877,   878,
    882,   883,   887,   889,   891,   893,   895,   897,   902,   902,
    903,   903,   908,   908,   911,   911,   914,   914,   915,   915,
    921,   922,   926,   927,   931,   932,   936,   937,   941,   942
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] = {
    "$end", "error", "$undefined", "NULLTOKEN", "TRUETOKEN", "FALSETOKEN",
    "BREAK", "CASE", "DEFAULT", "FOR", "NEW", "VAR", "CONSTTOKEN",
    "CONTINUE", "FUNCTION", "RETURN", "VOIDTOKEN", "DELETETOKEN", "IF",
    "THISTOKEN", "DO", "WHILE", "INTOKEN", "INSTANCEOF", "TYPEOF", "SWITCH",
    "WITH", "RESERVED", "THROW", "TRY", "CATCH", "FINALLY", "DEBUGGER",
    "IMPORT", "IF_WITHOUT_ELSE", "ELSE", "EQEQ", "NE", "STREQ", "STRNEQ",
    "LE", "GE", "OR", "AND", "PLUSPLUS", "MINUSMINUS", "LSHIFT", "RSHIFT",
    "URSHIFT", "T_EXP", "PLUSEQUAL", "MINUSEQUAL", "MULTEQUAL", "DIVEQUAL",
    "EXPEQUAL", "LSHIFTEQUAL", "RSHIFTEQUAL", "URSHIFTEQUAL", "ANDEQUAL",
    "MODEQUAL", "XOREQUAL", "OREQUAL", "NUMBER", "STRING", "IDENT",
    "AUTOPLUSPLUS", "AUTOMINUSMINUS", "'/'", "':'", "'('", "')'", "','",
    "'{'", "'}'", "'['", "']'", "'.'", "'+'", "'-'", "'~'", "'!'", "'*'",
    "'%'", "'<'", "'>'", "'&'", "'^'", "'|'", "'?'", "'='", "';'", "$accept",
    "Keywords", "IdentifierName", "Literal", "PropertyName", "Property",
    "$@1", "$@2", "PropertyList", "PrimaryExpr", "PrimaryExprNoBrace",
    "ArrayLiteral", "ElementList", "ElisionOpt", "Elision", "MemberExpr",
    "MemberExprNoBF", "NewExpr", "NewExprNoBF", "CallExpr", "CallExprNoBF",
    "Arguments", "ArgumentList", "LeftHandSideExpr", "LeftHandSideExprNoBF",
    "UpdateExprCommon", "UpdateExpr", "UpdateExprNoBF", "UnaryExprCommon",
    "UnaryExpr", "UnaryExprNoBF", "ExponentiationExpr",
    "ExponentiationExprNoBF", "MultiplicativeExpr", "MultiplicativeExprNoBF",
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
static const yytype_uint16 yytoknum[] = {
    0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
    265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
    275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
    285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
    295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
    305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
    315,   316,   317,   318,   319,   320,   321,    47,    58,    40,
    41,    44,   123,   125,    91,    93,    46,    43,    45,   126,
    33,    42,    37,    60,    62,    38,    94,   124,    63,    61,
    59
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] = {
    0,    91,    92,    92,    92,    92,    92,    92,    92,    92,
    92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
    92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
    92,    92,    92,    93,    93,    94,    94,    94,    94,    94,
    94,    94,    95,    95,    95,    96,    97,    96,    98,    96,
    99,    99,   100,   100,   100,   100,   101,   101,   101,   101,
    101,   102,   102,   102,   103,   103,   104,   104,   105,   105,
    106,   106,   106,   106,   106,   107,   107,   107,   107,   108,
    108,   109,   109,   110,   110,   110,   110,   111,   111,   111,
    111,   112,   112,   113,   113,   114,   114,   115,   115,   116,
    116,   116,   116,   117,   117,   117,   117,   118,   118,   118,
    118,   119,   119,   119,   119,   119,   119,   119,   120,   120,
    121,   121,   122,   122,   123,   123,   124,   124,   124,   124,
    125,   125,   125,   125,   126,   126,   126,   127,   127,   127,
    128,   128,   128,   128,   129,   129,   129,   129,   130,   130,
    130,   130,   130,   130,   130,   131,   131,   131,   131,   131,
    131,   132,   132,   132,   132,   132,   132,   132,   133,   133,
    133,   133,   133,   134,   134,   134,   134,   134,   135,   135,
    135,   135,   135,   136,   136,   137,   137,   138,   138,   139,
    139,   140,   140,   141,   141,   142,   142,   143,   143,   144,
    144,   145,   145,   146,   146,   147,   147,   148,   148,   149,
    149,   150,   150,   151,   151,   152,   152,   153,   153,   154,
    154,   155,   155,   156,   156,   157,   157,   157,   157,   157,
    157,   157,   157,   157,   157,   157,   157,   157,   158,   158,
    159,   159,   160,   160,   161,   161,   161,   161,   161,   161,
    161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
    161,   162,   162,   163,   163,   164,   164,   165,   165,   166,
    166,   167,   167,   168,   168,   169,   169,   170,   170,   171,
    172,   173,   174,   174,   175,   175,   176,   176,   176,   176,
    176,   176,   176,   176,   177,   177,   178,   178,   179,   179,
    179,   179,   180,   180,   180,   180,   181,   181,   181,   181,
    182,   183,   184,   184,   185,   185,   186,   186,   187,   187,
    188,   188,   189,   190,   190,   191,   191,   191,   192,   192,
    193,   193,   194,   194,   194,   194,   194,   194,   196,   195,
    197,   195,   199,   198,   200,   198,   201,   198,   202,   198,
    203,   203,   204,   204,   205,   205,   206,   206,   207,   207
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] = {
    0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     3,     0,     6,     0,     7,
    1,     3,     1,     2,     3,     4,     1,     1,     1,     1,
    3,     3,     3,     5,     2,     4,     0,     1,     1,     2,
    1,     1,     4,     3,     3,     1,     4,     3,     3,     1,
    2,     1,     2,     2,     2,     4,     3,     2,     2,     4,
    3,     2,     3,     1,     3,     1,     1,     1,     1,     2,
    2,     2,     2,     1,     2,     2,     1,     1,     2,     2,
    1,     2,     2,     2,     2,     2,     2,     2,     1,     1,
    1,     1,     1,     3,     1,     3,     1,     3,     3,     3,
    1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
    1,     3,     3,     3,     1,     3,     3,     3,     1,     3,
    3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
    3,     1,     3,     3,     3,     3,     3,     3,     1,     3,
    3,     3,     3,     1,     3,     3,     3,     3,     1,     3,
    3,     3,     3,     1,     3,     1,     3,     1,     3,     1,
    3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
    3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
    3,     1,     3,     1,     5,     1,     5,     1,     5,     1,
    3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
    1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
    1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
    1,     2,     3,     3,     3,     1,     3,     1,     3,     1,
    2,     1,     2,     3,     3,     1,     3,     1,     2,     2,
    2,     1,     2,     2,     5,     7,     7,     7,     5,     9,
    10,     7,     8,     9,     0,     1,     0,     1,     2,     2,
    3,     3,     2,     2,     3,     3,     2,     2,     3,     3,
    5,     5,     3,     5,     0,     1,     1,     2,     3,     4,
    2,     3,     3,     3,     3,     4,     7,     9,     2,     2,
    1,     3,     5,     5,     3,     3,     5,     5,     0,     6,
    0,     7,     0,     5,     0,     6,     0,     6,     0,     7,
    1,     3,     2,     3,     0,     1,     1,     2,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] = {
    354,    35,    36,    37,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,    56,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,    41,    38,    39,
    59,     0,     0,    40,     0,     0,    66,     0,     0,     0,
    0,   281,    57,    75,    58,    81,    97,    98,   107,   110,
    120,   121,   124,   130,   137,   144,   161,   178,   187,   193,
    199,   205,   211,   217,   223,   242,     0,   359,   244,   245,
    246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
    256,   257,   258,   259,   260,   358,     0,   355,   356,   303,
    0,   302,   296,     0,     0,    59,     0,    70,    52,    79,
    82,    71,   269,     0,   265,   277,     0,   275,   299,     0,
    298,     0,   307,   306,    79,    95,    96,   103,   106,   118,
    119,   122,   126,   134,   140,   148,   168,   183,   189,   195,
    201,   207,   213,   219,   238,     0,   103,   118,   112,   111,
    0,     0,     0,   113,     0,     0,     0,     0,   329,   328,
    330,     0,    99,   101,     0,   100,   102,     0,   261,     0,
    68,     0,     0,    67,   114,   115,   116,   117,     0,     0,
    0,    87,     0,     0,    88,   108,   109,   226,   227,   228,
    229,   230,   231,   232,   233,   234,   237,   235,   236,   225,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,   283,     0,   282,     1,
    357,   305,   304,     0,   103,   155,   173,   185,   191,   197,
    203,   209,   215,   221,   240,   297,     0,    79,    80,     0,
    0,    29,    30,    31,     2,     3,     4,     5,     6,     7,
    8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    32,    44,    43,    33,    53,    34,    42,     0,    50,
    0,     0,     0,    78,     0,   270,   264,     0,   263,   278,
    274,     0,   273,   301,   300,     0,    83,     0,     0,    84,
    104,   105,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,   309,     0,
    308,     0,     0,     0,     0,     0,   324,   323,     0,     0,
    0,   335,     0,   334,   322,    60,   262,    66,    62,    61,
    64,    69,    91,     0,    93,     0,    33,    77,     0,    90,
    224,   125,   132,   131,   133,   138,   139,   145,   146,   147,
    167,   166,   164,   165,   162,   163,   179,   180,   181,   182,
    188,   194,   200,   206,   212,     0,   243,   271,     0,   267,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,   294,    74,
    0,   350,   342,     0,     0,     0,     0,    54,     0,    73,
    279,   266,   276,   338,     0,     0,    86,   220,   123,   128,
    127,   129,   135,   136,   141,   142,   143,   154,   153,   151,
    152,   149,   150,   169,   170,   171,   172,   184,   190,   196,
    202,   208,     0,   239,     0,     0,     0,     0,     0,     0,
    325,   330,     0,   331,     0,     0,    92,     0,    76,    89,
    0,     0,     0,   272,     0,   294,     0,   103,   222,   160,
    158,   159,   156,   157,   174,   175,   176,   177,   186,   192,
    198,   204,   210,     0,   241,   295,     0,   346,     0,     0,
    344,     0,     0,    45,    55,    51,    72,     0,   340,    85,
    0,   284,     0,   288,   314,   311,   310,     0,   337,     0,
    336,   333,   332,    63,    65,    94,   218,     0,   280,     0,
    271,   268,     0,     0,     0,   294,     0,   348,     0,   343,
    0,   351,    46,     0,   339,     0,   214,     0,     0,     0,
    0,   315,   316,     0,     0,     0,   272,   294,   291,   216,
    0,   347,     0,   352,     0,   345,     0,    48,   341,   285,
    287,   286,     0,     0,   312,   314,   317,   326,   292,     0,
    0,     0,   349,   353,    47,     0,   318,   320,     0,     0,
    293,     0,   289,    49,   319,   321,   313,   327,   290
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] = {
    -1,   276,   277,    42,   278,   279,   566,   585,   280,    97,
        98,    44,   161,   162,   163,   114,    45,   115,    46,   116,
        47,   171,   353,   136,    48,   118,   119,    50,   120,   121,
        52,   122,    53,   123,    54,   124,    55,   125,    56,   126,
        226,    57,   127,   227,    58,   128,   228,    59,   129,   229,
        60,   130,   230,    61,   131,   231,    62,   132,   232,    63,
        133,   233,    64,   134,   234,    65,   391,   495,   235,    66,
        67,    68,    69,   103,   388,   104,   389,    70,   106,   107,
        285,   473,    71,    72,    73,    74,   496,   236,    75,    76,
        77,    78,    79,   515,   550,   551,   552,   575,    80,    81,
        82,    83,   151,    84,    85,   507,   545,   101,   499,   540,
        536,   562,   413,   539,    86,    87,    88
    };

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -377
static const yytype_int16 yypact[] = {
    994,  -377,  -377,  -377,     7,   -22,   197,     3,     5,    20,
    13,   354,  1466,  1466,    16,  -377,  1076,    70,  1466,    86,
    112,  1466,     9,    58,    63,  1466,  1466,  -377,  -377,  -377,
    118,  1466,  1466,  -377,  1466,   636,   123,  1466,  1466,  1466,
    1466,  -377,  -377,  -377,  -377,    -4,  -377,    32,  1135,  -377,
    159,  -377,  -377,  -377,   117,   156,   230,   234,   215,   129,
    157,   178,   195,    -9,  -377,  -377,    21,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,   267,   994,  -377,  -377,
    59,  -377,  1154,   197,    36,  -377,  1544,  -377,  -377,   113,
    -377,  -377,   179,    24,  -377,   179,    33,  -377,  -377,    60,
    -377,   201,  -377,  -377,   113,  -377,   116,  1213,  -377,   223,
    -377,  -377,  -377,   139,   163,   242,   365,   248,   209,   214,
    229,   279,    -5,  -377,  -377,    85,   186,  -377,  -377,  -377,
    1466,   321,  1466,  -377,  1466,  1466,   101,   232,  -377,  -377,
    271,    27,  -377,  -377,  1076,  -377,  -377,   211,  -377,   748,
    -377,   122,  1232,   285,  -377,  -377,  -377,  -377,  1310,  1466,
    1686,  -377,  1466,  1686,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,
    1466,  1466,  1466,  1466,  1466,  1466,  -377,  1466,  -377,  -377,
    -377,  -377,  -377,   298,   251,   242,   196,   311,   282,   283,
    294,   329,     6,  -377,  -377,   320,   292,   113,  -377,   339,
    12,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,  1686,  -377,  -377,  -377,   341,  -377,
    144,  1466,  1686,  -377,  1466,  -377,  -377,     3,  -377,  -377,
    -377,     5,  -377,  -377,  -377,    45,  -377,  1466,  1686,  -377,
    -377,  -377,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  -377,  1466,
    -377,   222,   343,   227,   244,   249,  -377,  -377,   344,     9,
    346,  -377,    -1,  -377,  -377,  -377,  -377,   123,  -377,  -377,
    -377,  -377,  -377,   274,  -377,   151,  -377,  -377,   152,  -377,
    -377,  -377,  -377,  -377,  -377,   139,   139,   163,   163,   163,
    242,   242,   242,   242,   242,   242,   365,   365,   365,   365,
    248,   209,   214,   229,   279,   347,  -377,    10,   -28,  -377,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,
    1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  1466,  -377,
    48,  -377,  -377,   304,   345,  1466,  1615,  -377,   153,  -377,
    -377,  -377,  -377,  -377,   306,   154,  -377,  -377,  -377,  -377,
    -377,  -377,   139,   139,   163,   163,   163,   242,   242,   242,
    242,   242,   242,   365,   365,   365,   365,   248,   209,   214,
    229,   279,   356,  -377,  1076,  1466,  1076,   350,  1076,   361,
    -377,  -377,    67,  -377,    87,  1388,  -377,  1466,  -377,  -377,
    1466,  1466,  1466,   405,   366,  1466,   309,  1213,  -377,   242,
    242,   242,   242,   242,   196,   196,   196,   196,   311,   282,
    283,   294,   329,   367,  -377,   358,   349,  -377,   314,   368,
    -377,   373,   140,  -377,  -377,  -377,  -377,   368,  -377,  -377,
    1466,   407,   324,  -377,   439,  -377,  -377,   377,  -377,   386,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,   326,  -377,  1466,
    362,  -377,   363,  1076,  1466,  1466,   368,  -377,   830,  -377,
    368,  -377,  -377,   330,  -377,   368,  -377,  1076,    89,  1466,
    34,   439,  -377,     9,  1076,   332,  -377,  1466,  -377,  -377,
    382,  -377,   368,  -377,   912,  -377,   368,  -377,  -377,  -377,
    -377,  -377,    57,   387,  -377,   439,  -377,   428,  -377,  1076,
    390,  1076,  -377,  -377,  -377,   368,   994,   994,   388,     9,
    -377,  1076,  -377,  -377,   994,   994,  -377,  -377,  -377
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] = {
    -377,  -377,  -153,  -377,  -377,    46,  -377,  -377,  -377,  -377,
        0,  -377,  -377,   121,  -377,    47,  -377,    83,  -377,  -377,
        -377,   -41,  -377,   221,  -377,    11,   707,  -377,    29,  1305,
        -377,  -142,  -377,  -177,  -377,  -158,  -377,   -68,  -377,  -152,
        -46,  -377,  -203,    62,  -377,  -201,    68,  -377,  -200,    61,
        -377,  -199,    71,  -377,  -188,    66,  -377,  -377,  -377,  -377,
        -377,  -377,  -377,  -124,  -376,  -377,   -39,     2,  -377,  -377,
        -13,   -18,  -377,  -377,  -377,   185,     4,  -377,  -377,   183,
        371,   -53,  -377,  -377,  -377,  -377,  -362,  -377,  -377,  -377,
        -377,  -377,  -377,  -377,   -95,  -377,   -70,  -377,  -377,  -377,
        -377,  -377,   142,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
        -377,  -377,  -290,  -327,  -377,   -34,   -85
    };

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] = {
    43,   159,   220,   141,   147,   424,   174,   380,    89,   190,
    381,    49,   382,   135,   383,   478,    43,   357,   365,   366,
    359,   108,   216,   146,   225,   286,   384,    49,   341,    51,
    493,   494,   471,   214,   290,    43,   157,   326,   350,   367,
    368,   369,   573,   474,   354,    51,    49,    92,   405,   361,
    362,   363,   364,    99,   376,   377,   378,   379,   283,   148,
    221,   293,   475,   463,    51,   168,   360,   102,   518,   105,
    169,    90,   170,   296,   220,   299,   411,   111,   302,   215,
    464,    35,   412,   327,   109,   140,   328,    43,   521,   100,
    570,   385,   217,   386,   406,   287,   528,    91,    49,   472,
    239,   168,   336,   342,   291,   240,   172,   574,   173,   411,
    110,   218,   411,   532,   288,   423,    51,   343,   497,   447,
    498,   414,   448,   292,   449,   586,   450,   150,   329,   419,
    432,   433,   370,   371,   372,   373,   374,   375,   451,   142,
    237,   344,   331,   519,   333,   426,   334,   335,   149,   222,
    294,   434,   435,   436,    43,   144,   329,   520,   559,    43,
    420,   428,   429,   430,   431,    49,   443,   444,   445,   446,
    49,   355,   329,   560,   358,   330,   238,   522,   427,   571,
    544,   145,   168,    51,   192,   168,   154,   281,    51,   282,
    297,   337,   298,   347,   160,   580,   409,   348,   193,   194,
    1,     2,     3,   452,   411,   453,   304,    93,   191,   561,
    542,    94,   543,   565,   210,   416,    15,   417,   568,   392,
    305,   306,   329,   329,   329,   329,   468,   469,   506,   509,
    300,   301,   117,   195,   196,   582,   393,   394,   213,   584,
    307,   308,   117,   211,   437,   438,   439,   440,   441,   442,
    27,   206,   207,   208,   209,   117,   200,   201,   593,    28,
    29,    95,   338,   339,    33,   212,    34,   219,   284,    96,
    295,    36,   303,   390,   202,   203,   197,   198,   199,   395,
    396,   345,   329,   418,   318,   319,   320,   321,   309,   310,
    311,   503,   454,   329,   322,   300,   301,   456,   329,   425,
    323,   177,   178,   179,   180,   181,   182,   183,   184,   185,
    186,   187,   188,   224,   457,   329,   324,   204,   205,   458,
    329,   460,   325,   225,   479,   480,   481,   482,   483,   225,
    225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
    189,   524,   332,   525,   466,   467,   526,   397,   398,   399,
    400,   484,   485,   486,   487,   112,   351,     1,     2,     3,
    340,   117,   387,   117,    93,   117,   117,   401,    94,   402,
    12,    13,   404,    15,   500,   501,   508,   501,    18,   533,
    329,   403,   408,   117,   537,   501,   546,   312,   313,   117,
    117,   407,   476,   117,   548,   329,   554,   329,    25,    26,
    567,   501,   579,   329,   225,   314,   315,    27,   410,   415,
    461,   117,   455,   459,   502,   470,    28,    29,    95,    31,
    32,    33,   514,    34,   510,   517,    96,   529,    36,   329,
    530,    37,    38,    39,    40,   534,   117,   541,   117,   535,
    538,   511,   547,   513,   113,   516,   549,   553,   316,   317,
    463,   472,   581,   557,    43,   587,    43,   512,    43,   589,
    591,   596,   505,   488,   490,    49,   225,    49,   465,    49,
    489,   492,   421,   527,   422,   491,   289,   556,   531,   220,
    588,   576,   462,    51,     0,    51,     0,    51,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,   117,     0,   564,   117,     0,     0,     0,   220,
    220,     0,     0,     0,     0,     0,     0,     0,   117,     0,
    558,     0,     0,   117,     0,     0,     0,     0,     0,     0,
    0,   555,     0,    43,   569,   577,     0,     0,    43,     0,
    0,   578,     0,     0,    49,     0,     0,    43,   117,    49,
    117,   572,   594,   595,    43,     0,     0,     0,    49,     0,
    0,     0,    51,     0,    43,    49,   590,    51,   592,     0,
    0,   597,     0,     0,     0,    49,    51,     0,   598,    43,
    0,    43,     0,    51,     0,     0,    43,    43,     0,     0,
    49,    43,    49,    51,    43,    43,     0,    49,    49,     0,
    0,     0,    49,     0,     0,    49,    49,     0,    51,     0,
    51,   117,   477,     0,     0,    51,    51,     0,     0,     0,
    51,     0,     0,    51,    51,     0,     0,   477,   477,   117,
    0,     0,     0,     0,     0,     0,   117,     0,     0,     1,
    2,     3,     4,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,     0,     0,
    18,    19,    20,     0,    21,    22,     0,     0,    23,    24,
    0,     0,     0,     0,     0,     0,   117,     0,     0,     0,
    25,    26,     0,     0,     0,     0,   117,     0,   117,    27,
    0,   117,   117,   477,     0,     0,   117,     0,    28,    29,
    30,    31,    32,    33,     0,    34,     0,     0,    35,   158,
    36,     0,     0,    37,    38,    39,    40,     0,     0,   137,
    137,     0,     0,     0,     0,   137,    41,     0,     0,     0,
    0,   117,   137,   137,     0,     0,     0,     0,   137,   137,
    0,     0,     0,     0,   137,   137,   137,   137,     0,     0,
    117,     1,     2,     3,     4,   477,   117,     5,     6,     7,
    8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    117,     0,    18,    19,    20,     0,    21,    22,   117,     0,
    23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
    0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
    28,    29,    30,    31,    32,    33,     0,    34,     0,     0,
    35,   346,    36,     0,     0,    37,    38,    39,    40,     0,
    0,     0,     0,     1,     2,     3,     4,     0,    41,     5,
    6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,     0,     0,    18,    19,    20,     0,    21,    22,
    0,     0,    23,    24,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,    25,    26,     0,     0,     0,     0,
    0,     0,     0,    27,     0,     0,     0,     0,     0,     0,
    0,     0,    28,    29,    30,    31,    32,    33,     0,    34,
    0,     0,    35,   563,    36,     0,     0,    37,    38,    39,
    40,     0,     0,     0,     0,     1,     2,     3,     4,     0,
    41,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,     0,     0,    18,    19,    20,     0,
    21,    22,     0,     0,    23,    24,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,    25,    26,     0,     0,
    0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
    0,     0,     0,     0,    28,    29,    30,    31,    32,    33,
    0,    34,     0,     0,    35,   583,    36,     0,     0,    37,
    38,    39,    40,     0,     0,     0,     0,     1,     2,     3,
    4,     0,    41,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,     0,     0,    18,    19,
    20,     0,    21,    22,     0,     0,    23,    24,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,    25,    26,
    0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
    0,     0,     0,     0,     0,     0,    28,    29,    30,    31,
    32,    33,     0,    34,     0,     0,    35,     0,    36,     0,
    0,    37,    38,    39,    40,     0,     0,     0,     0,     1,
    2,     3,     4,     0,    41,     5,     6,     7,     8,     9,
    0,    11,    12,    13,    14,    15,    16,    17,     0,     0,
    18,    19,    20,     0,    21,    22,     0,     0,    23,    24,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    25,    26,     0,     0,     0,     0,     0,     0,     0,    27,
    0,     0,     0,     0,     0,     0,     0,     0,    28,    29,
    30,    31,    32,    33,     0,    34,     0,     0,    35,     0,
    36,     0,     0,    37,    38,    39,    40,     1,     2,     3,
    0,     0,     0,     0,    93,   223,    41,     0,    94,     0,
    12,    13,     0,    15,     0,     0,     0,     0,    18,   175,
    176,     0,     0,     0,     0,   177,   178,   179,   180,   181,
    182,   183,   184,   185,   186,   187,   188,     0,    25,    26,
    0,     0,     0,     0,     0,     0,     0,    27,     0,     0,
    0,     0,     0,     0,     0,     0,    28,    29,    95,    31,
    32,    33,     0,    34,   189,     0,    96,     0,    36,     0,
    0,    37,    38,    39,    40,     1,     2,     3,     0,     0,
    0,     0,    93,     0,     0,     0,    94,     0,    12,    13,
    0,    15,     0,     0,     0,     0,    18,   300,   301,     0,
    0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
    184,   185,   186,   187,   188,     0,    25,    26,     0,     0,
    0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
    0,     0,     0,     0,    28,    29,    95,    31,    32,    33,
    0,    34,   189,     0,    96,     0,    36,   349,     0,    37,
    38,    39,    40,     1,     2,     3,     0,   138,   139,     0,
    93,     0,     0,   143,    94,     0,    12,    13,     0,    15,
    152,   153,     0,     0,    18,     0,   155,   156,     0,     0,
    0,     0,   164,   165,   166,   167,     0,     0,     0,     0,
    0,     0,     0,     0,    25,    26,     0,     0,     0,     0,
    0,     0,     0,    27,     0,     0,     0,     0,     0,     0,
    0,     0,    28,    29,    95,    31,    32,    33,     0,    34,
    352,     0,    96,     0,    36,     0,     0,    37,    38,    39,
    40,     1,     2,     3,     0,     0,     0,     0,    93,     0,
    0,     0,    94,     0,    12,    13,     0,    15,     0,     0,
    0,     0,    18,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
    0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
    28,    29,    95,    31,    32,    33,     0,    34,     0,     0,
    96,     0,    36,   523,     0,    37,    38,    39,    40,     1,
    2,     3,     0,     0,     0,     0,    93,     0,     0,     0,
    94,     0,    12,    13,     0,    15,     0,     0,     0,     0,
    18,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    25,    26,     0,     0,     0,     0,     0,     0,     0,    27,
    0,     0,     0,     0,     0,     0,     0,     0,    28,    29,
    95,    31,    32,    33,     0,    34,     0,     0,    96,     0,
    36,     0,     0,    37,    38,    39,    40,   241,   242,   243,
    244,   245,   246,   247,   248,   249,   250,   251,   252,   253,
    254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
    264,     0,   265,   266,   267,   268,   269,   270,     0,   271,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,   272,   273,   274,     0,
    0,     0,     0,     0,     0,     0,     0,   275,   241,   242,
    243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
    253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
    263,   264,     0,   265,   266,   267,   268,   269,   270,     0,
    271,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,   272,   273,   274,
    0,     0,     0,     0,     0,     0,     0,     0,   504,   241,
    242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
    252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
    262,   263,   264,     0,   265,   266,   267,   268,   269,   270,
    0,   271,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    356
};

#define yypact_value_is_default(Yystate) \
    (!!((Yystate) == (-377)))

#define yytable_value_is_error(Yytable_value) \
    YYID (0)

static const yytype_int16 yycheck[] = {
    0,    35,    87,    16,    22,   295,    47,   210,     1,    48,
    211,     0,   212,    11,   213,   391,    16,   170,   195,   196,
    173,     1,     1,    21,    92,     1,   214,    16,     1,     0,
    406,   407,    22,    42,     1,    35,    34,    42,   162,   197,
    198,   199,     8,    71,   168,    16,    35,    69,    42,   191,
    192,   193,   194,     6,   206,   207,   208,   209,    99,     1,
    1,     1,    90,    64,    35,    69,   190,    64,     1,    64,
    74,    64,    76,   114,   159,   116,    64,    64,   117,    88,
    81,    72,    70,    88,    64,    69,     1,    87,     1,     6,
    1,   215,    71,   217,    88,    71,   472,    90,    87,    89,
    64,    69,     1,    76,    71,    69,    74,    73,    76,    64,
    90,    90,    64,   475,    90,    70,    87,    90,    70,   322,
    410,   274,   323,    90,   324,    68,   325,    64,    71,   282,
    307,   308,   200,   201,   202,   203,   204,   205,   326,    69,
    93,   154,   140,    76,   142,   298,   144,   145,    90,    90,
    90,   309,   310,   311,   154,    69,    71,    90,   534,   159,
    284,   303,   304,   305,   306,   154,   318,   319,   320,   321,
    159,   169,    71,   535,   172,    90,    93,    90,   302,    90,
    507,    69,    69,   154,    67,    69,    68,    74,   159,    76,
    74,    90,    76,    71,    71,   557,   237,    75,    81,    82,
    3,     4,     5,   327,    64,   329,    67,    10,    49,   536,
    70,    14,   502,   540,    85,    71,    19,    73,   545,    23,
    81,    82,    71,    71,    71,    71,    75,    75,    75,    75,
    44,    45,    11,    77,    78,   562,    40,    41,    43,   566,
    77,    78,    21,    86,   312,   313,   314,   315,   316,   317,
    53,    36,    37,    38,    39,    34,    22,    23,   585,    62,
    63,    64,    30,    31,    67,    87,    69,     0,    89,    72,
    69,    74,    49,    22,    40,    41,    46,    47,    48,    83,
    84,    70,    71,   281,    36,    37,    38,    39,    46,    47,
    48,   415,    70,    71,    85,    44,    45,    70,    71,   297,
    86,    50,    51,    52,    53,    54,    55,    56,    57,    58,
    59,    60,    61,    92,    70,    71,    87,    83,    84,    70,
    71,   339,    43,   391,   392,   393,   394,   395,   396,   397,
    398,   399,   400,   401,   402,   403,   404,   405,   406,   407,
    89,   465,    21,   467,    70,    71,   470,    36,    37,    38,
    39,   397,   398,   399,   400,     1,    71,     3,     4,     5,
    89,   140,    64,   142,    10,   144,   145,    85,    14,    86,
    16,    17,    43,    19,    70,    71,    70,    71,    24,    70,
    71,    87,    90,   162,    70,    71,   510,    22,    23,   168,
    169,    71,   390,   172,    70,    71,    70,    71,    44,    45,
    70,    71,    70,    71,   472,    40,    41,    53,    69,    68,
    64,   190,    69,    69,    69,    68,    62,    63,    64,    65,
    66,    67,    72,    69,    68,    64,    72,    22,    74,    71,
    64,    77,    78,    79,    80,    68,   215,    64,   217,    90,
    72,   454,    35,   456,    90,   458,     7,    70,    83,    84,
    64,    89,    70,    90,   454,    68,   456,   455,   458,    31,
    70,    73,   416,   401,   403,   454,   534,   456,   347,   458,
    402,   405,   287,   471,   291,   404,   105,   530,   474,   564,
    575,   551,   340,   454,    -1,   456,    -1,   458,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   281,    -1,   538,   284,    -1,    -1,    -1,   594,
    595,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   297,    -1,
    533,    -1,    -1,   302,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,   529,    -1,   533,   547,   553,    -1,    -1,   538,    -1,
    -1,   554,    -1,    -1,   533,    -1,    -1,   547,   327,   538,
    329,   549,   586,   587,   554,    -1,    -1,    -1,   547,    -1,
    -1,    -1,   533,    -1,   564,   554,   579,   538,   581,    -1,
    -1,   589,    -1,    -1,    -1,   564,   547,    -1,   591,   579,
    -1,   581,    -1,   554,    -1,    -1,   586,   587,    -1,    -1,
    579,   591,   581,   564,   594,   595,    -1,   586,   587,    -1,
    -1,    -1,   591,    -1,    -1,   594,   595,    -1,   579,    -1,
    581,   390,   391,    -1,    -1,   586,   587,    -1,    -1,    -1,
    591,    -1,    -1,   594,   595,    -1,    -1,   406,   407,   408,
    -1,    -1,    -1,    -1,    -1,    -1,   415,    -1,    -1,     3,
    4,     5,     6,    -1,    -1,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
    24,    25,    26,    -1,    28,    29,    -1,    -1,    32,    33,
    -1,    -1,    -1,    -1,    -1,    -1,   455,    -1,    -1,    -1,
    44,    45,    -1,    -1,    -1,    -1,   465,    -1,   467,    53,
    -1,   470,   471,   472,    -1,    -1,   475,    -1,    62,    63,
    64,    65,    66,    67,    -1,    69,    -1,    -1,    72,    73,
    74,    -1,    -1,    77,    78,    79,    80,    -1,    -1,    12,
    13,    -1,    -1,    -1,    -1,    18,    90,    -1,    -1,    -1,
    -1,   510,    25,    26,    -1,    -1,    -1,    -1,    31,    32,
    -1,    -1,    -1,    -1,    37,    38,    39,    40,    -1,    -1,
    529,     3,     4,     5,     6,   534,   535,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    549,    -1,    24,    25,    26,    -1,    28,    29,   557,    -1,
    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    62,    63,    64,    65,    66,    67,    -1,    69,    -1,    -1,
    72,    73,    74,    -1,    -1,    77,    78,    79,    80,    -1,
    -1,    -1,    -1,     3,     4,     5,     6,    -1,    90,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    -1,    -1,    24,    25,    26,    -1,    28,    29,
    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,    69,
    -1,    -1,    72,    73,    74,    -1,    -1,    77,    78,    79,
    80,    -1,    -1,    -1,    -1,     3,     4,     5,     6,    -1,
    90,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    -1,    -1,    24,    25,    26,    -1,
    28,    29,    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    62,    63,    64,    65,    66,    67,
    -1,    69,    -1,    -1,    72,    73,    74,    -1,    -1,    77,
    78,    79,    80,    -1,    -1,    -1,    -1,     3,     4,     5,
    6,    -1,    90,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    -1,    -1,    24,    25,
    26,    -1,    28,    29,    -1,    -1,    32,    33,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,    65,
    66,    67,    -1,    69,    -1,    -1,    72,    -1,    74,    -1,
    -1,    77,    78,    79,    80,    -1,    -1,    -1,    -1,     3,
    4,     5,     6,    -1,    90,     9,    10,    11,    12,    13,
    -1,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
    24,    25,    26,    -1,    28,    29,    -1,    -1,    32,    33,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,
    64,    65,    66,    67,    -1,    69,    -1,    -1,    72,    -1,
    74,    -1,    -1,    77,    78,    79,    80,     3,     4,     5,
    -1,    -1,    -1,    -1,    10,    11,    90,    -1,    14,    -1,
    16,    17,    -1,    19,    -1,    -1,    -1,    -1,    24,    44,
    45,    -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    -1,    44,    45,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,    65,
    66,    67,    -1,    69,    89,    -1,    72,    -1,    74,    -1,
    -1,    77,    78,    79,    80,     3,     4,     5,    -1,    -1,
    -1,    -1,    10,    -1,    -1,    -1,    14,    -1,    16,    17,
    -1,    19,    -1,    -1,    -1,    -1,    24,    44,    45,    -1,
    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    -1,    44,    45,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    62,    63,    64,    65,    66,    67,
    -1,    69,    89,    -1,    72,    -1,    74,    75,    -1,    77,
    78,    79,    80,     3,     4,     5,    -1,    12,    13,    -1,
    10,    -1,    -1,    18,    14,    -1,    16,    17,    -1,    19,
    25,    26,    -1,    -1,    24,    -1,    31,    32,    -1,    -1,
    -1,    -1,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,    69,
    70,    -1,    72,    -1,    74,    -1,    -1,    77,    78,    79,
    80,     3,     4,     5,    -1,    -1,    -1,    -1,    10,    -1,
    -1,    -1,    14,    -1,    16,    17,    -1,    19,    -1,    -1,
    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    62,    63,    64,    65,    66,    67,    -1,    69,    -1,    -1,
    72,    -1,    74,    75,    -1,    77,    78,    79,    80,     3,
    4,     5,    -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,
    14,    -1,    16,    17,    -1,    19,    -1,    -1,    -1,    -1,
    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    44,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,
    64,    65,    66,    67,    -1,    69,    -1,    -1,    72,    -1,
    74,    -1,    -1,    77,    78,    79,    80,     3,     4,     5,
    6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    -1,    28,    29,    30,    31,    32,    33,    -1,    35,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,     3,     4,
    5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    26,    -1,    28,    29,    30,    31,    32,    33,    -1,
    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,     3,
    4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    -1,    28,    29,    30,    31,    32,    33,
    -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] = {
    0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    24,    25,
    26,    28,    29,    32,    33,    44,    45,    53,    62,    63,
    64,    65,    66,    67,    69,    72,    74,    77,    78,    79,
    80,    90,    94,   101,   102,   107,   109,   111,   115,   116,
    118,   119,   121,   123,   125,   127,   129,   132,   135,   138,
    141,   144,   147,   150,   153,   156,   160,   161,   162,   163,
    168,   173,   174,   175,   176,   179,   180,   181,   182,   183,
    189,   190,   191,   192,   194,   195,   205,   206,   207,     1,
    64,    90,    69,    10,    14,    64,    72,   100,   101,   106,
    108,   198,    64,   164,   166,    64,   169,   170,     1,    64,
    90,    64,     1,    90,   106,   108,   110,   114,   116,   117,
    119,   120,   122,   124,   126,   128,   130,   133,   136,   139,
    142,   145,   148,   151,   154,   158,   114,   117,   120,   120,
    69,   161,    69,   120,    69,    69,   158,   162,     1,    90,
    64,   193,   120,   120,    68,   120,   120,   158,    73,   206,
    71,   103,   104,   105,   120,   120,   120,   120,    69,    74,
    76,   112,    74,    76,   112,    44,    45,    50,    51,    52,
    53,    54,    55,    56,    57,    58,    59,    60,    61,    89,
    157,    49,    67,    81,    82,    77,    78,    46,    47,    48,
    22,    23,    40,    41,    83,    84,    36,    37,    38,    39,
    85,    86,    87,    43,    42,    88,     1,    71,    90,     0,
    207,     1,    90,    11,   114,   128,   131,   134,   137,   140,
    143,   146,   149,   152,   155,   159,   178,   106,   108,    64,
    69,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    28,    29,    30,    31,    32,
    33,    35,    62,    63,    64,    73,    92,    93,    95,    96,
    99,    74,    76,   112,    89,   171,     1,    71,    90,   171,
    1,    71,    90,     1,    90,    69,   112,    74,    76,   112,
    44,    45,   157,    49,    67,    81,    82,    77,    78,    46,
    47,    48,    22,    23,    40,    41,    83,    84,    36,    37,
    38,    39,    85,    86,    87,    43,    42,    88,     1,    71,
    90,   158,    21,   158,   158,   158,     1,    90,    30,    31,
    89,     1,    76,    90,   161,    70,    73,    71,    75,    75,
    154,    71,    70,   113,   154,   158,    64,    93,   158,    93,
    154,   122,   122,   122,   122,   124,   124,   126,   126,   126,
    128,   128,   128,   128,   128,   128,   130,   130,   130,   130,
    133,   136,   139,   142,   145,   154,   154,    64,   165,   167,
    22,   157,    23,    40,    41,    83,    84,    36,    37,    38,
    39,    85,    86,    87,    43,    42,    88,    71,    90,   112,
    69,    64,    70,   203,    93,    68,    71,    73,   158,    93,
    154,   166,   170,    70,   203,   158,    93,   154,   122,   122,
    122,   122,   124,   124,   126,   126,   126,   128,   128,   128,
    128,   128,   128,   130,   130,   130,   130,   133,   136,   139,
    142,   145,   154,   154,    70,    69,    70,    70,    70,    69,
    162,    64,   193,    64,    81,   104,    70,    71,    75,    75,
    68,    22,    89,   172,    71,    90,   158,   114,   155,   128,
    128,   128,   128,   128,   131,   131,   131,   131,   134,   137,
    140,   143,   146,   155,   155,   158,   177,    70,   203,   199,
    70,    71,    69,   154,    73,    96,    75,   196,    70,    75,
    68,   161,   158,   161,    72,   184,   161,    64,     1,    76,
    90,     1,    90,    75,   154,   154,   154,   158,   155,    22,
    64,   167,   177,    70,    68,    90,   201,    70,    72,   204,
    200,    64,    70,   203,   204,   197,   154,    35,    70,     7,
    185,   186,   187,    70,    70,   158,   172,    90,   161,   155,
    177,   204,   202,    73,   206,   204,    97,    70,   204,   161,
    1,    90,   158,     8,    73,   188,   187,   162,   161,    70,
    177,    70,   204,    73,   204,    98,    68,    68,   185,    31,
    161,    70,   161,   204,   206,   206,    73,   162,   161
};

#define yyerrok     (yyerrstatus = 0)
#define yyclearin   (yychar = YYEMPTY)
#define YYEMPTY     (-2)
#define YYEOF       0

#define YYACCEPT    goto yyacceptlab
#define YYABORT     goto yyabortlab
#define YYERROR     goto yyerrorlab

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL      goto yyerrlab
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
            YYERROR;                          \
        }                               \
    while (YYID (0))

/* Error token number */
#define YYTERROR    1
#define YYERRCODE   256

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
yy_location_print_(FILE *yyo, YYLTYPE const *const yylocp)
#else
static unsigned
yy_location_print_(yyo, yylocp)
FILE *yyo;
YYLTYPE const *const yylocp;
#endif
{
    unsigned res = 0;
    int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
    if (0 <= yylocp->first_line) {
        res += fprintf(yyo, "%d", yylocp->first_line);
        if (0 <= yylocp->first_column) {
            res += fprintf(yyo, ".%d", yylocp->first_column);
        }
    }
    if (0 <= yylocp->last_line) {
        if (yylocp->first_line < yylocp->last_line) {
            res += fprintf(yyo, "-%d", yylocp->last_line);
            if (0 <= end_col) {
                res += fprintf(yyo, ".%d", end_col);
            }
        } else if (0 <= end_col && yylocp->first_column < end_col) {
            res += fprintf(yyo, "-%d", end_col);
        }
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

# define YYDPRINTF(Args)            \
    do {                        \
        if (yydebug)                  \
            YYFPRINTF Args;             \
    } while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)            \
    do {                                      \
        if (yydebug)                                \
        {                                     \
            YYFPRINTF (stderr, "%s ", Title);                   \
            yy_symbol_print (stderr,                        \
                             Type, Value, Location); \
            YYFPRINTF (stderr, "\n");                       \
        }                                     \
    } while (YYID (0))

/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print(FILE *yyoutput, int yytype, YYSTYPE const *const yyvaluep, YYLTYPE const *const yylocationp)
#else
static void
yy_symbol_value_print(yyoutput, yytype, yyvaluep, yylocationp)
FILE *yyoutput;
int yytype;
YYSTYPE const *const yyvaluep;
YYLTYPE const *const yylocationp;
#endif
{
    FILE *yyo = yyoutput;
    YYUSE(yyo);
    if (!yyvaluep) {
        return;
    }
    YYUSE(yylocationp);
# ifdef YYPRINT
    if (yytype < YYNTOKENS) {
        YYPRINT(yyoutput, yytoknum[yytype], *yyvaluep);
    }
# else
    YYUSE(yyoutput);
# endif
    switch (yytype) {
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
yy_symbol_print(FILE *yyoutput, int yytype, YYSTYPE const *const yyvaluep, YYLTYPE const *const yylocationp)
#else
static void
yy_symbol_print(yyoutput, yytype, yyvaluep, yylocationp)
FILE *yyoutput;
int yytype;
YYSTYPE const *const yyvaluep;
YYLTYPE const *const yylocationp;
#endif
{
    if (yytype < YYNTOKENS) {
        YYFPRINTF(yyoutput, "token %s (", yytname[yytype]);
    } else {
        YYFPRINTF(yyoutput, "nterm %s (", yytname[yytype]);
    }

    YY_LOCATION_PRINT(yyoutput, *yylocationp);
    YYFPRINTF(yyoutput, ": ");
    yy_symbol_value_print(yyoutput, yytype, yyvaluep, yylocationp);
    YYFPRINTF(yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print(yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print(yybottom, yytop)
yytype_int16 *yybottom;
yytype_int16 *yytop;
#endif
{
    YYFPRINTF(stderr, "Stack now");
    for (; yybottom <= yytop; yybottom++) {
        int yybot = *yybottom;
        YYFPRINTF(stderr, " %d", yybot);
    }
    YYFPRINTF(stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                \
    do {                                \
        if (yydebug)                          \
            yy_stack_print ((Bottom), (Top));               \
    } while (YYID (0))

/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print(YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print(yyvsp, yylsp, yyrule)
YYSTYPE *yyvsp;
YYLTYPE *yylsp;
int yyrule;
#endif
{
    int yynrhs = yyr2[yyrule];
    int yyi;
    unsigned long int yylno = yyrline[yyrule];
    YYFPRINTF(stderr, "Reducing stack by rule %d (line %lu):\n",
              yyrule - 1, yylno);
    /* The symbols being reduced.  */
    for (yyi = 0; yyi < yynrhs; yyi++) {
        YYFPRINTF(stderr, "   $%d = ", yyi + 1);
        yy_symbol_print(stderr, yyrhs[yyprhs[yyrule] + yyi],
                        &(yyvsp[(yyi + 1) - (yynrhs)])
                        , &(yylsp[(yyi + 1) - (yynrhs)]));
        YYFPRINTF(stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)      \
    do {                    \
        if (yydebug)              \
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
#ifndef YYINITDEPTH
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
yystrlen(const char *yystr)
#else
static YYSIZE_T
yystrlen(yystr)
const char *yystr;
#endif
{
    YYSIZE_T yylen;
    for (yylen = 0; yystr[yylen]; yylen++) {
        continue;
    }
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
yystpcpy(char *yydest, const char *yysrc)
#else
static char *
yystpcpy(yydest, yysrc)
char *yydest;
const char *yysrc;
#endif
{
    char *yyd = yydest;
    const char *yys = yysrc;

    while ((*yyd++ = *yys++) != '\0') {
        continue;
    }

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
yytnamerr(char *yyres, const char *yystr)
{
    if (*yystr == '"') {
        YYSIZE_T yyn = 0;
        char const *yyp = yystr;

        for (;;)
            switch (*++yyp) {
            case '\'':
            case ',':
                goto do_not_strip_quotes;

            case '\\':
                if (*++yyp != '\\') {
                    goto do_not_strip_quotes;
                }
            /* Fall through.  */
            default:
                if (yyres) {
                    yyres[yyn] = *yyp;
                }
                yyn++;
                break;

            case '"':
                if (yyres) {
                    yyres[yyn] = '\0';
                }
                return yyn;
            }
    do_not_strip_quotes: ;
    }

    if (! yyres) {
        return yystrlen(yystr);
    }

    return yystpcpy(yyres, yystr) - yyres;
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
yysyntax_error(YYSIZE_T *yymsg_alloc, char **yymsg,
               yytype_int16 *yyssp, int yytoken)
{
    YYSIZE_T yysize0 = yytnamerr(YY_NULL, yytname[yytoken]);
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
    if (yytoken != YYEMPTY) {
        int yyn = yypact[*yyssp];
        yyarg[yycount++] = yytname[yytoken];
        if (!yypact_value_is_default(yyn)) {
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
                        && !yytable_value_is_error(yytable[yyx + yyn])) {
                    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM) {
                        yycount = 1;
                        yysize = yysize0;
                        break;
                    }
                    yyarg[yycount++] = yytname[yyx];
                    {
                        YYSIZE_T yysize1 = yysize + yytnamerr(YY_NULL, yytname[yyx]);
                        if (!(yysize <= yysize1
                                && yysize1 <= YYSTACK_ALLOC_MAXIMUM)) {
                            return 2;
                        }
                        yysize = yysize1;
                    }
                }
        }
    }

    switch (yycount) {
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
        YYSIZE_T yysize1 = yysize + yystrlen(yyformat);
        if (!(yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)) {
            return 2;
        }
        yysize = yysize1;
    }

    if (*yymsg_alloc < yysize) {
        *yymsg_alloc = 2 * yysize;
        if (!(yysize <= *yymsg_alloc
                && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM)) {
            *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
        }
        return 1;
    }

    /* Avoid sprintf, as that infringes on the user's name space.
       Don't have undefined behavior even if the translation
       produced a string with the wrong number of "%s"s.  */
    {
        char *yyp = *yymsg;
        int yyi = 0;
        while ((*yyp = *yyformat) != '\0')
            if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount) {
                yyp += yytnamerr(yyp, yyarg[yyi++]);
                yyformat += 2;
            } else {
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
yydestruct(const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct(yymsg, yytype, yyvaluep, yylocationp)
const char *yymsg;
int yytype;
YYSTYPE *yyvaluep;
YYLTYPE *yylocationp;
#endif
{
    YYUSE(yyvaluep);
    YYUSE(yylocationp);

    if (!yymsg) {
        yymsg = "Deleting";
    }
    YY_SYMBOL_PRINT(yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype) {

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
yyparse(void *YYPARSE_PARAM)
#else
int
yyparse(YYPARSE_PARAM)
void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse(void)
#else
int
yyparse()

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

    YYDPRINTF((stderr, "Starting parse\n"));

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

    if (yyss + yystacksize - 1 <= yyssp) {
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
            yyoverflow(YY_("memory exhausted"),
                       &yyss1, yysize * sizeof(*yyssp),
                       &yyvs1, yysize * sizeof(*yyvsp),
                       &yyls1, yysize * sizeof(*yylsp),
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
        if (YYMAXDEPTH <= yystacksize) {
            goto yyexhaustedlab;
        }
        yystacksize *= 2;
        if (YYMAXDEPTH < yystacksize) {
            yystacksize = YYMAXDEPTH;
        }

        {
            yytype_int16 *yyss1 = yyss;
            union yyalloc *yyptr =
                    (union yyalloc *) YYSTACK_ALLOC(YYSTACK_BYTES(yystacksize));
            if (! yyptr) {
                goto yyexhaustedlab;
            }
            YYSTACK_RELOCATE(yyss_alloc, yyss);
            YYSTACK_RELOCATE(yyvs_alloc, yyvs);
            YYSTACK_RELOCATE(yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
            if (yyss1 != yyssa) {
                YYSTACK_FREE(yyss1);
            }
        }
# endif
#endif /* no yyoverflow */

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;
        yylsp = yyls + yysize - 1;

        YYDPRINTF((stderr, "Stack size increased to %lu\n",
                   (unsigned long int) yystacksize));

        if (yyss + yystacksize - 1 <= yyssp) {
            YYABORT;
        }
    }

    YYDPRINTF((stderr, "Entering state %d\n", yystate));

    if (yystate == YYFINAL) {
        YYACCEPT;
    }

    goto yybackup;

    /*-----------.
    | yybackup.  |
    `-----------*/
yybackup:

    /* Do appropriate processing given the current state.  Read a
       lookahead token if we need one and don't already have one.  */

    /* First try to decide what to do without reference to lookahead token.  */
    yyn = yypact[yystate];
    if (yypact_value_is_default(yyn)) {
        goto yydefault;
    }

    /* Not known => get a lookahead token if don't already have one.  */

    /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
    if (yychar == YYEMPTY) {
        YYDPRINTF((stderr, "Reading a token: "));
        yychar = YYLEX;
    }

    if (yychar <= YYEOF) {
        yychar = yytoken = YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
    } else {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
    }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken) {
        goto yydefault;
    }
    yyn = yytable[yyn];
    if (yyn <= 0) {
        if (yytable_value_is_error(yyn)) {
            goto yyerrlab;
        }
        yyn = -yyn;
        goto yyreduce;
    }

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus) {
        yyerrstatus--;
    }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);

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
    if (yyn == 0) {
        goto yyerrlab;
    }
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
    yyval = yyvsp[1 - yylen];

    /* Default location.  */
    YYLLOC_DEFAULT(yyloc, (yylsp - yylen), yylen);
    YY_REDUCE_PRINT(yyn);
    switch (yyn) {
    case 2:
        /* Line 1792 of yacc.c  */
#line 194 "grammar.y"
        {
            (yyval.ident) = new Identifier("break");
        }
        break;

    case 3:
        /* Line 1792 of yacc.c  */
#line 195 "grammar.y"
        {
            (yyval.ident) = new Identifier("case");
        }
        break;

    case 4:
        /* Line 1792 of yacc.c  */
#line 196 "grammar.y"
        {
            (yyval.ident) = new Identifier("default");
        }
        break;

    case 5:
        /* Line 1792 of yacc.c  */
#line 197 "grammar.y"
        {
            (yyval.ident) = new Identifier("for");
        }
        break;

    case 6:
        /* Line 1792 of yacc.c  */
#line 198 "grammar.y"
        {
            (yyval.ident) = new Identifier("new");
        }
        break;

    case 7:
        /* Line 1792 of yacc.c  */
#line 199 "grammar.y"
        {
            (yyval.ident) = new Identifier("var");
        }
        break;

    case 8:
        /* Line 1792 of yacc.c  */
#line 200 "grammar.y"
        {
            (yyval.ident) = new Identifier("const");
        }
        break;

    case 9:
        /* Line 1792 of yacc.c  */
#line 201 "grammar.y"
        {
            (yyval.ident) = new Identifier("continue");
        }
        break;

    case 10:
        /* Line 1792 of yacc.c  */
#line 202 "grammar.y"
        {
            (yyval.ident) = new Identifier("function");
        }
        break;

    case 11:
        /* Line 1792 of yacc.c  */
#line 203 "grammar.y"
        {
            (yyval.ident) = new Identifier("return");
        }
        break;

    case 12:
        /* Line 1792 of yacc.c  */
#line 204 "grammar.y"
        {
            (yyval.ident) = new Identifier("void");
        }
        break;

    case 13:
        /* Line 1792 of yacc.c  */
#line 205 "grammar.y"
        {
            (yyval.ident) = new Identifier("delete");
        }
        break;

    case 14:
        /* Line 1792 of yacc.c  */
#line 206 "grammar.y"
        {
            (yyval.ident) = new Identifier("if");
        }
        break;

    case 15:
        /* Line 1792 of yacc.c  */
#line 207 "grammar.y"
        {
            (yyval.ident) = new Identifier("this");
        }
        break;

    case 16:
        /* Line 1792 of yacc.c  */
#line 208 "grammar.y"
        {
            (yyval.ident) = new Identifier("do");
        }
        break;

    case 17:
        /* Line 1792 of yacc.c  */
#line 209 "grammar.y"
        {
            (yyval.ident) = new Identifier("while");
        }
        break;

    case 18:
        /* Line 1792 of yacc.c  */
#line 210 "grammar.y"
        {
            (yyval.ident) = new Identifier("in");
        }
        break;

    case 19:
        /* Line 1792 of yacc.c  */
#line 211 "grammar.y"
        {
            (yyval.ident) = new Identifier("instanceof");
        }
        break;

    case 20:
        /* Line 1792 of yacc.c  */
#line 212 "grammar.y"
        {
            (yyval.ident) = new Identifier("typeof");
        }
        break;

    case 21:
        /* Line 1792 of yacc.c  */
#line 213 "grammar.y"
        {
            (yyval.ident) = new Identifier("switch");
        }
        break;

    case 22:
        /* Line 1792 of yacc.c  */
#line 214 "grammar.y"
        {
            (yyval.ident) = new Identifier("with");
        }
        break;

    case 23:
        /* Line 1792 of yacc.c  */
#line 215 "grammar.y"
        {
            (yyval.ident) = new Identifier("throw");
        }
        break;

    case 24:
        /* Line 1792 of yacc.c  */
#line 216 "grammar.y"
        {
            (yyval.ident) = new Identifier("try");
        }
        break;

    case 25:
        /* Line 1792 of yacc.c  */
#line 217 "grammar.y"
        {
            (yyval.ident) = new Identifier("catch");
        }
        break;

    case 26:
        /* Line 1792 of yacc.c  */
#line 218 "grammar.y"
        {
            (yyval.ident) = new Identifier("finally");
        }
        break;

    case 27:
        /* Line 1792 of yacc.c  */
#line 219 "grammar.y"
        {
            (yyval.ident) = new Identifier("debugger");
        }
        break;

    case 28:
        /* Line 1792 of yacc.c  */
#line 220 "grammar.y"
        {
            (yyval.ident) = new Identifier("import");
        }
        break;

    case 29:
        /* Line 1792 of yacc.c  */
#line 221 "grammar.y"
        {
            (yyval.ident) = new Identifier("null");
        }
        break;

    case 30:
        /* Line 1792 of yacc.c  */
#line 222 "grammar.y"
        {
            (yyval.ident) = new Identifier("true");
        }
        break;

    case 31:
        /* Line 1792 of yacc.c  */
#line 223 "grammar.y"
        {
            (yyval.ident) = new Identifier("false");
        }
        break;

    case 32:
        /* Line 1792 of yacc.c  */
#line 224 "grammar.y"
        {
            (yyval.ident) = new Identifier("else");
        }
        break;

    case 33:
        /* Line 1792 of yacc.c  */
#line 228 "grammar.y"
        {
            (yyval.ident) = (yyvsp[(1) - (1)].ident);
        }
        break;

    case 34:
        /* Line 1792 of yacc.c  */
#line 229 "grammar.y"
        {
            (yyval.ident) = (yyvsp[(1) - (1)].ident);
        }
        break;

    case 35:
        /* Line 1792 of yacc.c  */
#line 233 "grammar.y"
        {
            (yyval.node) = new NullNode();
        }
        break;

    case 36:
        /* Line 1792 of yacc.c  */
#line 234 "grammar.y"
        {
            (yyval.node) = new BooleanNode(true);
        }
        break;

    case 37:
        /* Line 1792 of yacc.c  */
#line 235 "grammar.y"
        {
            (yyval.node) = new BooleanNode(false);
        }
        break;

    case 38:
        /* Line 1792 of yacc.c  */
#line 236 "grammar.y"
        {
            (yyval.node) = new NumberNode((yyvsp[(1) - (1)].dval));
        }
        break;

    case 39:
        /* Line 1792 of yacc.c  */
#line 237 "grammar.y"
        {
            (yyval.node) = new StringNode((yyvsp[(1) - (1)].ustr));
        }
        break;

    case 40:
        /* Line 1792 of yacc.c  */
#line 238 "grammar.y"
        {
            Lexer &l = lexer();
            if (!l.scanRegExp()) {
                YYABORT;
            }
            (yyval.node) = new RegExpNode(l.pattern(), l.flags());
        }
        break;

    case 41:
        /* Line 1792 of yacc.c  */
#line 244 "grammar.y"
        {
            Lexer &l = lexer();
            if (!l.scanRegExp()) {
                YYABORT;
            }
            (yyval.node) = new RegExpNode("=" + l.pattern(), l.flags());
        }
        break;

    case 42:
        /* Line 1792 of yacc.c  */
#line 253 "grammar.y"
        {
            (yyval.pname) = new PropertyNameNode(*(yyvsp[(1) - (1)].ident));
        }
        break;

    case 43:
        /* Line 1792 of yacc.c  */
#line 254 "grammar.y"
        {
            (yyval.pname) = new PropertyNameNode(Identifier(*(yyvsp[(1) - (1)].ustr)));
        }
        break;

    case 44:
        /* Line 1792 of yacc.c  */
#line 255 "grammar.y"
        {
            (yyval.pname) = new PropertyNameNode(Identifier(UString::from((yyvsp[(1) - (1)].dval))));
        }
        break;

    case 45:
        /* Line 1792 of yacc.c  */
#line 259 "grammar.y"
        {
            (yyval.pnode) = new PropertyNode((yyvsp[(1) - (3)].pname), (yyvsp[(3) - (3)].node), PropertyNode::Constant);
        }
        break;

    case 46:
        /* Line 1792 of yacc.c  */
#line 260 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 47:
        /* Line 1792 of yacc.c  */
#line 260 "grammar.y"
        {
            if (!makeGetterOrSetterPropertyNode((yyval.pnode), *(yyvsp[(1) - (6)].ident), *(yyvsp[(2) - (6)].ident), nullptr, (yyvsp[(6) - (6)].body))) {
                YYABORT;
            }
        }
        break;

    case 48:
        /* Line 1792 of yacc.c  */
#line 264 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 49:
        /* Line 1792 of yacc.c  */
#line 264 "grammar.y"
        {
            if (!makeGetterOrSetterPropertyNode((yyval.pnode), *(yyvsp[(1) - (7)].ident), *(yyvsp[(2) - (7)].ident), (yyvsp[(4) - (7)].param), (yyvsp[(7) - (7)].body))) {
                YYABORT;
            }
        }
        break;

    case 50:
        /* Line 1792 of yacc.c  */
#line 271 "grammar.y"
        {
            (yyval.plist) = new PropertyListNode((yyvsp[(1) - (1)].pnode));
        }
        break;

    case 51:
        /* Line 1792 of yacc.c  */
#line 272 "grammar.y"
        {
            (yyval.plist) = new PropertyListNode((yyvsp[(3) - (3)].pnode), (yyvsp[(1) - (3)].plist));
        }
        break;

    case 53:
        /* Line 1792 of yacc.c  */
#line 277 "grammar.y"
        {
            (yyval.node) = new ObjectLiteralNode();
        }
        break;

    case 54:
        /* Line 1792 of yacc.c  */
#line 278 "grammar.y"
        {
            (yyval.node) = new ObjectLiteralNode((yyvsp[(2) - (3)].plist));
        }
        break;

    case 55:
        /* Line 1792 of yacc.c  */
#line 280 "grammar.y"
        {
            (yyval.node) = new ObjectLiteralNode((yyvsp[(2) - (4)].plist));
        }
        break;

    case 56:
        /* Line 1792 of yacc.c  */
#line 284 "grammar.y"
        {
            (yyval.node) = new ThisNode();
        }
        break;

    case 59:
        /* Line 1792 of yacc.c  */
#line 287 "grammar.y"
        {
            (yyval.node) = new VarAccessNode(*(yyvsp[(1) - (1)].ident));
        }
        break;

    case 60:
        /* Line 1792 of yacc.c  */
#line 288 "grammar.y"
        {
            (yyval.node) = makeGroupNode((yyvsp[(2) - (3)].node));
        }
        break;

    case 61:
        /* Line 1792 of yacc.c  */
#line 293 "grammar.y"
        {
            (yyval.node) = new ArrayNode((yyvsp[(2) - (3)].ival));
        }
        break;

    case 62:
        /* Line 1792 of yacc.c  */
#line 294 "grammar.y"
        {
            (yyval.node) = new ArrayNode((yyvsp[(2) - (3)].elm));
        }
        break;

    case 63:
        /* Line 1792 of yacc.c  */
#line 295 "grammar.y"
        {
            (yyval.node) = new ArrayNode((yyvsp[(4) - (5)].ival), (yyvsp[(2) - (5)].elm));
        }
        break;

    case 64:
        /* Line 1792 of yacc.c  */
#line 299 "grammar.y"
        {
            (yyval.elm) = new ElementNode((yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].node));
        }
        break;

    case 65:
        /* Line 1792 of yacc.c  */
#line 301 "grammar.y"
        {
            (yyval.elm) = new ElementNode((yyvsp[(1) - (4)].elm), (yyvsp[(3) - (4)].ival), (yyvsp[(4) - (4)].node));
        }
        break;

    case 66:
        /* Line 1792 of yacc.c  */
#line 305 "grammar.y"
        {
            (yyval.ival) = 0;
        }
        break;

    case 68:
        /* Line 1792 of yacc.c  */
#line 310 "grammar.y"
        {
            (yyval.ival) = 1;
        }
        break;

    case 69:
        /* Line 1792 of yacc.c  */
#line 311 "grammar.y"
        {
            (yyval.ival) = (yyvsp[(1) - (2)].ival) + 1;
        }
        break;

    case 71:
        /* Line 1792 of yacc.c  */
#line 316 "grammar.y"
        {
            (yyval.node) = (yyvsp[(1) - (1)].funcExpr);
        }
        break;

    case 72:
        /* Line 1792 of yacc.c  */
#line 317 "grammar.y"
        {
            (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
        }
        break;

    case 73:
        /* Line 1792 of yacc.c  */
#line 318 "grammar.y"
        {
            (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 74:
        /* Line 1792 of yacc.c  */
#line 319 "grammar.y"
        {
            (yyval.node) = new NewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args));
        }
        break;

    case 76:
        /* Line 1792 of yacc.c  */
#line 324 "grammar.y"
        {
            (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
        }
        break;

    case 77:
        /* Line 1792 of yacc.c  */
#line 325 "grammar.y"
        {
            (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 78:
        /* Line 1792 of yacc.c  */
#line 326 "grammar.y"
        {
            (yyval.node) = new NewExprNode((yyvsp[(2) - (3)].node), (yyvsp[(3) - (3)].args));
        }
        break;

    case 80:
        /* Line 1792 of yacc.c  */
#line 331 "grammar.y"
        {
            (yyval.node) = new NewExprNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 82:
        /* Line 1792 of yacc.c  */
#line 336 "grammar.y"
        {
            (yyval.node) = new NewExprNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 83:
        /* Line 1792 of yacc.c  */
#line 340 "grammar.y"
        {
            (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args));
        }
        break;

    case 84:
        /* Line 1792 of yacc.c  */
#line 341 "grammar.y"
        {
            (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args));
        }
        break;

    case 85:
        /* Line 1792 of yacc.c  */
#line 342 "grammar.y"
        {
            (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
        }
        break;

    case 86:
        /* Line 1792 of yacc.c  */
#line 343 "grammar.y"
        {
            (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 87:
        /* Line 1792 of yacc.c  */
#line 347 "grammar.y"
        {
            (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args));
        }
        break;

    case 88:
        /* Line 1792 of yacc.c  */
#line 348 "grammar.y"
        {
            (yyval.node) = makeFunctionCallNode((yyvsp[(1) - (2)].node), (yyvsp[(2) - (2)].args));
        }
        break;

    case 89:
        /* Line 1792 of yacc.c  */
#line 349 "grammar.y"
        {
            (yyval.node) = new BracketAccessorNode((yyvsp[(1) - (4)].node), (yyvsp[(3) - (4)].node));
        }
        break;

    case 90:
        /* Line 1792 of yacc.c  */
#line 350 "grammar.y"
        {
            (yyval.node) = new DotAccessorNode((yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 91:
        /* Line 1792 of yacc.c  */
#line 354 "grammar.y"
        {
            (yyval.args) = new ArgumentsNode();
        }
        break;

    case 92:
        /* Line 1792 of yacc.c  */
#line 355 "grammar.y"
        {
            (yyval.args) = new ArgumentsNode((yyvsp[(2) - (3)].alist));
        }
        break;

    case 93:
        /* Line 1792 of yacc.c  */
#line 359 "grammar.y"
        {
            (yyval.alist) = new ArgumentListNode((yyvsp[(1) - (1)].node));
        }
        break;

    case 94:
        /* Line 1792 of yacc.c  */
#line 360 "grammar.y"
        {
            (yyval.alist) = new ArgumentListNode((yyvsp[(1) - (3)].alist), (yyvsp[(3) - (3)].node));
        }
        break;

    case 99:
        /* Line 1792 of yacc.c  */
#line 374 "grammar.y"
        {
            (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpPlusPlus);
        }
        break;

    case 100:
        /* Line 1792 of yacc.c  */
#line 375 "grammar.y"
        {
            (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpPlusPlus);
        }
        break;

    case 101:
        /* Line 1792 of yacc.c  */
#line 376 "grammar.y"
        {
            (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpMinusMinus);
        }
        break;

    case 102:
        /* Line 1792 of yacc.c  */
#line 377 "grammar.y"
        {
            (yyval.node) = makePrefixNode((yyvsp[(2) - (2)].node), OpMinusMinus);
        }
        break;

    case 104:
        /* Line 1792 of yacc.c  */
#line 382 "grammar.y"
        {
            (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus);
        }
        break;

    case 105:
        /* Line 1792 of yacc.c  */
#line 383 "grammar.y"
        {
            (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus);
        }
        break;

    case 108:
        /* Line 1792 of yacc.c  */
#line 389 "grammar.y"
        {
            (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpPlusPlus);
        }
        break;

    case 109:
        /* Line 1792 of yacc.c  */
#line 390 "grammar.y"
        {
            (yyval.node) = makePostfixNode((yyvsp[(1) - (2)].node), OpMinusMinus);
        }
        break;

    case 111:
        /* Line 1792 of yacc.c  */
#line 395 "grammar.y"
        {
            (yyval.node) = makeDeleteNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 112:
        /* Line 1792 of yacc.c  */
#line 396 "grammar.y"
        {
            (yyval.node) = new VoidNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 113:
        /* Line 1792 of yacc.c  */
#line 397 "grammar.y"
        {
            (yyval.node) = makeTypeOfNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 114:
        /* Line 1792 of yacc.c  */
#line 398 "grammar.y"
        {
            (yyval.node) = makeUnaryPlusNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 115:
        /* Line 1792 of yacc.c  */
#line 399 "grammar.y"
        {
            (yyval.node) = makeNegateNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 116:
        /* Line 1792 of yacc.c  */
#line 400 "grammar.y"
        {
            (yyval.node) = makeBitwiseNotNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 117:
        /* Line 1792 of yacc.c  */
#line 401 "grammar.y"
        {
            (yyval.node) = makeLogicalNotNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 123:
        /* Line 1792 of yacc.c  */
#line 415 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpExp);
        }
        break;

    case 125:
        /* Line 1792 of yacc.c  */
#line 420 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpExp);
        }
        break;

    case 127:
        /* Line 1792 of yacc.c  */
#line 425 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMult);
        }
        break;

    case 128:
        /* Line 1792 of yacc.c  */
#line 426 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpDiv);
        }
        break;

    case 129:
        /* Line 1792 of yacc.c  */
#line 427 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMod);
        }
        break;

    case 131:
        /* Line 1792 of yacc.c  */
#line 433 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMult);
        }
        break;

    case 132:
        /* Line 1792 of yacc.c  */
#line 435 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpDiv);
        }
        break;

    case 133:
        /* Line 1792 of yacc.c  */
#line 437 "grammar.y"
        {
            (yyval.node) = makeMultNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMod);
        }
        break;

    case 135:
        /* Line 1792 of yacc.c  */
#line 442 "grammar.y"
        {
            (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpPlus);
        }
        break;

    case 136:
        /* Line 1792 of yacc.c  */
#line 443 "grammar.y"
        {
            (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMinus);
        }
        break;

    case 138:
        /* Line 1792 of yacc.c  */
#line 449 "grammar.y"
        {
            (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpPlus);
        }
        break;

    case 139:
        /* Line 1792 of yacc.c  */
#line 451 "grammar.y"
        {
            (yyval.node) = makeAddNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpMinus);
        }
        break;

    case 141:
        /* Line 1792 of yacc.c  */
#line 456 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpLShift);
        }
        break;

    case 142:
        /* Line 1792 of yacc.c  */
#line 457 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpRShift);
        }
        break;

    case 143:
        /* Line 1792 of yacc.c  */
#line 458 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpURShift);
        }
        break;

    case 145:
        /* Line 1792 of yacc.c  */
#line 463 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpLShift);
        }
        break;

    case 146:
        /* Line 1792 of yacc.c  */
#line 464 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpRShift);
        }
        break;

    case 147:
        /* Line 1792 of yacc.c  */
#line 465 "grammar.y"
        {
            (yyval.node) = makeShiftNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node), OpURShift);
        }
        break;

    case 149:
        /* Line 1792 of yacc.c  */
#line 470 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node));
        }
        break;

    case 150:
        /* Line 1792 of yacc.c  */
#line 471 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node));
        }
        break;

    case 151:
        /* Line 1792 of yacc.c  */
#line 472 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 152:
        /* Line 1792 of yacc.c  */
#line 473 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 153:
        /* Line 1792 of yacc.c  */
#line 474 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node));
        }
        break;

    case 154:
        /* Line 1792 of yacc.c  */
#line 475 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node));
        }
        break;

    case 156:
        /* Line 1792 of yacc.c  */
#line 480 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node));
        }
        break;

    case 157:
        /* Line 1792 of yacc.c  */
#line 481 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node));
        }
        break;

    case 158:
        /* Line 1792 of yacc.c  */
#line 482 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 159:
        /* Line 1792 of yacc.c  */
#line 483 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 160:
        /* Line 1792 of yacc.c  */
#line 485 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node));
        }
        break;

    case 162:
        /* Line 1792 of yacc.c  */
#line 490 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLess, (yyvsp[(3) - (3)].node));
        }
        break;

    case 163:
        /* Line 1792 of yacc.c  */
#line 491 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreater, (yyvsp[(3) - (3)].node));
        }
        break;

    case 164:
        /* Line 1792 of yacc.c  */
#line 492 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpLessEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 165:
        /* Line 1792 of yacc.c  */
#line 493 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpGreaterEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 166:
        /* Line 1792 of yacc.c  */
#line 495 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpInstanceOf, (yyvsp[(3) - (3)].node));
        }
        break;

    case 167:
        /* Line 1792 of yacc.c  */
#line 496 "grammar.y"
        {
            (yyval.node) = makeRelationalNode((yyvsp[(1) - (3)].node), OpIn, (yyvsp[(3) - (3)].node));
        }
        break;

    case 169:
        /* Line 1792 of yacc.c  */
#line 501 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 170:
        /* Line 1792 of yacc.c  */
#line 502 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 171:
        /* Line 1792 of yacc.c  */
#line 503 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 172:
        /* Line 1792 of yacc.c  */
#line 504 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 174:
        /* Line 1792 of yacc.c  */
#line 510 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 175:
        /* Line 1792 of yacc.c  */
#line 512 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 176:
        /* Line 1792 of yacc.c  */
#line 514 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 177:
        /* Line 1792 of yacc.c  */
#line 516 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 179:
        /* Line 1792 of yacc.c  */
#line 522 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpEqEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 180:
        /* Line 1792 of yacc.c  */
#line 523 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpNotEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 181:
        /* Line 1792 of yacc.c  */
#line 525 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 182:
        /* Line 1792 of yacc.c  */
#line 527 "grammar.y"
        {
            (yyval.node) = makeEqualNode((yyvsp[(1) - (3)].node), OpStrNEq, (yyvsp[(3) - (3)].node));
        }
        break;

    case 184:
        /* Line 1792 of yacc.c  */
#line 532 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 186:
        /* Line 1792 of yacc.c  */
#line 538 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 188:
        /* Line 1792 of yacc.c  */
#line 543 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 190:
        /* Line 1792 of yacc.c  */
#line 548 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 192:
        /* Line 1792 of yacc.c  */
#line 554 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 194:
        /* Line 1792 of yacc.c  */
#line 560 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitXOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 196:
        /* Line 1792 of yacc.c  */
#line 565 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 198:
        /* Line 1792 of yacc.c  */
#line 571 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 200:
        /* Line 1792 of yacc.c  */
#line 577 "grammar.y"
        {
            (yyval.node) = makeBitOperNode((yyvsp[(1) - (3)].node), OpBitOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 202:
        /* Line 1792 of yacc.c  */
#line 582 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 204:
        /* Line 1792 of yacc.c  */
#line 588 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 206:
        /* Line 1792 of yacc.c  */
#line 594 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpAnd, (yyvsp[(3) - (3)].node));
        }
        break;

    case 208:
        /* Line 1792 of yacc.c  */
#line 599 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 210:
        /* Line 1792 of yacc.c  */
#line 605 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 212:
        /* Line 1792 of yacc.c  */
#line 610 "grammar.y"
        {
            (yyval.node) = makeBinaryLogicalNode((yyvsp[(1) - (3)].node), OpOr, (yyvsp[(3) - (3)].node));
        }
        break;

    case 214:
        /* Line 1792 of yacc.c  */
#line 616 "grammar.y"
        {
            (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node));
        }
        break;

    case 216:
        /* Line 1792 of yacc.c  */
#line 622 "grammar.y"
        {
            (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node));
        }
        break;

    case 218:
        /* Line 1792 of yacc.c  */
#line 628 "grammar.y"
        {
            (yyval.node) = makeConditionalNode((yyvsp[(1) - (5)].node), (yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].node));
        }
        break;

    case 220:
        /* Line 1792 of yacc.c  */
#line 634 "grammar.y"
        {
            (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node));
        }
        break;

    case 222:
        /* Line 1792 of yacc.c  */
#line 640 "grammar.y"
        {
            (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node));
        }
        break;

    case 224:
        /* Line 1792 of yacc.c  */
#line 646 "grammar.y"
        {
            (yyval.node) = makeAssignNode((yyvsp[(1) - (3)].node), (yyvsp[(2) - (3)].op), (yyvsp[(3) - (3)].node));
        }
        break;

    case 225:
        /* Line 1792 of yacc.c  */
#line 650 "grammar.y"
        {
            (yyval.op) = OpEqual;
        }
        break;

    case 226:
        /* Line 1792 of yacc.c  */
#line 651 "grammar.y"
        {
            (yyval.op) = OpPlusEq;
        }
        break;

    case 227:
        /* Line 1792 of yacc.c  */
#line 652 "grammar.y"
        {
            (yyval.op) = OpMinusEq;
        }
        break;

    case 228:
        /* Line 1792 of yacc.c  */
#line 653 "grammar.y"
        {
            (yyval.op) = OpMultEq;
        }
        break;

    case 229:
        /* Line 1792 of yacc.c  */
#line 654 "grammar.y"
        {
            (yyval.op) = OpDivEq;
        }
        break;

    case 230:
        /* Line 1792 of yacc.c  */
#line 655 "grammar.y"
        {
            (yyval.op) = OpExpEq;
        }
        break;

    case 231:
        /* Line 1792 of yacc.c  */
#line 656 "grammar.y"
        {
            (yyval.op) = OpLShift;
        }
        break;

    case 232:
        /* Line 1792 of yacc.c  */
#line 657 "grammar.y"
        {
            (yyval.op) = OpRShift;
        }
        break;

    case 233:
        /* Line 1792 of yacc.c  */
#line 658 "grammar.y"
        {
            (yyval.op) = OpURShift;
        }
        break;

    case 234:
        /* Line 1792 of yacc.c  */
#line 659 "grammar.y"
        {
            (yyval.op) = OpAndEq;
        }
        break;

    case 235:
        /* Line 1792 of yacc.c  */
#line 660 "grammar.y"
        {
            (yyval.op) = OpXOrEq;
        }
        break;

    case 236:
        /* Line 1792 of yacc.c  */
#line 661 "grammar.y"
        {
            (yyval.op) = OpOrEq;
        }
        break;

    case 237:
        /* Line 1792 of yacc.c  */
#line 662 "grammar.y"
        {
            (yyval.op) = OpModEq;
        }
        break;

    case 239:
        /* Line 1792 of yacc.c  */
#line 667 "grammar.y"
        {
            (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
        break;

    case 241:
        /* Line 1792 of yacc.c  */
#line 672 "grammar.y"
        {
            (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
        break;

    case 243:
        /* Line 1792 of yacc.c  */
#line 677 "grammar.y"
        {
            (yyval.node) = new CommaNode((yyvsp[(1) - (3)].node), (yyvsp[(3) - (3)].node));
        }
        break;

    case 261:
        /* Line 1792 of yacc.c  */
#line 701 "grammar.y"
        {
            (yyval.stat) = new BlockNode(nullptr);
            DBG((yyval.stat), (yylsp[(2) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 262:
        /* Line 1792 of yacc.c  */
#line 702 "grammar.y"
        {
            (yyval.stat) = new BlockNode((yyvsp[(2) - (3)].srcs));
            DBG((yyval.stat), (yylsp[(3) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 263:
        /* Line 1792 of yacc.c  */
#line 706 "grammar.y"
        {
            (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 264:
        /* Line 1792 of yacc.c  */
#line 707 "grammar.y"
        {
            (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 265:
        /* Line 1792 of yacc.c  */
#line 711 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl));
        }
        break;

    case 266:
        /* Line 1792 of yacc.c  */
#line 713 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl));
        }
        break;

    case 267:
        /* Line 1792 of yacc.c  */
#line 717 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl));
        }
        break;

    case 268:
        /* Line 1792 of yacc.c  */
#line 719 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl));
        }
        break;

    case 269:
        /* Line 1792 of yacc.c  */
#line 723 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), nullptr, VarDeclNode::Variable);
        }
        break;

    case 270:
        /* Line 1792 of yacc.c  */
#line 724 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Variable);
        }
        break;

    case 271:
        /* Line 1792 of yacc.c  */
#line 728 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), nullptr, VarDeclNode::Variable);
        }
        break;

    case 272:
        /* Line 1792 of yacc.c  */
#line 729 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Variable);
        }
        break;

    case 273:
        /* Line 1792 of yacc.c  */
#line 733 "grammar.y"
        {
            (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 274:
        /* Line 1792 of yacc.c  */
#line 735 "grammar.y"
        {
            (yyval.stat) = new VarStatementNode((yyvsp[(2) - (3)].vlist));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 275:
        /* Line 1792 of yacc.c  */
#line 739 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (1)].decl));
        }
        break;

    case 276:
        /* Line 1792 of yacc.c  */
#line 741 "grammar.y"
        {
            (yyval.vlist) = new VarDeclListNode((yyvsp[(1) - (3)].vlist), (yyvsp[(3) - (3)].decl));
        }
        break;

    case 277:
        /* Line 1792 of yacc.c  */
#line 745 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (1)].ident), nullptr, VarDeclNode::Constant);
        }
        break;

    case 278:
        /* Line 1792 of yacc.c  */
#line 746 "grammar.y"
        {
            (yyval.decl) = new VarDeclNode(*(yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].init), VarDeclNode::Constant);
        }
        break;

    case 279:
        /* Line 1792 of yacc.c  */
#line 750 "grammar.y"
        {
            (yyval.init) = new AssignExprNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 280:
        /* Line 1792 of yacc.c  */
#line 754 "grammar.y"
        {
            (yyval.init) = new AssignExprNode((yyvsp[(2) - (2)].node));
        }
        break;

    case 281:
        /* Line 1792 of yacc.c  */
#line 758 "grammar.y"
        {
            (yyval.stat) = new EmptyStatementNode();
        }
        break;

    case 282:
        /* Line 1792 of yacc.c  */
#line 762 "grammar.y"
        {
            (yyval.stat) = new ExprStatementNode((yyvsp[(1) - (2)].node));
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 283:
        /* Line 1792 of yacc.c  */
#line 763 "grammar.y"
        {
            (yyval.stat) = new ExprStatementNode((yyvsp[(1) - (2)].node));
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
            AUTO_SEMICOLON;
        }
        break;

    case 284:
        /* Line 1792 of yacc.c  */
#line 768 "grammar.y"
        {
            (yyval.stat) = makeIfNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat), nullptr);
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)]));
        }
        break;

    case 285:
        /* Line 1792 of yacc.c  */
#line 770 "grammar.y"
        {
            (yyval.stat) = makeIfNode((yyvsp[(3) - (7)].node), (yyvsp[(5) - (7)].stat), (yyvsp[(7) - (7)].stat));
            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(4) - (7)]));
        }
        break;

    case 286:
        /* Line 1792 of yacc.c  */
#line 774 "grammar.y"
        {
            (yyval.stat) = new DoWhileNode((yyvsp[(2) - (7)].stat), (yyvsp[(5) - (7)].node));
            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(3) - (7)]));
        }
        break;

    case 287:
        /* Line 1792 of yacc.c  */
#line 775 "grammar.y"
        {
            (yyval.stat) = new DoWhileNode((yyvsp[(2) - (7)].stat), (yyvsp[(5) - (7)].node));
            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(3) - (7)]));
            AUTO_SEMICOLON;
        }
        break;

    case 288:
        /* Line 1792 of yacc.c  */
#line 776 "grammar.y"
        {
            (yyval.stat) = new WhileNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat));
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)]));
        }
        break;

    case 289:
        /* Line 1792 of yacc.c  */
#line 778 "grammar.y"
        {
            (yyval.stat) = new ForNode((yyvsp[(3) - (9)].node), (yyvsp[(5) - (9)].node), (yyvsp[(7) - (9)].node), (yyvsp[(9) - (9)].stat));
            DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(8) - (9)]));
        }
        break;

    case 290:
        /* Line 1792 of yacc.c  */
#line 780 "grammar.y"
        {
            (yyval.stat) = new ForNode((yyvsp[(4) - (10)].vlist), (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].node), (yyvsp[(10) - (10)].stat));
            DBG((yyval.stat), (yylsp[(1) - (10)]), (yylsp[(9) - (10)]));
        }
        break;

    case 291:
        /* Line 1792 of yacc.c  */
#line 782 "grammar.y"
        {
            Node *n = (yyvsp[(3) - (7)].node)->nodeInsideAllParens();
            if (!n->isLocation()) {
                YYABORT;
            }
            (yyval.stat) = new ForInNode(n, (yyvsp[(5) - (7)].node), (yyvsp[(7) - (7)].stat));
            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(6) - (7)]));
        }
        break;

    case 292:
        /* Line 1792 of yacc.c  */
#line 790 "grammar.y"
        {
            (yyval.stat) = new ForInNode(*(yyvsp[(4) - (8)].ident), nullptr, (yyvsp[(6) - (8)].node), (yyvsp[(8) - (8)].stat));
            DBG((yyval.stat), (yylsp[(1) - (8)]), (yylsp[(7) - (8)]));
        }
        break;

    case 293:
        /* Line 1792 of yacc.c  */
#line 792 "grammar.y"
        {
            (yyval.stat) = new ForInNode(*(yyvsp[(4) - (9)].ident), (yyvsp[(5) - (9)].init), (yyvsp[(7) - (9)].node), (yyvsp[(9) - (9)].stat));
            DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(8) - (9)]));
        }
        break;

    case 294:
        /* Line 1792 of yacc.c  */
#line 796 "grammar.y"
        {
            (yyval.node) = nullptr;
        }
        break;

    case 296:
        /* Line 1792 of yacc.c  */
#line 801 "grammar.y"
        {
            (yyval.node) = nullptr;
        }
        break;

    case 298:
        /* Line 1792 of yacc.c  */
#line 806 "grammar.y"
        {
            (yyval.stat) = new ContinueNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 299:
        /* Line 1792 of yacc.c  */
#line 807 "grammar.y"
        {
            (yyval.stat) = new ContinueNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
            AUTO_SEMICOLON;
        }
        break;

    case 300:
        /* Line 1792 of yacc.c  */
#line 808 "grammar.y"
        {
            (yyval.stat) = new ContinueNode(*(yyvsp[(2) - (3)].ident));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 301:
        /* Line 1792 of yacc.c  */
#line 809 "grammar.y"
        {
            (yyval.stat) = new ContinueNode(*(yyvsp[(2) - (3)].ident));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 302:
        /* Line 1792 of yacc.c  */
#line 813 "grammar.y"
        {
            (yyval.stat) = new BreakNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 303:
        /* Line 1792 of yacc.c  */
#line 814 "grammar.y"
        {
            (yyval.stat) = new BreakNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
            AUTO_SEMICOLON;
        }
        break;

    case 304:
        /* Line 1792 of yacc.c  */
#line 815 "grammar.y"
        {
            (yyval.stat) = new BreakNode(*(yyvsp[(2) - (3)].ident));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 305:
        /* Line 1792 of yacc.c  */
#line 816 "grammar.y"
        {
            (yyval.stat) = new BreakNode(*(yyvsp[(2) - (3)].ident));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 306:
        /* Line 1792 of yacc.c  */
#line 820 "grammar.y"
        {
            (yyval.stat) = new ReturnNode(nullptr);
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 307:
        /* Line 1792 of yacc.c  */
#line 821 "grammar.y"
        {
            (yyval.stat) = new ReturnNode(nullptr);
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
            AUTO_SEMICOLON;
        }
        break;

    case 308:
        /* Line 1792 of yacc.c  */
#line 822 "grammar.y"
        {
            (yyval.stat) = new ReturnNode((yyvsp[(2) - (3)].node));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 309:
        /* Line 1792 of yacc.c  */
#line 823 "grammar.y"
        {
            (yyval.stat) = new ReturnNode((yyvsp[(2) - (3)].node));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 310:
        /* Line 1792 of yacc.c  */
#line 827 "grammar.y"
        {
            (yyval.stat) = new WithNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].stat));
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)]));
        }
        break;

    case 311:
        /* Line 1792 of yacc.c  */
#line 831 "grammar.y"
        {
            (yyval.stat) = new SwitchNode((yyvsp[(3) - (5)].node), (yyvsp[(5) - (5)].cblk));
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(4) - (5)]));
        }
        break;

    case 312:
        /* Line 1792 of yacc.c  */
#line 835 "grammar.y"
        {
            (yyval.cblk) = new CaseBlockNode((yyvsp[(2) - (3)].clist), nullptr, nullptr);
        }
        break;

    case 313:
        /* Line 1792 of yacc.c  */
#line 837 "grammar.y"
        {
            (yyval.cblk) = new CaseBlockNode((yyvsp[(2) - (5)].clist), (yyvsp[(3) - (5)].ccl), (yyvsp[(4) - (5)].clist));
        }
        break;

    case 314:
        /* Line 1792 of yacc.c  */
#line 841 "grammar.y"
        {
            (yyval.clist) = nullptr;
        }
        break;

    case 316:
        /* Line 1792 of yacc.c  */
#line 846 "grammar.y"
        {
            (yyval.clist) = new ClauseListNode((yyvsp[(1) - (1)].ccl));
        }
        break;

    case 317:
        /* Line 1792 of yacc.c  */
#line 847 "grammar.y"
        {
            (yyval.clist) = new ClauseListNode((yyvsp[(1) - (2)].clist), (yyvsp[(2) - (2)].ccl));
        }
        break;

    case 318:
        /* Line 1792 of yacc.c  */
#line 851 "grammar.y"
        {
            (yyval.ccl) = new CaseClauseNode((yyvsp[(2) - (3)].node));
        }
        break;

    case 319:
        /* Line 1792 of yacc.c  */
#line 852 "grammar.y"
        {
            (yyval.ccl) = new CaseClauseNode((yyvsp[(2) - (4)].node), (yyvsp[(4) - (4)].srcs));
        }
        break;

    case 320:
        /* Line 1792 of yacc.c  */
#line 856 "grammar.y"
        {
            (yyval.ccl) = new CaseClauseNode(nullptr);
        }
        break;

    case 321:
        /* Line 1792 of yacc.c  */
#line 857 "grammar.y"
        {
            (yyval.ccl) = new CaseClauseNode(nullptr, (yyvsp[(3) - (3)].srcs));
        }
        break;

    case 322:
        /* Line 1792 of yacc.c  */
#line 861 "grammar.y"
        {
            (yyval.stat) = makeLabelNode(*(yyvsp[(1) - (3)].ident), (yyvsp[(3) - (3)].stat));
        }
        break;

    case 323:
        /* Line 1792 of yacc.c  */
#line 865 "grammar.y"
        {
            (yyval.stat) = new ThrowNode((yyvsp[(2) - (3)].node));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 324:
        /* Line 1792 of yacc.c  */
#line 866 "grammar.y"
        {
            (yyval.stat) = new ThrowNode((yyvsp[(2) - (3)].node));
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(2) - (3)]));
            AUTO_SEMICOLON;
        }
        break;

    case 325:
        /* Line 1792 of yacc.c  */
#line 870 "grammar.y"
        {
            (yyval.stat) = new TryNode((yyvsp[(2) - (4)].stat), CommonIdentifiers::shared()->nullIdentifier, nullptr, (yyvsp[(4) - (4)].stat));
            DBG((yyval.stat), (yylsp[(1) - (4)]), (yylsp[(2) - (4)]));
        }
        break;

    case 326:
        /* Line 1792 of yacc.c  */
#line 871 "grammar.y"
        {
            (yyval.stat) = new TryNode((yyvsp[(2) - (7)].stat), *(yyvsp[(5) - (7)].ident), (yyvsp[(7) - (7)].stat), nullptr);
            DBG((yyval.stat), (yylsp[(1) - (7)]), (yylsp[(2) - (7)]));
        }
        break;

    case 327:
        /* Line 1792 of yacc.c  */
#line 873 "grammar.y"
        {
            (yyval.stat) = new TryNode((yyvsp[(2) - (9)].stat), *(yyvsp[(5) - (9)].ident), (yyvsp[(7) - (9)].stat), (yyvsp[(9) - (9)].stat));
            DBG((yyval.stat), (yylsp[(1) - (9)]), (yylsp[(2) - (9)]));
        }
        break;

    case 328:
        /* Line 1792 of yacc.c  */
#line 877 "grammar.y"
        {
            (yyval.stat) = new EmptyStatementNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 329:
        /* Line 1792 of yacc.c  */
#line 878 "grammar.y"
        {
            (yyval.stat) = new EmptyStatementNode();
            DBG((yyval.stat), (yylsp[(1) - (2)]), (yylsp[(1) - (2)]));
            AUTO_SEMICOLON;
        }
        break;

    case 330:
        /* Line 1792 of yacc.c  */
#line 882 "grammar.y"
        {
            (yyval.pkgn) = new PackageNameNode(*(yyvsp[(1) - (1)].ident));
        }
        break;

    case 331:
        /* Line 1792 of yacc.c  */
#line 883 "grammar.y"
        {
            (yyval.pkgn) = new PackageNameNode((yyvsp[(1) - (3)].pkgn), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 332:
        /* Line 1792 of yacc.c  */
#line 887 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(2) - (5)].pkgn), true, nullptr);
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
        }
        break;

    case 333:
        /* Line 1792 of yacc.c  */
#line 889 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(2) - (5)].pkgn), true, nullptr);
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); AUTO_SEMICOLON;
        }
        break;

    case 334:
        /* Line 1792 of yacc.c  */
#line 891 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(2) - (3)].pkgn), false, nullptr);
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 335:
        /* Line 1792 of yacc.c  */
#line 893 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(2) - (3)].pkgn), false, nullptr);
            DBG((yyval.stat), (yylsp[(1) - (3)]), (yylsp[(3) - (3)])); AUTO_SEMICOLON;
        }
        break;

    case 336:
        /* Line 1792 of yacc.c  */
#line 895 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(4) - (5)].pkgn), false, *(yyvsp[(2) - (5)].ident));
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)]));
        }
        break;

    case 337:
        /* Line 1792 of yacc.c  */
#line 897 "grammar.y"
        {
            (yyval.stat) = makeImportNode((yyvsp[(4) - (5)].pkgn), false, *(yyvsp[(2) - (5)].ident));
            DBG((yyval.stat), (yylsp[(1) - (5)]), (yylsp[(5) - (5)])); AUTO_SEMICOLON;
        }
        break;

    case 338:
        /* Line 1792 of yacc.c  */
#line 902 "grammar.y"
        {
            inFuncDecl();
        }
        break;

    case 339:
        /* Line 1792 of yacc.c  */
#line 902 "grammar.y"
        {
            (yyval.func) = new FuncDeclNode(*(yyvsp[(2) - (6)].ident), (yyvsp[(6) - (6)].body));
        }
        break;

    case 340:
        /* Line 1792 of yacc.c  */
#line 903 "grammar.y"
        {
            inFuncDecl();
        }
        break;

    case 341:
        /* Line 1792 of yacc.c  */
#line 904 "grammar.y"
        {
            (yyval.func) = new FuncDeclNode(*(yyvsp[(2) - (7)].ident), (yyvsp[(4) - (7)].param), (yyvsp[(7) - (7)].body));
        }
        break;

    case 342:
        /* Line 1792 of yacc.c  */
#line 908 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 343:
        /* Line 1792 of yacc.c  */
#line 908 "grammar.y"
        {
            (yyval.funcExpr) = new FuncExprNode(CommonIdentifiers::shared()->nullIdentifier, (yyvsp[(5) - (5)].body));
        }
        break;

    case 344:
        /* Line 1792 of yacc.c  */
#line 911 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 345:
        /* Line 1792 of yacc.c  */
#line 911 "grammar.y"
        {
            (yyval.funcExpr) = new FuncExprNode(CommonIdentifiers::shared()->nullIdentifier, (yyvsp[(6) - (6)].body), (yyvsp[(3) - (6)].param));
        }
        break;

    case 346:
        /* Line 1792 of yacc.c  */
#line 914 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 347:
        /* Line 1792 of yacc.c  */
#line 914 "grammar.y"
        {
            (yyval.funcExpr) = new FuncExprNode(*(yyvsp[(2) - (6)].ident), (yyvsp[(6) - (6)].body));
        }
        break;

    case 348:
        /* Line 1792 of yacc.c  */
#line 915 "grammar.y"
        {
            inFuncExpr();
        }
        break;

    case 349:
        /* Line 1792 of yacc.c  */
#line 915 "grammar.y"
        {
            (yyval.funcExpr) = new FuncExprNode(*(yyvsp[(2) - (7)].ident), (yyvsp[(7) - (7)].body), (yyvsp[(4) - (7)].param));
        }
        break;

    case 350:
        /* Line 1792 of yacc.c  */
#line 921 "grammar.y"
        {
            (yyval.param) = new ParameterNode(*(yyvsp[(1) - (1)].ident));
        }
        break;

    case 351:
        /* Line 1792 of yacc.c  */
#line 922 "grammar.y"
        {
            (yyval.param) = new ParameterNode((yyvsp[(1) - (3)].param), *(yyvsp[(3) - (3)].ident));
        }
        break;

    case 352:
        /* Line 1792 of yacc.c  */
#line 926 "grammar.y"
        {
            (yyval.body) = new FunctionBodyNode(nullptr);
            DBG((yyval.body), (yylsp[(1) - (2)]), (yylsp[(2) - (2)]));
        }
        break;

    case 353:
        /* Line 1792 of yacc.c  */
#line 927 "grammar.y"
        {
            (yyval.body) = new FunctionBodyNode((yyvsp[(2) - (3)].srcs));
            DBG((yyval.body), (yylsp[(1) - (3)]), (yylsp[(3) - (3)]));
        }
        break;

    case 354:
        /* Line 1792 of yacc.c  */
#line 931 "grammar.y"
        {
            parser().didFinishParsing(new ProgramNode(nullptr));
        }
        break;

    case 355:
        /* Line 1792 of yacc.c  */
#line 932 "grammar.y"
        {
            parser().didFinishParsing(new ProgramNode((yyvsp[(1) - (1)].srcs)));
        }
        break;

    case 356:
        /* Line 1792 of yacc.c  */
#line 936 "grammar.y"
        {
            (yyval.srcs) = new SourceElementsNode((yyvsp[(1) - (1)].stat));
        }
        break;

    case 357:
        /* Line 1792 of yacc.c  */
#line 937 "grammar.y"
        {
            (yyval.srcs) = new SourceElementsNode((yyvsp[(1) - (2)].srcs), (yyvsp[(2) - (2)].stat));
        }
        break;

    case 358:
        /* Line 1792 of yacc.c  */
#line 941 "grammar.y"
        {
            (yyval.stat) = (yyvsp[(1) - (1)].func);
        }
        break;

    case 359:
        /* Line 1792 of yacc.c  */
#line 942 "grammar.y"
        {
            (yyval.stat) = (yyvsp[(1) - (1)].stat);
        }
        break;

        /* Line 1792 of yacc.c  */
#line 4178 "grammar.cpp"
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
    YY_SYMBOL_PRINT("-> $$ =", yyr1[yyn], &yyval, &yyloc);

    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);

    *++yyvsp = yyval;
    *++yylsp = yyloc;

    /* Now `shift' the result of the reduction.  Determine what state
       that goes to, based on the state we popped back to and the rule
       number reduced by.  */

    yyn = yyr1[yyn];

    yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
    if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp) {
        yystate = yytable[yystate];
    } else {
        yystate = yydefgoto[yyn - YYNTOKENS];
    }

    goto yynewstate;

    /*------------------------------------.
    | yyerrlab -- here on detecting error |
    `------------------------------------*/
yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE(yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus) {
        ++yynerrs;
#if ! YYERROR_VERBOSE
        yyerror(YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
        {
            char const *yymsgp = YY_("syntax error");
            int yysyntax_error_status;
            yysyntax_error_status = YYSYNTAX_ERROR;
            if (yysyntax_error_status == 0) {
                yymsgp = yymsg;
            } else if (yysyntax_error_status == 1) {
                if (yymsg != yymsgbuf) {
                    YYSTACK_FREE(yymsg);
                }
                yymsg = (char *) YYSTACK_ALLOC(yymsg_alloc);
                if (!yymsg) {
                    yymsg = yymsgbuf;
                    yymsg_alloc = sizeof yymsgbuf;
                    yysyntax_error_status = 2;
                } else {
                    yysyntax_error_status = YYSYNTAX_ERROR;
                    yymsgp = yymsg;
                }
            }
            yyerror(yymsgp);
            if (yysyntax_error_status == 2) {
                goto yyexhaustedlab;
            }
        }
# undef YYSYNTAX_ERROR
#endif
    }

    yyerror_range[1] = yylloc;

    if (yyerrstatus == 3) {
        /* If just tried and failed to reuse lookahead token after an
        error, discard it.  */

        if (yychar <= YYEOF) {
            /* Return failure if at end of input.  */
            if (yychar == YYEOF) {
                YYABORT;
            }
        } else {
            yydestruct("Error: discarding",
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
    if (/*CONSTCOND*/ 0) {
        goto yyerrorlab;
    }

    yyerror_range[1] = yylsp[1 - yylen];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);
    yystate = *yyssp;
    goto yyerrlab1;

    /*-------------------------------------------------------------.
    | yyerrlab1 -- common code for both syntax error and YYERROR.  |
    `-------------------------------------------------------------*/
yyerrlab1:
    yyerrstatus = 3;  /* Each real token shifted decrements this.  */

    for (;;) {
        yyn = yypact[yystate];
        if (!yypact_value_is_default(yyn)) {
            yyn += YYTERROR;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR) {
                yyn = yytable[yyn];
                if (0 < yyn) {
                    break;
                }
            }
        }

        /* Pop the current state because it cannot handle the error token.  */
        if (yyssp == yyss) {
            YYABORT;
        }

        yyerror_range[1] = *yylsp;
        yydestruct("Error: popping",
                   yystos[yystate], yyvsp, yylsp);
        YYPOPSTACK(1);
        yystate = *yyssp;
        YY_STACK_PRINT(yyss, yyssp);
    }

    YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
    *++yyvsp = yylval;
    YY_IGNORE_MAYBE_UNINITIALIZED_END

    yyerror_range[2] = yylloc;
    /* Using YYLLOC is tempting, but would change the location of
       the lookahead.  YYLOC is available though.  */
    YYLLOC_DEFAULT(yyloc, yyerror_range, 2);
    *++yylsp = yyloc;

    /* Shift the error token.  */
    YY_SYMBOL_PRINT("Shifting", yystos[yyn], yyvsp, yylsp);

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
    yyerror(YY_("memory exhausted"));
    yyresult = 2;
    /* Fall through.  */
#endif

yyreturn:
    if (yychar != YYEMPTY) {
        /* Make sure we have latest lookahead translation.  See comments at
           user semantic actions for why this is necessary.  */
        yytoken = YYTRANSLATE(yychar);
        yydestruct("Cleanup: discarding lookahead",
                   yytoken, &yylval, &yylloc);
    }
    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    YYPOPSTACK(yylen);
    YY_STACK_PRINT(yyss, yyssp);
    while (yyssp != yyss) {
        yydestruct("Cleanup: popping",
                   yystos[*yyssp], yyvsp, yylsp);
        YYPOPSTACK(1);
    }
#ifndef yyoverflow
    if (yyss != yyssa) {
        YYSTACK_FREE(yyss);
    }
#endif
#if YYERROR_VERBOSE
    if (yymsg != yymsgbuf) {
        YYSTACK_FREE(yymsg);
    }
#endif
    /* Make sure YYID is used.  */
    return YYID(yyresult);
}

/* Line 2055 of yacc.c  */
#line 945 "grammar.y"

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

