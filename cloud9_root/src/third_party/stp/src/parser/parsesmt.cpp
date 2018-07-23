/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         smtparse
#define yylex           smtlex
#define yyerror         smterror
#define yylval          smtlval
#define yychar          smtchar
#define yydebug         smtdebug
#define yynerrs         smtnerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "smt.y"

  /********************************************************************
   * AUTHORS: Vijay Ganesh, Trevor Hansen
   *
   * BEGIN DATE: July, 2006
   *
   * This file is modified version of the CVCL's smtlib.y file. Please
   * see CVCL license below
   ********************************************************************/
  
  /********************************************************************
   *
   * \file smtlib.y
   * 
   * Author: Sergey Berezin, Clark Barrett
   * 
   * Created: Apr 30 2005
   *
   * <hr>
   * Copyright (C) 2004 by the Board of Trustees of Leland Stanford
   * Junior University and by New York University. 
   *
   * License to use, copy, modify, sell and/or distribute this software
   * and its documentation for any purpose is hereby granted without
   * royalty, subject to the terms and conditions defined in the \ref
   * LICENSE file provided with this distribution.  In particular:
   *
   * - The above copyright notice and this permission notice must appear
   * in all copies of the software and related documentation.
   *
   * - THE SOFTWARE IS PROVIDED "AS-IS", WITHOUT ANY WARRANTIES,
   * EXPRESSED OR IMPLIED.  USE IT AT YOUR OWN RISK.
   * 
   * <hr>
   ********************************************************************/
  // -*- c++ -*-

#include "ParserInterface.h"

  using namespace std; 
  using namespace BEEV;

  // Suppress the bogus warning suppression in bison (it generates
  // compile error)
#undef __GNUC_MINOR__
  
  extern char* smttext;
  extern int smtlineno;
  extern int smtlex(void);

  int yyerror(const char *s) {
    cout << "syntax error: line " << smtlineno << "\n" << s << endl;
    cout << "  token: " << smttext << endl;
    FatalError("");
    return 1;
  }

  ASTNode query;
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 104857600
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1
#define YYPARSE_PARAM AssertsQuery
  

/* Line 268 of yacc.c  */
#line 145 "parsesmt.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMERAL_TOK = 258,
     BVCONST_TOK = 259,
     BITCONST_TOK = 260,
     FORMID_TOK = 261,
     TERMID_TOK = 262,
     STRING_TOK = 263,
     USER_VAL_TOK = 264,
     SOURCE_TOK = 265,
     CATEGORY_TOK = 266,
     DIFFICULTY_TOK = 267,
     BITVEC_TOK = 268,
     ARRAY_TOK = 269,
     SELECT_TOK = 270,
     STORE_TOK = 271,
     TRUE_TOK = 272,
     FALSE_TOK = 273,
     NOT_TOK = 274,
     IMPLIES_TOK = 275,
     ITE_TOK = 276,
     AND_TOK = 277,
     OR_TOK = 278,
     XOR_TOK = 279,
     IFF_TOK = 280,
     EXISTS_TOK = 281,
     FORALL_TOK = 282,
     LET_TOK = 283,
     FLET_TOK = 284,
     NOTES_TOK = 285,
     CVC_COMMAND_TOK = 286,
     SORTS_TOK = 287,
     FUNS_TOK = 288,
     PREDS_TOK = 289,
     EXTENSIONS_TOK = 290,
     DEFINITION_TOK = 291,
     AXIOMS_TOK = 292,
     LOGIC_TOK = 293,
     COLON_TOK = 294,
     LBRACKET_TOK = 295,
     RBRACKET_TOK = 296,
     LPAREN_TOK = 297,
     RPAREN_TOK = 298,
     SAT_TOK = 299,
     UNSAT_TOK = 300,
     UNKNOWN_TOK = 301,
     ASSUMPTION_TOK = 302,
     FORMULA_TOK = 303,
     STATUS_TOK = 304,
     BENCHMARK_TOK = 305,
     EXTRASORTS_TOK = 306,
     EXTRAFUNS_TOK = 307,
     EXTRAPREDS_TOK = 308,
     LANGUAGE_TOK = 309,
     DOLLAR_TOK = 310,
     QUESTION_TOK = 311,
     DISTINCT_TOK = 312,
     SEMICOLON_TOK = 313,
     EOF_TOK = 314,
     EQ_TOK = 315,
     NAND_TOK = 316,
     NOR_TOK = 317,
     NEQ_TOK = 318,
     ASSIGN_TOK = 319,
     BV_TOK = 320,
     BOOLEAN_TOK = 321,
     BVLEFTSHIFT_1_TOK = 322,
     BVRIGHTSHIFT_1_TOK = 323,
     BVARITHRIGHTSHIFT_TOK = 324,
     BVPLUS_TOK = 325,
     BVSUB_TOK = 326,
     BVNOT_TOK = 327,
     BVMULT_TOK = 328,
     BVDIV_TOK = 329,
     SBVDIV_TOK = 330,
     BVMOD_TOK = 331,
     SBVREM_TOK = 332,
     SBVMOD_TOK = 333,
     BVNEG_TOK = 334,
     BVAND_TOK = 335,
     BVOR_TOK = 336,
     BVXOR_TOK = 337,
     BVNAND_TOK = 338,
     BVNOR_TOK = 339,
     BVXNOR_TOK = 340,
     BVCONCAT_TOK = 341,
     BVLT_TOK = 342,
     BVGT_TOK = 343,
     BVLE_TOK = 344,
     BVGE_TOK = 345,
     BVSLT_TOK = 346,
     BVSGT_TOK = 347,
     BVSLE_TOK = 348,
     BVSGE_TOK = 349,
     BVSX_TOK = 350,
     BVZX_TOK = 351,
     BVROTATE_RIGHT_TOK = 352,
     BVROTATE_LEFT_TOK = 353,
     BVREPEAT_TOK = 354,
     BVCOMP_TOK = 355,
     BOOLEXTRACT_TOK = 356,
     BOOL_TO_BV_TOK = 357,
     BVEXTRACT_TOK = 358
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 66 "smt.y"
  
  // FIXME: Why is this not an UNSIGNED int?
  int uintval;                  /* for numerals in types. */

  // for BV32 BVCONST 
  unsigned long long ullval;

  struct {
    //stores the indexwidth and valuewidth
    //indexwidth is 0 iff type is bitvector. positive iff type is
    //array, and stores the width of the indexing bitvector
    unsigned int indexwidth;
    //width of the bitvector type
    unsigned int valuewidth;
  } indexvaluewidth;

  //ASTNode,ASTVec
  BEEV::ASTNode *node;
  BEEV::ASTVec *vec;
  std::string *str;



