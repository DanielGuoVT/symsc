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
#define yyparse         smt2parse
#define yylex           smt2lex
#define yyerror         smt2error
#define yylval          smt2lval
#define yychar          smt2char
#define yydebug         smt2debug
#define yynerrs         smt2nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "smt2.y"

  /********************************************************************
   * AUTHORS:  Trevor Hansen
   *
   * BEGIN DATE: May, 2010
   *
   * This file is modified version of the STP's smtlib.y file. Please
   * see CVCL license below
   ********************************************************************/

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
  
  extern char* smt2text;
  extern int smt2lineno;
  extern int smt2lex(void);

  int yyerror(const char *s) {
    cout << "syntax error: line " << smt2lineno << "\n" << s << endl;
    cout << "  token: " << smt2text << endl;
    FatalError("");
    return 1;
  }

  ASTNode querysmt2;
  vector<ASTVec> assertionsSMT2;
    
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 104857600
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1
#define YYPARSE_PARAM AssertsQuery
  

/* Line 268 of yacc.c  */
#line 157 "parsesmt2.cpp"

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
     END = 0,
     NUMERAL_TOK = 258,
     BVCONST_DECIMAL_TOK = 259,
     BVCONST_BINARY_TOK = 260,
     BVCONST_HEXIDECIMAL_TOK = 261,
     DECIMAL_TOK = 262,
     FORMID_TOK = 263,
     TERMID_TOK = 264,
     STRING_TOK = 265,
     SOURCE_TOK = 266,
     CATEGORY_TOK = 267,
     DIFFICULTY_TOK = 268,
     VERSION_TOK = 269,
     STATUS_TOK = 270,
     PRINT_TOK = 271,
     UNDERSCORE_TOK = 272,
     LPAREN_TOK = 273,
     RPAREN_TOK = 274,
     BVLEFTSHIFT_1_TOK = 275,
     BVRIGHTSHIFT_1_TOK = 276,
     BVARITHRIGHTSHIFT_TOK = 277,
     BVPLUS_TOK = 278,
     BVSUB_TOK = 279,
     BVNOT_TOK = 280,
     BVMULT_TOK = 281,
     BVDIV_TOK = 282,
     SBVDIV_TOK = 283,
     BVMOD_TOK = 284,
     SBVREM_TOK = 285,
     SBVMOD_TOK = 286,
     BVNEG_TOK = 287,
     BVAND_TOK = 288,
     BVOR_TOK = 289,
     BVXOR_TOK = 290,
     BVNAND_TOK = 291,
     BVNOR_TOK = 292,
     BVXNOR_TOK = 293,
     BVCONCAT_TOK = 294,
     BVLT_TOK = 295,
     BVGT_TOK = 296,
     BVLE_TOK = 297,
     BVGE_TOK = 298,
     BVSLT_TOK = 299,
     BVSGT_TOK = 300,
     BVSLE_TOK = 301,
     BVSGE_TOK = 302,
     BVSX_TOK = 303,
     BVEXTRACT_TOK = 304,
     BVZX_TOK = 305,
     BVROTATE_RIGHT_TOK = 306,
     BVROTATE_LEFT_TOK = 307,
     BVREPEAT_TOK = 308,
     BVCOMP_TOK = 309,
     BITVEC_TOK = 310,
     ARRAY_TOK = 311,
     BOOL_TOK = 312,
     TRUE_TOK = 313,
     FALSE_TOK = 314,
     NOT_TOK = 315,
     AND_TOK = 316,
     OR_TOK = 317,
     XOR_TOK = 318,
     ITE_TOK = 319,
     EQ_TOK = 320,
     IMPLIES_TOK = 321,
     DISTINCT_TOK = 322,
     LET_TOK = 323,
     EXIT_TOK = 324,
     CHECK_SAT_TOK = 325,
     LOGIC_TOK = 326,
     NOTES_TOK = 327,
     OPTION_TOK = 328,
     DECLARE_FUNCTION_TOK = 329,
     FORMULA_TOK = 330,
     PUSH_TOK = 331,
     POP_TOK = 332,
     SELECT_TOK = 333,
     STORE_TOK = 334
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 78 "smt2.y"
  
  unsigned uintval;                  /* for numerals in types. */
  //ASTNode,ASTVec
  BEEV::ASTNode *node;
  BEEV::ASTVec *vec;
  std::string *str;



/* Line 293 of yacc.c  */
#line 283 "parsesmt2.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 295 "parsesmt2.cpp"

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
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   563

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  80
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  13
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  268

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   334

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
      75,    76,    77,    78,    79
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    11,    15,    19,    24,    30,
      35,    41,    46,    51,    56,    61,    66,    68,    70,    72,
      74,    76,    79,    82,    85,    94,    99,   116,   118,   121,
     123,   125,   127,   133,   138,   143,   149,   155,   161,   167,
     173,   179,   185,   191,   195,   200,   206,   213,   218,   223,
     229,   235,   243,   246,   248,   253,   258,   260,   263,   265,
     269,   273,   278,   286,   293,   300,   305,   309,   312,   315,
     319,   323,   327,   331,   335,   339,   343,   347,   351,   355,
     359,   363,   367,   371,   375,   379,   383,   387,   394,   401,
     408,   412,   414
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      81,     0,    -1,    82,     0,    -1,    82,    83,    -1,    83,
      -1,    18,    69,    19,    -1,    18,    70,    19,    -1,    18,
      71,    10,    19,    -1,    18,    72,    85,    10,    19,    -1,
      18,    73,    85,    19,    -1,    18,    72,    85,     7,    19,
      -1,    18,    72,    85,    19,    -1,    18,    76,     3,    19,
      -1,    18,    77,     3,    19,    -1,    18,    74,    86,    19,
      -1,    18,    75,    88,    19,    -1,    10,    -1,    11,    -1,
      12,    -1,    13,    -1,    14,    -1,    15,    84,    -1,    16,
      58,    -1,    16,    59,    -1,    10,    18,    19,    18,    17,
      55,     3,    19,    -1,    10,    18,    19,    57,    -1,    10,
      18,    19,    18,    56,    18,    17,    55,     3,    19,    18,
      17,    55,     3,    19,    19,    -1,    88,    -1,    87,    88,
      -1,    58,    -1,    59,    -1,     8,    -1,    18,    65,    92,
      92,    19,    -1,    18,    67,    91,    19,    -1,    18,    67,
      87,    19,    -1,    18,    44,    92,    92,    19,    -1,    18,
      46,    92,    92,    19,    -1,    18,    45,    92,    92,    19,
      -1,    18,    47,    92,    92,    19,    -1,    18,    40,    92,
      92,    19,    -1,    18,    42,    92,    92,    19,    -1,    18,
      41,    92,    92,    19,    -1,    18,    43,    92,    92,    19,
      -1,    18,    88,    19,    -1,    18,    60,    88,    19,    -1,
      18,    66,    88,    88,    19,    -1,    18,    64,    88,    88,
      88,    19,    -1,    18,    61,    87,    19,    -1,    18,    62,
      87,    19,    -1,    18,    63,    88,    88,    19,    -1,    18,
      65,    88,    88,    19,    -1,    18,    68,    18,    89,    19,
      88,    19,    -1,    90,    89,    -1,    90,    -1,    18,    10,
      88,    19,    -1,    18,    10,    92,    19,    -1,    92,    -1,
      91,    92,    -1,     9,    -1,    18,    92,    19,    -1,    78,
      92,    92,    -1,    79,    92,    92,    92,    -1,    18,    17,
      49,     3,     3,    19,    92,    -1,    18,    17,    50,     3,
      19,    92,    -1,    18,    17,    48,     3,    19,    92,    -1,
      64,    88,    92,    92,    -1,    39,    92,    92,    -1,    25,
      92,    -1,    32,    92,    -1,    33,    92,    92,    -1,    34,
      92,    92,    -1,    35,    92,    92,    -1,    38,    92,    92,
      -1,    54,    92,    92,    -1,    24,    92,    92,    -1,    23,
      92,    92,    -1,    26,    92,    92,    -1,    27,    92,    92,
      -1,    29,    92,    92,    -1,    28,    92,    92,    -1,    30,
      92,    92,    -1,    31,    92,    92,    -1,    36,    92,    92,
      -1,    37,    92,    92,    -1,    20,    92,    92,    -1,    21,
      92,    92,    -1,    22,    92,    92,    -1,    18,    17,    52,
       3,    19,    92,    -1,    18,    17,    51,     3,    19,    92,
      -1,    18,    17,    53,     3,    19,    92,    -1,    17,     4,
       3,    -1,     6,    -1,     5,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   196,   196,   206,   207,   212,   219,   224,   234,   238,
     241,   243,   245,   254,   263,   267,   276,   294,   296,   298,
     300,   302,   304,   309,   317,   327,   335,   359,   368,   379,
     385,   391,   396,   403,   429,   453,   460,   467,   474,   481,
     488,   495,   502,   509,   513,   518,   524,   531,   536,   541,
     547,   553,   561,   562,   565,   579,   597,   607,   618,   623,
     627,   640,   654,   670,   683,   692,   700,   708,   716,   724,
     732,   740,   748,   764,   775,   783,   792,   800,   809,   818,
     827,   835,   843,   851,   859,   868,   877,   886,   912,   936,
     952,   958,   965
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "$undefined", "NUMERAL_TOK",
  "BVCONST_DECIMAL_TOK", "BVCONST_BINARY_TOK", "BVCONST_HEXIDECIMAL_TOK",
  "DECIMAL_TOK", "FORMID_TOK", "TERMID_TOK", "STRING_TOK", "SOURCE_TOK",
  "CATEGORY_TOK", "DIFFICULTY_TOK", "VERSION_TOK", "STATUS_TOK",
  "PRINT_TOK", "UNDERSCORE_TOK", "LPAREN_TOK", "RPAREN_TOK",
  "BVLEFTSHIFT_1_TOK", "BVRIGHTSHIFT_1_TOK", "BVARITHRIGHTSHIFT_TOK",
  "BVPLUS_TOK", "BVSUB_TOK", "BVNOT_TOK", "BVMULT_TOK", "BVDIV_TOK",
  "SBVDIV_TOK", "BVMOD_TOK", "SBVREM_TOK", "SBVMOD_TOK", "BVNEG_TOK",
  "BVAND_TOK", "BVOR_TOK", "BVXOR_TOK", "BVNAND_TOK", "BVNOR_TOK",
  "BVXNOR_TOK", "BVCONCAT_TOK", "BVLT_TOK", "BVGT_TOK", "BVLE_TOK",
  "BVGE_TOK", "BVSLT_TOK", "BVSGT_TOK", "BVSLE_TOK", "BVSGE_TOK",
  "BVSX_TOK", "BVEXTRACT_TOK", "BVZX_TOK", "BVROTATE_RIGHT_TOK",
  "BVROTATE_LEFT_TOK", "BVREPEAT_TOK", "BVCOMP_TOK", "BITVEC_TOK",
  "ARRAY_TOK", "BOOL_TOK", "TRUE_TOK", "FALSE_TOK", "NOT_TOK", "AND_TOK",
  "OR_TOK", "XOR_TOK", "ITE_TOK", "EQ_TOK", "IMPLIES_TOK", "DISTINCT_TOK",
  "LET_TOK", "EXIT_TOK", "CHECK_SAT_TOK", "LOGIC_TOK", "NOTES_TOK",
  "OPTION_TOK", "DECLARE_FUNCTION_TOK", "FORMULA_TOK", "PUSH_TOK",
  "POP_TOK", "SELECT_TOK", "STORE_TOK", "$accept", "cmd", "commands",
  "cmdi", "status", "attribute", "var_decl", "an_formulas", "an_formula",
  "lets", "let", "an_terms", "an_term", 0
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
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    80,    81,    82,    82,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    84,    85,    85,    85,
      85,    85,    85,    85,    86,    86,    86,    87,    87,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    89,    89,    90,    90,    91,    91,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     1,     3,     3,     4,     5,     4,
       5,     4,     4,     4,     4,     4,     1,     1,     1,     1,
       1,     2,     2,     2,     8,     4,    16,     1,     2,     1,
       1,     1,     5,     4,     4,     5,     5,     5,     5,     5,
       5,     5,     5,     3,     4,     5,     6,     4,     4,     5,
       5,     7,     2,     1,     4,     4,     1,     2,     1,     3,
       3,     4,     7,     6,     6,     4,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     6,     6,
       3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     4,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     2,     3,     5,     6,     0,
      17,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      31,     0,    29,    30,     0,     0,     0,     7,    16,    21,
      22,    23,     0,     0,    11,     9,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    12,    13,    10,
       8,     0,    92,    91,    58,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    56,     0,    43,     0,    25,     0,     0,     0,     0,
       0,     0,     0,     0,    67,     0,     0,     0,     0,     0,
       0,    68,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    44,    47,    28,    48,     0,     0,     0,     0,     0,
       0,    34,    33,    57,     0,     0,    53,     0,     0,    90,
       0,     0,     0,     0,     0,     0,    59,    84,    85,    86,
      75,    74,    76,    77,    79,    78,    80,    81,    69,    70,
      71,    82,    83,    72,    66,    73,     0,    60,     0,    39,
      41,    40,    42,    35,    37,    36,    38,    49,     0,     0,
      50,    32,    45,     0,     0,    52,     0,     0,     0,     0,
       0,     0,     0,     0,    65,    61,    46,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    54,    55,
      51,    24,     0,    64,     0,    63,    88,    87,    89,     0,
      62,     0,     0,     0,     0,     0,     0,    26
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,    39,    26,    29,   110,   111,   175,
     176,   120,   128
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -48
static const yytype_int16 yypact[] =
{
     -17,    50,    41,    58,   -48,    28,    43,    54,    70,    70,
      56,    36,    60,    64,   -48,   -48,   -48,   -48,   -48,    49,
     -48,   -48,   -48,   -48,    63,   -44,    -2,    51,    57,    55,
     -48,   495,   -48,   -48,    59,    69,    71,   -48,   -48,   -48,
     -48,   -48,    72,    73,   -48,   -48,    74,   -48,   392,   392,
     392,   392,   392,   392,   392,   392,    36,    36,    36,    36,
      36,     1,    36,     1,    80,    81,   -48,   -48,   -48,   -48,
     -48,   -15,   -48,   -48,   -48,   102,   455,   392,   392,   392,
     392,   392,   392,   392,   392,   392,   392,   392,   392,   392,
     392,   392,   392,   392,   392,   392,   392,   392,    36,   392,
     392,   392,   392,   392,   392,   392,   392,   392,   392,    90,
      -6,   -48,    38,    36,    36,   328,    36,   392,    36,    53,
     233,   -48,    92,   -48,   -13,   -48,    86,    65,   109,   392,
     392,   392,   392,   392,   -48,   392,   392,   392,   392,   392,
     392,   -48,   392,   392,   392,   392,   392,   392,   392,   392,
     392,   392,   392,   110,   111,   112,   113,   122,   123,   124,
     125,   -48,   -48,   -48,   -48,   126,    36,    36,   128,   130,
     131,   -48,   -48,   -48,    67,   132,    92,    97,   135,   -48,
     151,   154,   155,   156,   157,   158,   -48,   -48,   -48,   -48,
     -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,
     -48,   -48,   -48,   -48,   -48,   -48,   392,   -48,   392,   -48,
     -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,   164,     1,
     -48,   -48,   -48,     1,    36,   -48,   191,   178,   177,   194,
     179,   180,   181,   182,   -48,   -48,   -48,   184,   185,   187,
     188,   171,   392,   190,   392,   392,   392,   392,   -48,   -48,
     -48,   -48,   207,   -48,   392,   -48,   -48,   -48,   -48,   192,
     -48,   201,   223,   186,   240,   225,   226,   -48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -48,   -48,   -48,   243,   -48,   238,   -48,   -47,   -11,    98,
     -48,   -48,    85
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      34,     1,    30,   124,   177,    42,    72,    73,    43,    30,
      74,   112,    31,   162,    40,    41,   119,    44,    75,   115,
      65,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    14,   125,   178,    30,   109,    30,    17,   113,   114,
     116,   118,    32,    33,    31,    97,    31,   164,    15,    32,
      33,    30,    18,    35,    19,    98,    28,    36,    37,   126,
      45,    31,   171,    38,    47,    46,     1,   223,    66,    99,
     100,    20,    21,    22,    23,    24,    25,   150,    67,   179,
      68,    69,    70,    71,    32,    33,    32,    33,   122,   163,
     123,   163,   165,   166,    65,   168,   126,   170,   163,   161,
     174,    32,    33,   180,   181,   182,   183,   184,   185,     5,
       6,     7,     8,     9,    10,    11,    12,    13,   186,   209,
     210,   211,   212,   101,   102,   103,   104,   105,   106,   107,
     108,   213,   214,   215,   216,   217,   117,   220,   121,   221,
     222,   224,   226,   227,   228,   218,   219,   229,   230,   231,
     232,   233,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   236,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   240,   241,   242,   243,   244,   245,
     246,   247,   169,   248,   249,   173,   250,   251,   166,   254,
     259,   261,   237,   239,   187,   188,   189,   190,   191,   262,
     192,   193,   194,   195,   196,   197,   252,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,    72,    73,
     263,   264,    74,   265,   266,   267,    16,    27,     0,     0,
      75,    76,   172,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,     0,   225,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    97,     0,     0,
       0,   234,     0,   235,     0,     0,     0,    98,     0,     0,
       0,     0,     0,     0,   206,     0,     0,     0,   238,     0,
       0,    99,   100,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   253,     0,   255,
     256,   257,   258,    72,    73,     0,    30,    74,     0,   260,
       0,     0,     0,     0,     0,   127,   115,     0,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    48,    49,
      50,    51,    52,    53,    54,    55,     0,     0,     0,     0,
       0,     0,    97,     0,     0,     0,    32,    33,    56,    57,
      58,    59,   167,    61,    62,    63,    64,    72,    73,     0,
       0,    74,     0,     0,     0,     0,    99,   100,     0,    75,
      76,     0,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    97,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,     0,     0,     0,
      72,    73,     0,     0,    74,     0,     0,     0,     0,     0,
      99,   100,   127,    76,     0,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,     0,     0,     0,     0,    97,
       0,     0,     0,    31,     0,     0,     0,     0,     0,    98,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    99,   100,    48,    49,    50,    51,    52,
      53,    54,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    33,    56,    57,    58,    59,    60,
      61,    62,    63,    64
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-48))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      11,    18,     8,    18,    17,     7,     5,     6,    10,     8,
       9,    58,    18,    19,    58,    59,    63,    19,    17,    18,
      31,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,     0,    57,    56,     8,    56,     8,    19,    59,    60,
      61,    62,    58,    59,    18,    54,    18,    19,     0,    58,
      59,     8,    19,     3,    10,    64,    10,     3,    19,     4,
      19,    18,    19,    10,    19,    18,    18,    10,    19,    78,
      79,    11,    12,    13,    14,    15,    16,    98,    19,     3,
      19,    19,    19,    19,    58,    59,    58,    59,    18,   110,
      19,   112,   113,   114,   115,   116,     4,   118,   119,    19,
      18,    58,    59,    48,    49,    50,    51,    52,    53,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    19,    19,
      19,    19,    19,    48,    49,    50,    51,    52,    53,    54,
      55,    19,    19,    19,    19,    19,    61,    19,    63,    19,
      19,    19,    55,    18,     3,   166,   167,     3,     3,     3,
       3,     3,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    19,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,     3,    17,    19,     3,    19,    19,
      19,    19,   117,    19,    19,   120,    19,    19,   219,    19,
       3,    19,   223,   224,   129,   130,   131,   132,   133,    18,
     135,   136,   137,   138,   139,   140,    55,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,     5,     6,
      17,    55,     9,     3,    19,    19,     3,     9,    -1,    -1,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,   176,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      -1,   206,    -1,   208,    -1,    -1,    -1,    64,    -1,    -1,
      -1,    -1,    -1,    -1,   219,    -1,    -1,    -1,   223,    -1,
      -1,    78,    79,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   242,    -1,   244,
     245,   246,   247,     5,     6,    -1,     8,     9,    -1,   254,
      -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     5,     6,    -1,
      -1,     9,    -1,    -1,    -1,    -1,    78,    79,    -1,    17,
      18,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,
       5,     6,    -1,    -1,     9,    -1,    -1,    -1,    -1,    -1,
      78,    79,    17,    18,    -1,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     8,    -1,    -1,    -1,    -1,    -1,    54,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    40,    41,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    18,    81,    82,    83,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,    83,    19,    19,    10,
      11,    12,    13,    14,    15,    16,    85,    85,    10,    86,
       8,    18,    58,    59,    88,     3,     3,    19,    10,    84,
      58,    59,     7,    10,    19,    19,    18,    19,    40,    41,
      42,    43,    44,    45,    46,    47,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    88,    19,    19,    19,    19,
      19,    19,     5,     6,     9,    17,    18,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    54,    64,    78,
      79,    92,    92,    92,    92,    92,    92,    92,    92,    88,
      87,    88,    87,    88,    88,    18,    88,    92,    88,    87,
      91,    92,    18,    19,    18,    57,     4,    17,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      88,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    19,    19,    88,    19,    88,    88,    64,    88,    92,
      88,    19,    19,    92,    18,    89,    90,    17,    56,     3,
      48,    49,    50,    51,    52,    53,    19,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    88,    88,
      19,    19,    19,    10,    19,    89,    55,    18,     3,     3,
       3,     3,     3,     3,    92,    92,    19,    88,    92,    88,
       3,    17,    19,     3,    19,    19,    19,    19,    19,    19,
      19,    19,    55,    92,    19,    92,    92,    92,    92,     3,
      92,    19,    18,    17,    55,     3,    19,    19
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
#line 197 "smt2.y"
    {
       querysmt2 = ASTNode();
       assertionsSMT2.clear();
       parserInterface->cleanUp();
       YYACCEPT;
}
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 208 "smt2.y"
    {}
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 213 "smt2.y"
    {
	   querysmt2 = ASTNode();
       assertionsSMT2.clear();
       parserInterface->cleanUp();
       YYACCEPT;
	}
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 220 "smt2.y"
    {
		parserInterface->checkSat(assertionsSMT2);
	}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 225 "smt2.y"
    {
	  if (!(0 == strcmp((yyvsp[(3) - (4)].str)->c_str(),"QF_BV") ||
	        0 == strcmp((yyvsp[(3) - (4)].str)->c_str(),"QF_ABV") ||
	        0 == strcmp((yyvsp[(3) - (4)].str)->c_str(),"QF_AUFBV"))) {
	    yyerror("Wrong input logic:");
	  }
	  parserInterface->success();
	  delete (yyvsp[(3) - (4)].str);
	}
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 235 "smt2.y"
    {
	delete (yyvsp[(4) - (5)].str);
	}
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 239 "smt2.y"
    {
	}
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 242 "smt2.y"
    {}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 244 "smt2.y"
    {}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 246 "smt2.y"
    {
		for (int i=0; i < (yyvsp[(3) - (4)].uintval);i++)
		{
			parserInterface->push();
			assertionsSMT2.push_back(ASTVec());
		}
		parserInterface->success();
	}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 255 "smt2.y"
    {
		for (int i=0; i < (yyvsp[(3) - (4)].uintval);i++)
		{
			parserInterface->pop();
			assertionsSMT2.erase(assertionsSMT2.end()-1);
		}
		parserInterface->success();
	}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 264 "smt2.y"
    {
    parserInterface->success();
    }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 268 "smt2.y"
    {
	assertionsSMT2.back().push_back(*(yyvsp[(3) - (4)].node));
	parserInterface->deleteNode((yyvsp[(3) - (4)].node));
	parserInterface->success();
	}
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 276 "smt2.y"
    { 
 
 std::transform((yyvsp[(1) - (1)].str)->begin(), (yyvsp[(1) - (1)].str)->end(), (yyvsp[(1) - (1)].str)->begin(), ::tolower);
  
  if (0 == strcmp((yyvsp[(1) - (1)].str)->c_str(), "sat"))
  	input_status = TO_BE_SATISFIABLE;
  else if (0 == strcmp((yyvsp[(1) - (1)].str)->c_str(), "unsat"))
    input_status = TO_BE_UNSATISFIABLE;
  else if (0 == strcmp((yyvsp[(1) - (1)].str)->c_str(), "unknown"))
  	input_status = TO_BE_UNKNOWN; 
  else 
  	yyerror((yyvsp[(1) - (1)].str)->c_str());
  delete (yyvsp[(1) - (1)].str);
  (yyval.node) = NULL; 
}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 295 "smt2.y"
    {}
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 297 "smt2.y"
    {}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 299 "smt2.y"
    {}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 301 "smt2.y"
    {}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 303 "smt2.y"
    {}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 305 "smt2.y"
    {
	parserInterface->setPrintSuccess(true);
	parserInterface->success();
}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 310 "smt2.y"
    {
	parserInterface->setPrintSuccess(false);
}
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 318 "smt2.y"
    {
  ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol((yyvsp[(1) - (8)].str)->c_str()); 
  parserInterface->addSymbol(s);
  //Sort_symbs has the indexwidth/valuewidth. Set those fields in
  //var
  s.SetIndexWidth(0);
  s.SetValueWidth((yyvsp[(7) - (8)].uintval));
  delete (yyvsp[(1) - (8)].str);
}
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 328 "smt2.y"
    {
  ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol((yyvsp[(1) - (4)].str)->c_str());
  s.SetIndexWidth(0);
  s.SetValueWidth(0);
  parserInterface->addSymbol(s);
  delete (yyvsp[(1) - (4)].str);
}
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 336 "smt2.y"
    {
  ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol((yyvsp[(1) - (16)].str)->c_str());
  parserInterface->addSymbol(s);
  unsigned int index_len = (yyvsp[(9) - (16)].uintval);
  unsigned int value_len = (yyvsp[(14) - (16)].uintval);
  if(index_len > 0) {
    s.SetIndexWidth((yyvsp[(9) - (16)].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }

  if(value_len > 0) {
    s.SetValueWidth((yyvsp[(14) - (16)].uintval));
  }
  else {
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
  }
  delete (yyvsp[(1) - (16)].str);
}
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 360 "smt2.y"
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[(1) - (1)].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
    parserInterface->deleteNode((yyvsp[(1) - (1)].node));
  }
}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 369 "smt2.y"
    {
  if ((yyvsp[(1) - (2)].vec) != NULL && (yyvsp[(2) - (2)].node) != NULL) {
    (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
    (yyval.vec) = (yyvsp[(1) - (2)].vec);
    parserInterface->deleteNode((yyvsp[(2) - (2)].node));
  }
}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 380 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->CreateNode(TRUE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 386 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->CreateNode(FALSE)); 
  assert(0 == (yyval.node)->GetIndexWidth()); 
  assert(0 == (yyval.node)->GetValueWidth());
}
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 392 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(*(yyvsp[(1) - (1)].node)); 
  parserInterface->deleteNode((yyvsp[(1) - (1)].node));      
}
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 397 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(EQ,*(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode((yyvsp[(3) - (5)].node));
  parserInterface->deleteNode((yyvsp[(4) - (5)].node));      
}
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 404 "smt2.y"
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
    parserInterface->newNode(forms[0]) :
    parserInterface->newNode(parserInterface->CreateNode(AND, forms));

  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 430 "smt2.y"
    {
  using namespace BEEV;

  ASTVec terms = *(yyvsp[(3) - (4)].vec);
  ASTVec forms;

  for(ASTVec::const_iterator it=terms.begin(),itend=terms.end();
      it!=itend; it++) {
    for(ASTVec::const_iterator it2=it+1; it2!=itend; it2++) {
      ASTNode n = (parserInterface->nf->CreateNode(NOT, parserInterface->CreateNode(IFF, *it, *it2)));
      forms.push_back(n); 
    }
  }

  if(forms.size() == 0) 
    FatalError("empty distinct");
 
  (yyval.node) = (forms.size() == 1) ?
    parserInterface->newNode(forms[0]) :
    parserInterface->newNode(parserInterface->CreateNode(AND, forms));

  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 454 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVSLT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode((yyvsp[(3) - (5)].node));
  parserInterface->deleteNode((yyvsp[(4) - (5)].node));      
}
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 461 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVSLE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 468 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVSGT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 475 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVSGE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 482 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVLT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 489 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVLE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 496 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVGT, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 503 "smt2.y"
    {
  ASTNode * n = parserInterface->newNode(BVGE, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 510 "smt2.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node);
}
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 514 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->nf->CreateNode(NOT, *(yyvsp[(3) - (4)].node)));
    parserInterface->deleteNode( (yyvsp[(3) - (4)].node));
}
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 519 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(IMPLIES, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));      
}
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 525 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->nf->CreateNode(ITE, *(yyvsp[(3) - (6)].node), *(yyvsp[(4) - (6)].node), *(yyvsp[(5) - (6)].node)));
  parserInterface->deleteNode( (yyvsp[(3) - (6)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (6)].node));      
  parserInterface->deleteNode( (yyvsp[(5) - (6)].node));
}
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 532 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->CreateNode(AND, *(yyvsp[(3) - (4)].vec)));
  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 537 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(parserInterface->CreateNode(OR, *(yyvsp[(3) - (4)].vec)));
  delete (yyvsp[(3) - (4)].vec);
}
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 542 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(XOR, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));
}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 548 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode(IFF, *(yyvsp[(3) - (5)].node), *(yyvsp[(4) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (5)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (5)].node));
}
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 554 "smt2.y"
    {
  (yyval.node) = (yyvsp[(6) - (7)].node);
  //Cleanup the LetIDToExprMap
  parserInterface->letMgr.CleanupLetIDMap();                      
}
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 563 "smt2.y"
    {}
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 566 "smt2.y"
    {
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  parserInterface->letMgr.LetExprMgr(*(yyvsp[(2) - (4)].str),*(yyvsp[(3) - (4)].node));
  delete (yyvsp[(2) - (4)].str);
  parserInterface->deleteNode( (yyvsp[(3) - (4)].node));
}
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 580 "smt2.y"
    {
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  parserInterface->letMgr.LetExprMgr(*(yyvsp[(2) - (4)].str),*(yyvsp[(3) - (4)].node));
  delete (yyvsp[(2) - (4)].str);
  parserInterface->deleteNode( (yyvsp[(3) - (4)].node));

}
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 598 "smt2.y"
    {
  (yyval.vec) = new ASTVec;
  if ((yyvsp[(1) - (1)].node) != NULL) {
    (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
    parserInterface->deleteNode( (yyvsp[(1) - (1)].node));
  
  }
}
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 608 "smt2.y"
    {
  if ((yyvsp[(1) - (2)].vec) != NULL && (yyvsp[(2) - (2)].node) != NULL) {
    (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
    (yyval.vec) = (yyvsp[(1) - (2)].vec);
    parserInterface->deleteNode( (yyvsp[(2) - (2)].node));
  }
}
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 619 "smt2.y"
    {
  (yyval.node) = parserInterface->newNode((*(yyvsp[(1) - (1)].node)));
  parserInterface->deleteNode( (yyvsp[(1) - (1)].node));
}
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 624 "smt2.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node);
}
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 628 "smt2.y"
    {
  //ARRAY READ
  // valuewidth is same as array, indexwidth is 0.
  ASTNode array = *(yyvsp[(2) - (3)].node);
  ASTNode index = *(yyvsp[(3) - (3)].node);
  unsigned int width = array.GetValueWidth();
  ASTNode * n = 
    parserInterface->newNode(parserInterface->nf->CreateTerm(READ, width, array, index));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 641 "smt2.y"
    {
  //ARRAY WRITE
  unsigned int width = (yyvsp[(4) - (4)].node)->GetValueWidth();
  ASTNode array = *(yyvsp[(2) - (4)].node);
  ASTNode index = *(yyvsp[(3) - (4)].node);
  ASTNode writeval = *(yyvsp[(4) - (4)].node);
  ASTNode write_term = parserInterface->nf->CreateArrayTerm(WRITE,(yyvsp[(2) - (4)].node)->GetIndexWidth(),width,array,index,writeval);
  ASTNode * n = parserInterface->newNode(write_term);
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(2) - (4)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (4)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (4)].node));
}
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 655 "smt2.y"
    {
  int width = (yyvsp[(4) - (7)].uintval) - (yyvsp[(5) - (7)].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
      
  if((unsigned)(yyvsp[(4) - (7)].uintval) >= (yyvsp[(7) - (7)].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      
      
  ASTNode hi  =  parserInterface->CreateBVConst(32, (yyvsp[(4) - (7)].uintval));
  ASTNode low =  parserInterface->CreateBVConst(32, (yyvsp[(5) - (7)].uintval));
  ASTNode output = parserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[(7) - (7)].node),hi,low);
  ASTNode * n = parserInterface->newNode(output);
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(7) - (7)].node));
}
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 671 "smt2.y"
    {
  if (0 != (yyvsp[(4) - (6)].uintval))
    {
      unsigned w = (yyvsp[(6) - (6)].node)->GetValueWidth() + (yyvsp[(4) - (6)].uintval);
      ASTNode leading_zeroes = parserInterface->CreateZeroConst((yyvsp[(4) - (6)].uintval));
      ASTNode *n =  parserInterface->newNode(parserInterface->nf->CreateTerm(BVCONCAT,w,leading_zeroes,*(yyvsp[(6) - (6)].node)));
      (yyval.node) = n;
      parserInterface->deleteNode( (yyvsp[(6) - (6)].node));
    }
  else
    (yyval.node) = (yyvsp[(6) - (6)].node);
}
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 684 "smt2.y"
    {
  unsigned w = (yyvsp[(6) - (6)].node)->GetValueWidth() + (yyvsp[(4) - (6)].uintval);
  ASTNode width = parserInterface->CreateBVConst(32,w);
  ASTNode *n =  parserInterface->newNode(parserInterface->nf->CreateTerm(BVSX,w,*(yyvsp[(6) - (6)].node),width));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(6) - (6)].node));
}
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 693 "smt2.y"
    {
  const unsigned int width = (yyvsp[(3) - (4)].node)->GetValueWidth();
  (yyval.node) = parserInterface->newNode(parserInterface->nf->CreateArrayTerm(ITE,(yyvsp[(4) - (4)].node)->GetIndexWidth(), width,*(yyvsp[(2) - (4)].node), *(yyvsp[(3) - (4)].node), *(yyvsp[(4) - (4)].node)));      
  parserInterface->deleteNode( (yyvsp[(2) - (4)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (4)].node));
  parserInterface->deleteNode( (yyvsp[(4) - (4)].node));
}
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 701 "smt2.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth() + (yyvsp[(3) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
  parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 709 "smt2.y"
    {
  //this is the BVNEG (term) in the CVCL language
  unsigned int width = (yyvsp[(2) - (2)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(2) - (2)].node)));
  (yyval.node) = n;
  parserInterface->deleteNode( (yyvsp[(2) - (2)].node));
}
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 717 "smt2.y"
    {
  //this is the BVUMINUS term in CVCL langauge
  unsigned width = (yyvsp[(2) - (2)].node)->GetValueWidth();
  ASTNode * n =  parserInterface->newNode(parserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[(2) - (2)].node)));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (2)].node));
}
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 725 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 733 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)); 
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 741 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVXOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 749 "smt2.y"
    {
//   (bvxnor s t) abbreviates (bvor (bvand s t) (bvand (bvnot s) (bvnot t)))
      unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
      ASTNode * n = parserInterface->newNode(
      parserInterface->nf->CreateTerm( BVOR, width,
     parserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)),
     parserInterface->nf->CreateTerm(BVAND, width,
	     parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(2) - (3)].node)),
     	 parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(3) - (3)].node))
     )));

      (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 765 "smt2.y"
    {
  	ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(ITE, 1, 
  	parserInterface->nf->CreateNode(EQ, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)),
  	parserInterface->CreateOneConst(1),
  	parserInterface->CreateZeroConst(1)));
  	
      (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 776 "smt2.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVSUB, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 784 "smt2.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVPLUS, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));

}
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 793 "smt2.y"
    {
  const unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(BVMULT, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 801 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVDIV, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 810 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVMOD, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 819 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVDIV, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;

    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 828 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVREM, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 836 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(SBVMOD, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 844 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(BVNEG, width, parserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node))));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 852 "smt2.y"
    {
  unsigned int width = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(parserInterface->nf->CreateTerm(BVNEG, width, parserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[(2) - (3)].node), *(yyvsp[(3) - (3)].node)))); 
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 860 "smt2.y"
    {
  // shifting left by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVLEFTSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 869 "smt2.y"
    {
  // shifting right by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVRIGHTSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 878 "smt2.y"
    {
  // shifting arithmetic right by who know how much?
  unsigned int w = (yyvsp[(2) - (3)].node)->GetValueWidth();
  ASTNode * n = parserInterface->newNode(BVSRSHIFT,w,*(yyvsp[(2) - (3)].node),*(yyvsp[(3) - (3)].node));
  (yyval.node) = n;
    parserInterface->deleteNode( (yyvsp[(2) - (3)].node));
    parserInterface->deleteNode( (yyvsp[(3) - (3)].node));
}
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 887 "smt2.y"
    {
  ASTNode *n;
  unsigned width = (yyvsp[(6) - (6)].node)->GetValueWidth();
  unsigned rotate = (yyvsp[(4) - (6)].uintval) % width;
  if (0 == rotate)
    {
      n = (yyvsp[(6) - (6)].node);
    }
  else 
    {
      ASTNode high = parserInterface->CreateBVConst(32,width-1);
      ASTNode zero = parserInterface->CreateBVConst(32,0);
      ASTNode cut = parserInterface->CreateBVConst(32,width-rotate);
      ASTNode cutMinusOne = parserInterface->CreateBVConst(32,width-rotate-1);

      ASTNode top =  parserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[(6) - (6)].node),high, cut);
      ASTNode bottom =  parserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[(6) - (6)].node),cutMinusOne,zero);
      n =  parserInterface->newNode(parserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
          parserInterface->deleteNode( (yyvsp[(6) - (6)].node));


    }
      
  (yyval.node) = n;
}
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 913 "smt2.y"
    {
  ASTNode *n;
  unsigned width = (yyvsp[(6) - (6)].node)->GetValueWidth();
  unsigned rotate = (yyvsp[(4) - (6)].uintval) % width;
  if (0 == rotate)
    {
      n = (yyvsp[(6) - (6)].node);
    }
  else 
    {
      ASTNode high = parserInterface->CreateBVConst(32,width-1);
      ASTNode zero = parserInterface->CreateBVConst(32,0);
      ASTNode cut = parserInterface->CreateBVConst(32,rotate); 
      ASTNode cutMinusOne = parserInterface->CreateBVConst(32,rotate-1);

      ASTNode bottom =  parserInterface->nf->CreateTerm(BVEXTRACT,rotate,*(yyvsp[(6) - (6)].node),cutMinusOne, zero);
      ASTNode top =  parserInterface->nf->CreateTerm(BVEXTRACT,width-rotate,*(yyvsp[(6) - (6)].node),high,cut);
      n =  parserInterface->newNode(parserInterface->nf->CreateTerm(BVCONCAT,width,bottom,top));
      parserInterface->deleteNode( (yyvsp[(6) - (6)].node));
    }
      
  (yyval.node) = n;
}
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 937 "smt2.y"
    {
	  unsigned count = (yyvsp[(4) - (6)].uintval);
	  if (count < 1)
	  	FatalError("One or more repeats please");

	  unsigned w = (yyvsp[(6) - (6)].node)->GetValueWidth();  
      ASTNode n =  *(yyvsp[(6) - (6)].node);
      
      for (unsigned i =1; i < count; i++)
      {
      	  n = parserInterface->nf->CreateTerm(BVCONCAT,w*(i+1),n,*(yyvsp[(6) - (6)].node));
      }
      (yyval.node) = parserInterface->newNode(n);
      parserInterface->deleteNode( (yyvsp[(6) - (6)].node));
}
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 953 "smt2.y"
    {
	(yyval.node) = parserInterface->newNode(parserInterface->CreateBVConst(*(yyvsp[(2) - (3)].str), 10, (yyvsp[(3) - (3)].uintval)));
    (yyval.node)->SetValueWidth((yyvsp[(3) - (3)].uintval));
    delete (yyvsp[(2) - (3)].str);
}
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 959 "smt2.y"
    {
	unsigned width = (yyvsp[(1) - (1)].str)->length()*4;
	(yyval.node) = parserInterface->newNode(parserInterface->CreateBVConst(*(yyvsp[(1) - (1)].str), 16, width));
    (yyval.node)->SetValueWidth(width);
    delete (yyvsp[(1) - (1)].str);
}
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 966 "smt2.y"
    {
	unsigned width = (yyvsp[(1) - (1)].str)->length();
	(yyval.node) = parserInterface->newNode(parserInterface->CreateBVConst(*(yyvsp[(1) - (1)].str), 2, width));
    (yyval.node)->SetValueWidth(width);
    delete (yyvsp[(1) - (1)].str);
}
    break;



/* Line 1806 of yacc.c  */
#line 2991 "parsesmt2.cpp"
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
#line 974 "smt2.y"