/* Line 293 of yacc.c  */
#line 308 "parsesmt.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 320 "parsesmt.cpp"

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
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
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
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

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
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   431

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  101
/* YYNRULES -- Number of states.  */
#define YYNSTATES  271

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   358

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,    11,    13,    15,    18,    22,    26,
      30,    34,    40,    46,    48,    53,    55,    57,    59,    61,
      63,    66,    68,    71,    74,    77,    79,    82,    84,    87,
      92,    96,    98,   101,   103,   105,   107,   113,   118,   124,
     130,   136,   142,   148,   154,   160,   166,   170,   175,   181,
     188,   193,   198,   204,   210,   214,   222,   230,   232,   235,
     237,   242,   244,   246,   248,   252,   256,   261,   269,   274,
     278,   281,   284,   288,   292,   296,   300,   304,   308,   311,
     314,   318,   322,   326,   330,   334,   338,   342,   346,   350,
     354,   360,   366,   372,   378,   384,   389,   396,   398,   400,
     403,   406
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
     105,     0,    -1,   106,    -1,    42,    50,   107,   108,    43,
      -1,     8,    -1,   109,    -1,   108,   109,    -1,    39,    47,
     119,    -1,    39,    48,   119,    -1,    39,    49,   111,    -1,
      39,    38,   110,    -1,    39,    52,    42,   116,    43,    -1,
      39,    53,    42,   116,    43,    -1,   112,    -1,     8,    40,
       3,    41,    -1,     8,    -1,    44,    -1,    45,    -1,    46,
      -1,   114,    -1,   114,   113,    -1,     9,    -1,    39,    10,
      -1,    39,    11,    -1,    39,    12,    -1,   124,    -1,   124,
     124,    -1,   117,    -1,   116,   117,    -1,    42,     8,   115,
      43,    -1,    42,     8,    43,    -1,   119,    -1,   118,   119,
      -1,    17,    -1,    18,    -1,   126,    -1,    42,    60,   122,
     122,    43,    -1,    42,    57,   121,    43,    -1,    42,    91,
     122,   122,    43,    -1,    42,    93,   122,   122,    43,    -1,
      42,    92,   122,   122,    43,    -1,    42,    94,   122,   122,
      43,    -1,    42,    87,   122,   122,    43,    -1,    42,    89,
     122,   122,    43,    -1,    42,    88,   122,   122,    43,    -1,
      42,    90,   122,   122,    43,    -1,    42,   119,    43,    -1,
      42,    19,   119,    43,    -1,    42,    20,   119,   119,    43,
      -1,    42,    21,   119,   119,   119,    43,    -1,    42,    22,
     118,    43,    -1,    42,    23,   118,    43,    -1,    42,    24,
     119,   119,    43,    -1,    42,    25,   119,   119,    43,    -1,
     120,   119,    43,    -1,    42,    28,    42,    56,     8,   122,
      43,    -1,    42,    29,    42,    55,     8,   119,    43,    -1,
     122,    -1,   121,   122,    -1,     4,    -1,     4,    40,     3,
      41,    -1,   123,    -1,     5,    -1,   125,    -1,    42,   122,
      43,    -1,    15,   122,   122,    -1,    16,   122,   122,   122,
      -1,   103,    40,     3,    39,     3,    41,   122,    -1,    21,
     119,   122,   122,    -1,    86,   122,   122,    -1,    72,   122,
      -1,    79,   122,    -1,    80,   122,   122,    -1,    81,   122,
     122,    -1,    82,   122,   122,    -1,    85,   122,   122,    -1,
     100,   122,   122,    -1,    71,   122,   122,    -1,    70,   121,
      -1,    73,   121,    -1,    74,   122,   122,    -1,    76,   122,
     122,    -1,    75,   122,   122,    -1,    77,   122,   122,    -1,
      78,   122,   122,    -1,    83,   122,   122,    -1,    84,   122,
     122,    -1,    67,   122,   122,    -1,    68,   122,   122,    -1,
      69,   122,   122,    -1,    98,    40,     3,    41,   122,    -1,
      97,    40,     3,    41,   122,    -1,    99,    40,     3,    41,
     122,    -1,    95,    40,     3,    41,   122,    -1,    96,    40,
       3,    41,   122,    -1,    13,    40,     3,    41,    -1,    14,
      40,     3,    39,     3,    41,    -1,     6,    -1,     7,    -1,
      56,     7,    -1,    55,     6,    -1,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   208,   208,   235,   257,   263,   272,   284,   289,   300,
     304,   315,   319,   323,   330,   334,   341,   345,   349,   367,
     370,   376,   384,   387,   390,   394,   400,   411,   414,   421,
     431,   444,   453,   464,   470,   476,   480,   487,   514,   522,
     530,   538,   546,   554,   562,   570,   578,   582,   587,   593,
     600,   605,   610,   616,   622,   632,   647,   656,   665,   676,
     681,   686,   693,   694,   699,   703,   716,   730,   746,   754,
     762,   770,   778,   786,   794,   802,   821,   836,   844,   852,
     860,   869,   878,   887,   895,   903,   911,   919,   928,   937,
     946,   977,  1008,  1024,  1032,  1049,  1064,  1085,  1090,  1095,
    1102,  1106
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMERAL_TOK", "BVCONST_TOK",
  "BITCONST_TOK", "FORMID_TOK", "TERMID_TOK", "STRING_TOK", "USER_VAL_TOK",
  "SOURCE_TOK", "CATEGORY_TOK", "DIFFICULTY_TOK", "BITVEC_TOK",
  "ARRAY_TOK", "SELECT_TOK", "STORE_TOK", "TRUE_TOK", "FALSE_TOK",
  "NOT_TOK", "IMPLIES_TOK", "ITE_TOK", "AND_TOK", "OR_TOK", "XOR_TOK",
  "IFF_TOK", "EXISTS_TOK", "FORALL_TOK", "LET_TOK", "FLET_TOK",
  "NOTES_TOK", "CVC_COMMAND_TOK", "SORTS_TOK", "FUNS_TOK", "PREDS_TOK",
  "EXTENSIONS_TOK", "DEFINITION_TOK", "AXIOMS_TOK", "LOGIC_TOK",
  "COLON_TOK", "LBRACKET_TOK", "RBRACKET_TOK", "LPAREN_TOK", "RPAREN_TOK",
  "SAT_TOK", "UNSAT_TOK", "UNKNOWN_TOK", "ASSUMPTION_TOK", "FORMULA_TOK",
  "STATUS_TOK", "BENCHMARK_TOK", "EXTRASORTS_TOK", "EXTRAFUNS_TOK",
  "EXTRAPREDS_TOK", "LANGUAGE_TOK", "DOLLAR_TOK", "QUESTION_TOK",
  "DISTINCT_TOK", "SEMICOLON_TOK", "EOF_TOK", "EQ_TOK", "NAND_TOK",
  "NOR_TOK", "NEQ_TOK", "ASSIGN_TOK", "BV_TOK", "BOOLEAN_TOK",
  "BVLEFTSHIFT_1_TOK", "BVRIGHTSHIFT_1_TOK", "BVARITHRIGHTSHIFT_TOK",
  "BVPLUS_TOK", "BVSUB_TOK", "BVNOT_TOK", "BVMULT_TOK", "BVDIV_TOK",
  "SBVDIV_TOK", "BVMOD_TOK", "SBVREM_TOK", "SBVMOD_TOK", "BVNEG_TOK",
  "BVAND_TOK", "BVOR_TOK", "BVXOR_TOK", "BVNAND_TOK", "BVNOR_TOK",
  "BVXNOR_TOK", "BVCONCAT_TOK", "BVLT_TOK", "BVGT_TOK", "BVLE_TOK",
  "BVGE_TOK", "BVSLT_TOK", "BVSGT_TOK", "BVSLE_TOK", "BVSGE_TOK",
  "BVSX_TOK", "BVZX_TOK", "BVROTATE_RIGHT_TOK", "BVROTATE_LEFT_TOK",
  "BVREPEAT_TOK", "BVCOMP_TOK", "BOOLEXTRACT_TOK", "BOOL_TO_BV_TOK",
  "BVEXTRACT_TOK", "$accept", "cmd", "benchmark", "bench_name",
  "bench_attributes", "bench_attribute", "logic_name", "status",
  "annotation", "user_value", "attribute", "sort_symbs", "var_decls",
  "var_decl", "an_formulas", "an_formula", "letexpr_mgmt", "an_terms",
  "an_term", "an_nonbvconst_term", "sort_symb", "var", "fvar", 0
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   104,   105,   106,   107,   108,   108,   109,   109,   109,
     109,   109,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   113,   114,   114,   114,   115,   115,   116,   116,   117,
     117,   118,   118,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   120,   120,   121,   121,   122,
     122,   122,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   124,   124,   125,   125,   125,
     126,   126
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     5,     1,     1,     2,     3,     3,     3,
       3,     5,     5,     1,     4,     1,     1,     1,     1,     1,
       2,     1,     2,     2,     2,     1,     2,     1,     2,     4,
       3,     1,     2,     1,     1,     1,     5,     4,     5,     5,
       5,     5,     5,     5,     5,     5,     3,     4,     5,     6,
       4,     4,     5,     5,     3,     7,     7,     1,     2,     1,
       4,     1,     1,     1,     3,     3,     4,     7,     4,     3,
       2,     2,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     5,     5,     5,     5,     4,     6,     1,     1,     2,
       2,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     0,     1,     4,     0,     0,     0,
       5,    13,    19,    22,    23,    24,     0,     0,     0,     0,
       0,     0,     3,     6,    21,    20,    15,    10,   101,    33,
      34,     0,     0,     7,     0,    35,     8,    16,    17,    18,
       9,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   100,     0,     0,     0,    27,     0,
       0,     0,     0,     0,     0,    31,     0,     0,     0,     0,
       0,    59,    62,    97,    98,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    57,    61,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    54,     0,    11,    28,    12,    14,    47,     0,     0,
      50,    32,    51,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,     0,     0,     0,    78,     0,    70,    79,
       0,     0,     0,     0,     0,    71,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    58,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,    25,    48,     0,    52,    53,
       0,     0,     0,    65,     0,     0,    64,    87,    88,    89,
      77,    80,    82,    81,    83,    84,    72,    73,    74,    85,
      86,    75,    69,     0,     0,     0,     0,     0,    76,     0,
      36,    42,    44,    43,    45,    38,    40,    39,    41,     0,
       0,    29,    26,    49,     0,     0,    60,    66,    68,     0,
       0,     0,     0,     0,     0,     0,     0,    55,    56,    93,
      94,    91,    90,    92,     0,    95,     0,     0,     0,    67,
      96
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     7,     9,    10,    27,    40,    11,    25,
      12,   194,    67,    68,    74,    75,    34,   117,   118,   119,
     195,   120,    35
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -81
static const yytype_int16 yypact[] =
{
     -16,   -23,    28,   -81,    21,   -81,   -81,    -9,   248,   -28,
     -81,   -81,    24,   -81,   -81,   -81,    27,   197,   197,   -27,
       0,    14,   -81,   -81,   -81,   -81,    17,   -81,   -81,   -81,
     -81,   337,    52,   -81,   197,   -81,   -81,   -81,   -81,   -81,
     -81,    18,    18,    56,   197,   197,   197,   197,   197,   197,
     197,    20,    22,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,    23,   -81,    33,    57,   -22,   -81,   -20,
      36,    35,   197,   197,    80,   -81,   127,   197,   197,    25,
      29,    39,   -81,   -81,   -81,   250,   250,   197,   250,    73,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
      42,    43,    45,    47,    48,   250,    49,   150,   -81,   -81,
     -81,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     -81,   -81,    -4,   -81,   -81,   -81,   -81,   -81,    50,   197,
     -81,   -81,   -81,    61,    68,    82,    83,    89,   250,   250,
     250,    76,   -81,   250,   250,   250,   250,   250,   -81,   250,
     250,   250,   250,   250,   250,   -81,   250,   250,   250,   250,
     250,   250,   250,   122,   123,   124,   125,   126,   250,   128,
     -81,   -81,    87,    93,    95,    96,    97,    98,    99,   100,
     115,    92,   119,   -81,   117,    11,   -81,   118,   -81,   -81,
     250,   197,   121,   -81,   250,   250,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,   131,   132,   133,   137,   142,   -81,   129,
     -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   -81,   160,
     161,   -81,   -81,   -81,   141,   143,   -81,   -81,   -81,   250,
     250,   250,   250,   250,   164,   144,   148,   -81,   -81,   -81,
     -81,   -81,   -81,   -81,   147,   -81,   186,   250,   153,   -81,
     -81
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -81,   -81,   -81,   -81,   -81,   182,   -81,   -81,   -81,   -81,
     -81,   -81,   158,   -55,   154,   103,   -81,   -80,   -54,   -81,
       6,   -81,   -81
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     121,   122,   123,   124,   125,   126,   127,   128,   129,   191,
     192,     8,   134,   156,   134,    22,   159,    37,    38,    39,
      66,   133,    66,   135,   191,   192,     1,     4,     5,     6,
       8,   148,   149,    24,   151,    26,   153,   154,   155,   193,
     157,   158,    41,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,    42,    43,    64,    70,
      66,   178,    79,   181,    80,   132,   130,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   131,   136,   137,   147,
     152,   145,   173,   174,   146,   175,    28,   176,   177,   179,
     200,   201,   202,   196,   203,   204,   205,    29,    30,   207,
     208,   209,   181,   210,   198,   181,   211,   212,   213,   214,
     215,   199,   216,   217,   218,   219,   220,   221,   222,   206,
      33,    36,    31,   140,   228,   223,   224,   225,   226,   227,
     230,   229,   239,    28,    63,    32,   231,    65,   232,   233,
     234,   235,   236,   237,    29,    30,   244,    71,    72,    73,
     247,   248,    77,    78,    81,    82,    83,    84,   238,   240,
     241,   243,   246,   255,   256,    85,    86,   264,   254,    31,
     142,    87,   249,   250,   251,   138,   139,   141,   252,   141,
     143,   144,    32,   253,   257,   265,   258,   266,   267,   268,
     150,    23,    88,   180,   270,   259,   260,   261,   262,   263,
      69,   242,    76,    28,     0,     0,    89,     0,     0,     0,
       0,     0,     0,   269,    29,    30,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,    31,
       0,     0,   197,     0,     0,   110,   111,   112,   113,   114,
     115,     0,    32,   116,    81,    82,    83,    84,    13,    14,
      15,     0,     0,     0,     0,    85,    86,     0,     0,     0,
       0,    87,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    16,     0,     0,     0,
       0,     0,    88,     0,     0,    17,    18,    19,     0,     0,
      20,    21,     0,     0,   245,     0,    89,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,     0,     0,    28,     0,   110,   111,   112,   113,   114,
     115,     0,     0,   116,    29,    30,    44,    45,    46,    47,
      48,    49,    50,     0,     0,    51,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,     0,    53,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    55,    56,    57,    58,    59,    60,
      61,    62
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-81))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      54,    55,    56,    57,    58,    59,    60,    61,    62,    13,
      14,    39,    67,    93,    69,    43,    96,    44,    45,    46,
      42,    43,    42,    43,    13,    14,    42,    50,     0,     8,
      39,    85,    86,     9,    88,     8,    90,    91,    92,    43,
      94,    95,    42,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,    42,    40,     6,     3,
      42,   115,    42,   117,    42,     8,    43,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    43,    41,    43,    40,
       7,    56,    40,    40,    55,    40,     6,    40,    40,    40,
       8,     8,     3,    43,   148,   149,   150,    17,    18,   153,
     154,   155,   156,   157,    43,   159,   160,   161,   162,   163,
     164,    43,   166,   167,   168,   169,   170,   171,   172,    43,
      17,    18,    42,    43,   178,     3,     3,     3,     3,     3,
      43,     3,    40,     6,    31,    55,    43,    34,    43,    43,
      43,    43,    43,    43,    17,    18,   200,    44,    45,    46,
     204,   205,    49,    50,     4,     5,     6,     7,    43,    40,
      43,    43,    41,     3,     3,    15,    16,     3,    39,    42,
      43,    21,    41,    41,    41,    72,    73,    74,    41,    76,
      77,    78,    55,    41,    43,    41,    43,    39,    41,     3,
      87,     9,    42,    43,    41,   249,   250,   251,   252,   253,
      42,   195,    48,     6,    -1,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,   267,    17,    18,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    -1,    -1,    42,
      -1,    -1,   139,    -1,    -1,    95,    96,    97,    98,    99,
     100,    -1,    55,   103,     4,     5,     6,     7,    10,    11,
      12,    -1,    -1,    -1,    -1,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    47,    48,    49,    -1,    -1,
      52,    53,    -1,    -1,   201,    -1,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,     6,    -1,    95,    96,    97,    98,    99,
     100,    -1,    -1,   103,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    57,    -1,    -1,    60,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    88,    89,    90,    91,    92,
      93,    94
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    42,   105,   106,    50,     0,     8,   107,    39,   108,
     109,   112,   114,    10,    11,    12,    38,    47,    48,    49,
      52,    53,    43,   109,     9,   113,     8,   110,     6,    17,
      18,    42,    55,   119,   120,   126,   119,    44,    45,    46,
     111,    42,    42,    40,    19,    20,    21,    22,    23,    24,
      25,    28,    29,    57,    60,    87,    88,    89,    90,    91,
      92,    93,    94,   119,     6,   119,    42,   116,   117,   116,
       3,   119,   119,   119,   118,   119,   118,   119,   119,    42,
      42,     4,     5,     6,     7,    15,    16,    21,    42,    56,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      95,    96,    97,    98,    99,   100,   103,   121,   122,   123,
     125,   122,   122,   122,   122,   122,   122,   122,   122,   122,
      43,    43,     8,    43,   117,    43,    41,    43,   119,   119,
      43,   119,    43,   119,   119,    56,    55,    40,   122,   122,
     119,   122,     7,   122,   122,   122,   121,   122,   122,   121,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,    40,    40,    40,    40,    40,   122,    40,
      43,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,    13,    14,    43,   115,   124,    43,   119,    43,    43,
       8,     8,     3,   122,   122,   122,    43,   122,   122,   122,
     122,   122,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,     3,     3,     3,     3,     3,   122,     3,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    40,
      40,    43,   124,    43,   122,   119,    41,   122,   122,    41,
      41,    41,    41,    41,    39,     3,     3,    43,    43,   122,
     122,   122,   122,   122,     3,    41,    39,    41,     3,   122,
      41
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

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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
		  Type, Value); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
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
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
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
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
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
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
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

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

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

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

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
  *++yyvsp = yylval;

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

/* Line 1806 of yacc.c  */
#line 209 "smt.y"
    {
  ASTNode assumptions;
  if((yyvsp[(1) - (1)].node) == NULL) 
    {
      assumptions = parserInterface->CreateNode(TRUE);
    } 
  else 
    {
      assumptions = *(yyvsp[(1) - (1)].node);
    }
      
  if(query.IsNull()) 
    {
      query = parserInterface->CreateNode(FALSE);
    }

  ((ASTVec*)AssertsQuery)->push_back(assumptions);
  ((ASTVec*)AssertsQuery)->push_back(query);
  delete (yyvsp[(1) - (1)].node);
  parserInterface->letMgr.cleanupParserSymbolTable();
  query = ASTNode();
  YYACCEPT;
}
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 236 "smt.y"
    {
  if((yyvsp[(4) - (5)].vec) != NULL){
    if((yyvsp[(4) - (5)].vec)->size() > 1) 
      (yyval.node) = new ASTNode(parserInterface->CreateNode(AND,*(yyvsp[(4) - (5)].vec)));
    else if((yyvsp[(4) - (5)].vec)->size() ==1)
      (yyval.node) = new ASTNode((*(yyvsp[(4) - (5)].vec))[0]);
     else
      (yyval.node) = new ASTNode(parserInterface->CreateNode(TRUE));     
    delete (yyvsp[(4) - (5)].vec);
  }
  else {
    (yyval.node) = NULL;
  }
  delete (yyvsp[(3) - (5)].str); //discard the benchmarkname.
}
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 258 "smt.y"
    {
}
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 264 "smt.y"
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[(1) - (1)].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
    parserInterface->AddAssert(*(yyvsp[(1) - (1)].node));
    delete (yyvsp[(1) - (1)].node);
  }
}
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 273 "smt.y"
    {
  if ((yyvsp[(1) - (2)].vec) != NULL && (yyvsp[(2) - (2)].node) != NULL) {
    (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
    parserInterface->AddAssert(*(yyvsp[(2) - (2)].node));
    (yyval.vec) = (yyvsp[(1) - (2)].vec);
    delete (yyvsp[(2) - (2)].node);
  }
}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 285 "smt.y"
    {
  //assumptions are like asserts
  (yyval.node) = (yyvsp[(3) - (3)].node);
}
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 290 "smt.y"
    {
  // Previously this would call AddQuery() on the negation.
  // But if multiple formula were (eroneously) present
  // it discarded all but the last formula. Allowing multiple 
  // formula and taking the conjunction of them along with all
  // the assumptions is what the other solvers do.  

  //assumptions are like asserts
  (yyval.node) = (yyvsp[(3) - (3)].node);
}
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 301 "smt.y"
    {
  (yyval.node) = NULL;
}
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 305 "smt.y"
    {
  if (!(0 == strcmp((yyvsp[(3) - (3)].str)->c_str(),"QF_UFBV")  ||
        0 == strcmp((yyvsp[(3) - (3)].str)->c_str(),"QF_BV") ||
        //0 == strcmp($3->c_str(),"QF_UF") ||
        0 == strcmp((yyvsp[(3) - (3)].str)->c_str(),"QF_AUFBV"))) {
    yyerror("Wrong input logic:");
  }
  delete (yyvsp[(3) - (3)].str);
  (yyval.node) = NULL;
}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 316 "smt.y"
    {
  (yyval.node) = NULL;
}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 320 "smt.y"
    {
  (yyval.node) = NULL;
}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 324 "smt.y"
    {
  (yyval.node) = NULL;
}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 331 "smt.y"
    {
  (yyval.str) = (yyvsp[(1) - (4)].str);
}
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 335 "smt.y"
    {
  (yyval.str) = (yyvsp[(1) - (1)].str);
}
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 341 "smt.y"
    { 
  input_status = TO_BE_SATISFIABLE; 
  (yyval.node) = NULL; 
}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 345 "smt.y"
    { 
  input_status = TO_BE_UNSATISFIABLE; 
  (yyval.node) = NULL; 
  }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 350 "smt.y"
    { 
  input_status = TO_BE_UNKNOWN; 
  (yyval.node) = NULL; 
}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 368 "smt.y"
    {
}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 371 "smt.y"
    {
}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 377 "smt.y"
    {
  //cerr << "Printing user_value: " << *$1 << endl;
  delete (yyvsp[(1) - (1)].str);
}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 385 "smt.y"
    {
}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 388 "smt.y"
    {
}
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 395 "smt.y"
    {
  //a single sort symbol here means either a BitVec or a Boolean
  (yyval.indexvaluewidth).indexwidth = (yyvsp[(1) - (1)].indexvaluewidth).indexwidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[(1) - (1)].indexvaluewidth).valuewidth;
}
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 401 "smt.y"
    {
  //two sort symbols mean f: type --> type
  (yyval.indexvaluewidth).indexwidth = (yyvsp[(1) - (2)].indexvaluewidth).valuewidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[(2) - (2)].indexvaluewidth).valuewidth;
}
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 412 "smt.y"
    {}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 415 "smt.y"
    {}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 422 "smt.y"
    {
  ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol((yyvsp[(2) - (4)].str)->c_str());
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  s.SetIndexWidth((yyvsp[(3) - (4)].indexvaluewidth).indexwidth);
  s.SetValueWidth((yyvsp[(3) - (4)].indexvaluewidth).valuewidth);
  parserInterface->letMgr._parser_symbol_table.insert(s);
  delete (yyvsp[(2) - (4)].str);
}
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 432 "smt.y"
    {
  ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol((yyvsp[(2) - (3)].str)->c_str());
  s.SetIndexWidth(0);
  s.SetValueWidth(0);
  parserInterface->letMgr._parser_symbol_table.insert(s);
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  delete (yyvsp[(2) - (3)].str);
}
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 445 "smt.y"
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[(1) - (1)].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
    delete (yyvsp[(1) - (1)].node);
  }
}
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 454 "smt.y"
    {
  if ((yyvsp[(1) - (2)].vec) != NULL && (yyvsp[(2) - (2)].node) != NULL) {
    (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
    (yyval.vec) = (yyvsp[(1) - (2)].vec);
    delete (yyvsp[(2) - (2)].node);
  }
}
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 465 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateNode(TRUE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 471 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateNode(FALSE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 477 "smt.y"
    {
  (yyval.node) = (yyvsp[(1) - (1)].node);
}
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 481 "smt.y"
    {
  ASTNode * n = new ASTNode(parserInterface->CreateNode(EQ,*(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 488 "smt.y"
    {
  using namespace BEEV;

  ASTVec terms = *(yyvsp[(3) - (4)].vec);
  ASTVec forms;

  for(ASTVec::const_iterator it=terms.begin(),itend=terms.end();
      it!=itend; it++) {
    for(ASTVec::const_iterator it2=it+1; it2!=itend; it2++) {
      ASTNode n = (parserInterface->nf->CreateNode(NOT, parserInterface->CreateNode(EQ, *it, *it2)));

          
      forms.push_back(n); 
    }
  }

  if(forms.size() == 0) 
    FatalError("empty distinct");
 
  (yyval.node) = (forms.size() == 1) ?
    new ASTNode(forms[0]) :
    new ASTNode(parserInterface->CreateNode(AND, forms));

  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 516 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVSLT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 524 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVSLE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 532 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVSGT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 540 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVSGE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 548 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVLT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 556 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVLE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 564 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVGT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 572 "smt.y"
    {
  ASTNode * n = parserInterface->newNode(BVGE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);      
}
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 579 "smt.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node);
}
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 583 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(NOT, *(yyvsp[(3) - (4)].node)));
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 588 "smt.y"
    {
  (yyval.node) = parserInterface->newNode(IMPLIES, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
}
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 594 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(ITE, *(yyvsp[(3) - (6)].node), *(yyvsp[(4) - (6)].node), *(yyvsp[(5) - (6)].node)));
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(4) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 601 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateNode(AND, *(yyvsp[(3) - (4)].vec)));
  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 606 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateNode(OR, *(yyvsp[(3) - (4)].vec)));
  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 611 "smt.y"
    {
  (yyval.node) = parserInterface->newNode(XOR, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
}
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 617 "smt.y"
    {
  (yyval.node) = parserInterface->newNode(IFF, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  delete (yyvsp[(3) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
}
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 624 "smt.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node);
  //Cleanup the LetIDToExprMap
  parserInterface->letMgr.CleanupLetIDMap();                      
}
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 633 "smt.y"
    {
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  parserInterface->letMgr.LetExprMgr(*(yyvsp[(5) - (7)].str),*(yyvsp[(6) - (7)].node));
  
  delete (yyvsp[(5) - (7)].str);
  delete (yyvsp[(6) - (7)].node);      
}
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 648 "smt.y"
    {
  //Do LET-expr management
  parserInterface->letMgr.LetExprMgr(*(yyvsp[(5) - (7)].str),*(yyvsp[(6) - (7)].node));
  delete (yyvsp[(5) - (7)].str);
  delete (yyvsp[(6) - (7)].node);     
}
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 657 "smt.y"
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[(1) - (1)].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
    delete (yyvsp[(1) - (1)].node);
  }
}
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 666 "smt.y"
    {
  if ((yyvsp[(1) - (2)].vec) != NULL && (yyvsp[(2) - (2)].node) != NULL) {
    (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
    (yyval.vec) = (yyvsp[(1) - (2)].vec);
    delete (yyvsp[(2) - (2)].node);
  }
}
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 677 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateBVConst(*(yyvsp[(1) - (1)].str), 10, 32));
  delete (yyvsp[(1) - (1)].str);
}
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 682 "smt.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateBVConst(*(yyvsp[(1) - (4)].str),10,(yyvsp[(3) - (4)].uintval)));
  delete (yyvsp[(1) - (4)].str);
}
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 687 "smt.y"
    {
(yyval.node) = (yyvsp[(1) - (1)].node);
}
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 693 "smt.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 695 "smt.y"
    {
  (yyval.node) = new ASTNode((*(yyvsp[(1) - (1)].node)));
  delete (yyvsp[(1) - (1)].node);
}
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 700 "smt.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node);
}
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 704 "smt.y"
    {
  //ARRAY READ
  // valuewidth is same as array, indexwidth is 0.
  ASTNode array = *(yyvsp[(2) - (3)].node);
  ASTNode index = *(yyvsp[(3) - (3)].node);
  unsigned int width = array.GetValueWidth();
  ASTNode * n = 
    new ASTNode(parserInterface->nf->CreateTerm(READ, width, array, index));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 717 "smt.y"
    {
  //ARRAY WRITE
  unsigned int width = (yyvsp[(4) - (4)].node)->GetValueWidth();
  ASTNode array = *(yyvsp[(2) - (4)].node);
  ASTNode index = *(yyvsp[(3) - (4)].node);
  ASTNode writeval = *(yyvsp[(4) - (4)].node);
  ASTNode write_term = parserInterface->nf->CreateArrayTerm(WRITE,(yyvsp[(2) - (4)].node)->GetIndexWidth(),width,array,index,writeval);
  ASTNode * n = new ASTNode(write_term);
  (yyval.node) = n;
  delete (yyvsp[(2) - (4)].node);
  delete (yyvsp[(3) - (4)].node);
  delete (yyvsp[(4) - (4)].node);
}
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 731 "smt.y"
    {
  int width = (yyvsp[(3) - (7)].uintval) - (yyvsp[(5) - (7)].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
      
  if((unsigned)(yyvsp[(3) - (7)].uintval) >= (yyvsp[(7) - (7)].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      
      
  ASTNode hi  =  parserInterface->CreateBVConst(32, (yyvsp[(3) - (7)].uintval));
  ASTNode low =  parserInterface->CreateBVConst(32, (yyvsp[(5) - (7)].uintval));
  ASTNode output = parserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[(7) - (7)].node),hi,low);
  ASTNode * n = new ASTNode(output);
  (yyval.node) = n;
  delete (yyvsp[(7) - (7)].node);
}
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 747 "smt.y"
    {
  const unsigned int width = (yyvsp[(3) - (4)].node)->GetValueWidth();
  (yyval.node) = new ASTNode(parserInterface->nf->CreateArrayTerm(ITE,(yyvsp[(4) - (4)].node)->GetIndexWidth(), width,*(yyvsp[(2) - (4)].node), *(yyvsp[(3) - (4)].node), *(yyvsp[(4) - (4)].node)));      
  delete (yyvsp[(2) - (4)].node);
  delete (yyvsp[(3) - (4)].node);
  delete (yyvsp[(4) - (4)].node);
}
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 755 "smt.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth() + (yyvsp[(3) - (3)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 763 "smt.y"
    {
  //this is the BVNEG (term) in the CVCL language
  unsigned int width = (yyvsp[(2) - (2)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(2) - (2)].node)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 771 "smt.y"
    {
  //this is the BVUMINUS term in CVCL langauge
  unsigned width = (yyvsp[(2) - (2)].node)->GetValueWidth();
  ASTNode * n =  new ASTNode(parserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[(2) - (2)].node)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 779 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 787 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)); 
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 795 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n =parserInterface->newNode(BVXOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 803 "smt.y"
    {
//   (bvxnor s t) abbreviates (bvor (bvand s t) (bvand (bvnot s) (bvnot t)))

	
      unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
      ASTNode * n = new ASTNode(
      parserInterface->nf->CreateTerm( BVOR, width,
     parserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)),
     parserInterface->nf->CreateTerm(BVAND, width,
	     parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(2) - (3)].node)),
     	 parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(3) - (3)].node))
     )));

      (yyval.node) = n;
      delete (yyvsp[(2) - (3)].node);
      delete (yyvsp[(3) - (3)].node);
  
  }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 822 "smt.y"
    {
	

  	ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(ITE, 1, 
  	parserInterface->nf->CreateNode(EQ, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)),
  	parserInterface->CreateOneConst(1),
  	parserInterface->CreateZeroConst(1)));
  	
      (yyval.node) = n;
      delete (yyvsp[(2) - (3)].node);
      delete (yyvsp[(3) - (3)].node);
  }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 837 "smt.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVSUB, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 845 "smt.y"
    {
  const unsigned int width = (*(yyvsp[(2) - (2)].vec))[0].GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVPLUS, width, *(yyvsp[(2) - (2)].vec)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (2)].vec);

}
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 853 "smt.y"
    {
  const unsigned int width = (*(yyvsp[(2) - (2)].vec))[0].GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVMULT, width, *(yyvsp[(2) - (2)].vec)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (2)].vec);
}
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 861 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVDIV, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 870 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVMOD, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 879 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVDIV, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 888 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVREM, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 896 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVMOD, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 904 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNEG, width, parserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node))));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 912 "smt.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNEG, width, parserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)))); 
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 920 "smt.y"
    {
  // shifting left by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVLEFTSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 929 "smt.y"
    {
  // shifting right by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVRIGHTSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 938 "smt.y"
    {
  // shifting arithmetic right by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVSRSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
  delete (yyvsp[(2) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 947 "smt.y"
    {
      
  ASTNode *n;
  unsigned width = (yyvsp[(5) - (5)].node)->GetValueWidth();
  unsigned rotate = (yyvsp[(3) - (5)].uintval);
  if (0 == rotate)
    {
      n = (yyvsp[(5) - (5)].node);
    }
  else if (rotate < width)
    {
      ASTNode high = parserInterface->CreateBVConst(32,width-1);
      ASTNode zero = parserInterface->CreateBVConst(32,0);
      ASTNode cut = parserInterface->CreateBVConst(32,width-rotate);
      ASTNode cutMinusOne = parserInterface->CreateBVConst(32,width-rotate-1);

      ASTNode top =  parserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[(5) - (5)].node),high, cut);
      ASTNode bottom =  parserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[(5) - (5)].node),cutMinusOne,zero);
      n =  new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
      delete (yyvsp[(5) - (5)].node);
    }
  else
    {
      n = NULL; // remove gcc warning.
      yyerror("Rotate must be strictly less than the width.");
    }
      
  (yyval.node) = n;
      
}
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 978 "smt.y"
    {
      
  ASTNode *n;
  unsigned width = (yyvsp[(5) - (5)].node)->GetValueWidth();
  unsigned rotate = (yyvsp[(3) - (5)].uintval);
  if (0 == rotate)
    {
      n = (yyvsp[(5) - (5)].node);
    }
  else if (rotate < width)
    {
      ASTNode high = parserInterface->CreateBVConst(32,width-1);
      ASTNode zero = parserInterface->CreateBVConst(32,0);
      ASTNode cut = parserInterface->CreateBVConst(32,rotate); 
      ASTNode cutMinusOne = parserInterface->CreateBVConst(32,rotate-1);

      ASTNode bottom =  parserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[(5) - (5)].node),cutMinusOne, zero);
      ASTNode top =  parserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[(5) - (5)].node),high,cut);
      n =  new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
      delete (yyvsp[(5) - (5)].node);
    }
  else
    {
      n = NULL; // remove gcc warning.
      yyerror("Rotate must be strictly less than the width.");
    }
      
  (yyval.node) = n;
      
}
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 1009 "smt.y"
    {
	  unsigned count = (yyvsp[(3) - (5)].uintval);
	  if (count < 1)
	  	FatalError("One or more repeats please");

	  unsigned w = (yyvsp[(5) - (5)].node)->GetValueWidth();  
      ASTNode n =  *(yyvsp[(5) - (5)].node);
      
      for (unsigned i =1; i < count; i++)
      {
      	  n = parserInterface->nf->CreateTerm(BVCONCAT,w*(i+1),n,*(yyvsp[(5) - (5)].node));
      }
       delete (yyvsp[(5) - (5)].node);
      (yyval.node) = new ASTNode(n);
    }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 1025 "smt.y"
    {
  unsigned w = (yyvsp[(5) - (5)].node)->GetValueWidth() + (yyvsp[(3) - (5)].uintval);
  ASTNode width = parserInterface->CreateBVConst(32,w);
  ASTNode *n =  new ASTNode(parserInterface->nf->CreateTerm(BVSX,w,*(yyvsp[(5) - (5)].node),width));
  (yyval.node) = n;
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 1033 "smt.y"
    {
  if (0 != (yyvsp[(3) - (5)].uintval))
    {
      unsigned w = (yyvsp[(5) - (5)].node)->GetValueWidth() + (yyvsp[(3) - (5)].uintval);
      ASTNode leading_zeroes = parserInterface->CreateZeroConst((yyvsp[(3) - (5)].uintval));
      ASTNode *n =  new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT,w,leading_zeroes,*(yyvsp[(5) - (5)].node)));
      (yyval.node) = n;
      delete (yyvsp[(5) - (5)].node);
    }
  else
    (yyval.node) = (yyvsp[(5) - (5)].node);

}
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 1050 "smt.y"
    {
  // Just return BV width.  If sort is BOOL, width is 0.
  // Otherwise, BITVEC[w] returns w. 
  //
  //((indexwidth is 0) && (valuewidth>0)) iff type is BV
  (yyval.indexvaluewidth).indexwidth = 0;
  unsigned int length = (yyvsp[(3) - (4)].uintval);
  if(length > 0) {
    (yyval.indexvaluewidth).valuewidth = length;
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
}
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 1065 "smt.y"
    {
  unsigned int index_len = (yyvsp[(3) - (6)].uintval);
  unsigned int value_len = (yyvsp[(5) - (6)].uintval);
  if(index_len > 0) {
    (yyval.indexvaluewidth).indexwidth = (yyvsp[(3) - (6)].uintval);
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  if(value_len > 0) {
    (yyval.indexvaluewidth).valuewidth = (yyvsp[(5) - (6)].uintval);
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
}
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 1086 "smt.y"
    {
  (yyval.node) = new ASTNode((*(yyvsp[(1) - (1)].node))); 
  delete (yyvsp[(1) - (1)].node);      
}
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 1091 "smt.y"
    {
  (yyval.node) = new ASTNode((*(yyvsp[(1) - (1)].node)));
  delete (yyvsp[(1) - (1)].node);
}
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 1096 "smt.y"
    {
  (yyval.node) = (yyvsp[(2) - (2)].node);
}
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 1103 "smt.y"
    {
  (yyval.node) = (yyvsp[(2) - (2)].node); 
}
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 1107 "smt.y"
    {
  (yyval.node) = new ASTNode((*(yyvsp[(1) - (1)].node))); 
  delete (yyvsp[(1) - (1)].node);      
}
    break;



/* Line 1806 of yacc.c  */
#line 3130 "parsesmt.cpp"
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
		      yytoken, &yylval);
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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


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

#if !defined(yyoverflow) || YYERROR_VERBOSE
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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



/* Line 2067 of yacc.c  */
#line 1112 "smt.y"


