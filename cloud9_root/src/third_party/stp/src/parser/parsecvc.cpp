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
#define yyparse         cvcparse
#define yylex           cvclex
#define yyerror         cvcerror
#define yylval          cvclval
#define yychar          cvcchar
#define yydebug         cvcdebug
#define yynerrs         cvcnerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 1 "cvc.y"

  // -*- c++ -*-
  /********************************************************************
   * AUTHORS: Vijay Ganesh
   *
   * BEGIN DATE: November, 2005
   *
   * LICENSE: Please view LICENSE file in the home dir of this Program
   ********************************************************************/
  
#include "parser.h"
#include "ParserInterface.h"

  using namespace std; 
  using namespace BEEV;
  
  // Suppress the bogus warning suppression in bison (it generates
  // compile error)
#undef __GNUC_MINOR__
  
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 1048576000
#define YYERROR_VERBOSE 1
#define YY_EXIT_FAILURE -1
#define YYPARSE_PARAM AssertsQuery
  
  extern int cvclex(void);
  extern char* yytext;
  extern int cvclineno;
  int yyerror(const char *s) {
    cout << "syntax error: line " << cvclineno << "\n" << s << endl;    
    FatalError("");
    return YY_EXIT_FAILURE;
  };
  
  

/* Line 268 of yacc.c  */
#line 117 "parsecvc.cpp"

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
     AND_TOK = 258,
     OR_TOK = 259,
     NOT_TOK = 260,
     EXCEPT_TOK = 261,
     XOR_TOK = 262,
     NAND_TOK = 263,
     NOR_TOK = 264,
     IMPLIES_TOK = 265,
     IFF_TOK = 266,
     IF_TOK = 267,
     THEN_TOK = 268,
     ELSE_TOK = 269,
     ELSIF_TOK = 270,
     END_TOK = 271,
     ENDIF_TOK = 272,
     NEQ_TOK = 273,
     ASSIGN_TOK = 274,
     BV_TOK = 275,
     BVLEFTSHIFT_TOK = 276,
     BVRIGHTSHIFT_TOK = 277,
     BVPLUS_TOK = 278,
     BVSUB_TOK = 279,
     BVUMINUS_TOK = 280,
     BVMULT_TOK = 281,
     BVDIV_TOK = 282,
     BVMOD_TOK = 283,
     SBVDIV_TOK = 284,
     SBVREM_TOK = 285,
     BVNEG_TOK = 286,
     BVAND_TOK = 287,
     BVOR_TOK = 288,
     BVXOR_TOK = 289,
     BVNAND_TOK = 290,
     BVNOR_TOK = 291,
     BVXNOR_TOK = 292,
     BVCONCAT_TOK = 293,
     BVLT_TOK = 294,
     BVGT_TOK = 295,
     BVLE_TOK = 296,
     BVGE_TOK = 297,
     BVSLT_TOK = 298,
     BVSGT_TOK = 299,
     BVSLE_TOK = 300,
     BVSGE_TOK = 301,
     BOOL_TO_BV_TOK = 302,
     BVSX_TOK = 303,
     BOOLEXTRACT_TOK = 304,
     ASSERT_TOK = 305,
     QUERY_TOK = 306,
     BOOLEAN_TOK = 307,
     ARRAY_TOK = 308,
     OF_TOK = 309,
     WITH_TOK = 310,
     TRUELIT_TOK = 311,
     FALSELIT_TOK = 312,
     IN_TOK = 313,
     LET_TOK = 314,
     PUSH_TOK = 315,
     POP_TOK = 316,
     BVCONST_TOK = 317,
     TERMID_TOK = 318,
     FORMID_TOK = 319,
     COUNTEREXAMPLE_TOK = 320,
     NUMERAL_TOK = 321,
     BIN_BASED_NUMBER = 322,
     DEC_BASED_NUMBER = 323,
     HEX_BASED_NUMBER = 324,
     STRING_TOK = 325
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 38 "cvc.y"


  unsigned int uintval;                 /* for numerals in types. */
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
  vector<char*> * stringVec;
  char* str;

  //Hash_Map to hold Array Updates during parse A map from array index
  //to array values. To support the WITH construct
  BEEV::ASTNodeMap * Index_To_UpdateValue;



/* Line 293 of yacc.c  */
#line 248 "parsecvc.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 260 "parsecvc.cpp"

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
#define YYFINAL  56
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1190

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNRULES -- Number of states.  */
#define YYNSTATES  322

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   325

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      66,    79,     2,     2,    78,     2,    65,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    77,    76,
       2,    62,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    80,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    67,    68,    69,
      70,    71,    72,    73,    74,    75
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    12,    14,    17,    20,
      23,    25,    29,    31,    34,    38,    42,    45,    49,    53,
      59,    65,    67,    71,    73,    75,    77,    79,    84,    86,
      91,    97,   101,   107,   111,   113,   118,   125,   129,   133,
     136,   140,   144,   148,   152,   156,   160,   164,   171,   178,
     185,   192,   199,   206,   213,   220,   222,   224,   226,   231,
     237,   241,   247,   249,   251,   255,   257,   261,   263,   268,
     271,   274,   277,   282,   287,   294,   297,   301,   305,   312,
     319,   326,   333,   340,   344,   348,   352,   356,   363,   372,
     377,   386,   395,   404,   413,   422,   424,   426,   431,   433,
     437,   443,   451,   453,   457,   461,   467,   471
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      82,     0,    -1,    84,    -1,    84,    83,    -1,    70,    76,
      -1,    -1,    88,    -1,    89,    88,    -1,    85,    88,    -1,
      89,    86,    -1,    86,    -1,    85,    89,    86,    -1,    87,
      -1,    86,    87,    -1,    50,    99,    76,    -1,    51,    99,
      76,    -1,    90,    76,    -1,    89,    90,    76,    -1,    92,
      77,    93,    -1,    92,    77,    93,    62,   103,    -1,    92,
      77,    93,    62,    99,    -1,    75,    -1,    75,    78,    91,
      -1,    91,    -1,    94,    -1,    95,    -1,    96,    -1,    20,
      66,    71,    79,    -1,    52,    -1,    53,    94,    54,    94,
      -1,    12,    99,    13,   103,    98,    -1,    14,   103,    17,
      -1,    15,   103,    13,   103,    98,    -1,    66,    99,    79,
      -1,    69,    -1,    69,    66,   103,    79,    -1,    49,    66,
     103,    78,    71,    79,    -1,   103,    62,   103,    -1,   103,
      18,   103,    -1,     5,    99,    -1,    99,     4,    99,    -1,
      99,     9,    99,    -1,    99,     3,    99,    -1,    99,     8,
      99,    -1,    99,    10,    99,    -1,    99,    11,    99,    -1,
      99,     7,    99,    -1,    39,    66,   103,    78,   103,    79,
      -1,    40,    66,   103,    78,   103,    79,    -1,    41,    66,
     103,    78,   103,    79,    -1,    42,    66,   103,    78,   103,
      79,    -1,    43,    66,   103,    78,   103,    79,    -1,    44,
      66,   103,    78,   103,    79,    -1,    45,    66,   103,    78,
     103,    79,    -1,    46,    66,   103,    78,   103,    79,    -1,
     100,    -1,    56,    -1,    57,    -1,    59,   106,    58,    99,
      -1,    12,    99,    13,    99,   101,    -1,    14,    99,    17,
      -1,    15,    99,    13,    99,   101,    -1,    75,    -1,   103,
      -1,   102,    78,   103,    -1,    68,    -1,    66,   103,    79,
      -1,    67,    -1,    47,    66,    99,    79,    -1,    71,    72,
      -1,    71,    73,    -1,    71,    74,    -1,   103,    63,   103,
      80,    -1,   103,    66,   103,    79,    -1,   103,    63,    71,
      77,    71,    80,    -1,    31,   103,    -1,   103,    32,   103,
      -1,   103,    33,   103,    -1,    34,    66,   103,    78,   103,
      79,    -1,    35,    66,   103,    78,   103,    79,    -1,    36,
      66,   103,    78,   103,    79,    -1,    37,    66,   103,    78,
     103,    79,    -1,    48,    66,   103,    78,    71,    79,    -1,
     103,    38,   103,    -1,   103,    21,    71,    -1,   103,    22,
      71,    -1,   103,    22,   103,    -1,    23,    66,    71,    78,
     102,    79,    -1,    24,    66,    71,    78,   103,    78,   103,
      79,    -1,    25,    66,   103,    79,    -1,    26,    66,    71,
      78,   103,    78,   103,    79,    -1,    27,    66,    71,    78,
     103,    78,   103,    79,    -1,    28,    66,    71,    78,   103,
      78,   103,    79,    -1,    29,    66,    71,    78,   103,    78,
     103,    79,    -1,    30,    66,    71,    78,   103,    78,   103,
      79,    -1,    97,    -1,   104,    -1,    59,   106,    58,   103,
      -1,    75,    -1,   103,    55,   105,    -1,    63,   103,    80,
      19,   103,    -1,   105,    55,    63,   103,    80,    19,   103,
      -1,   107,    -1,   106,    78,   107,    -1,    75,    62,   103,
      -1,    75,    77,    93,    62,   103,    -1,    75,    62,    99,
      -1,    75,    77,    93,    62,    99,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   179,   179,   183,   189,   196,   211,   217,   223,   241,
     245,   249,   269,   276,   285,   291,   296,   299,   304,   315,
     330,   347,   353,   361,   377,   378,   379,   382,   394,   400,
     408,   426,   427,   446,   450,   454,   460,   475,   482,   489,
     494,   500,   506,   512,   518,   524,   530,   536,   543,   550,
     557,   564,   571,   578,   585,   592,   593,   599,   606,   615,
     624,   625,   631,   639,   646,   656,   657,   658,   659,   669,
     675,   681,   687,   697,   707,   722,   729,   740,   751,   762,
     774,   786,   798,   815,   824,   840,   860,   876,   883,   891,
     898,   906,   914,   922,   930,   937,   938,   939,   942,   950,
     980,   987,   996,   997,  1000,  1019,  1033,  1043
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"AND\"", "\"OR\"", "\"NOT\"",
  "\"EXCEPT\"", "\"XOR\"", "\"NAND\"", "\"NOR\"", "\"=>\"", "\"<=>\"",
  "\"IF\"", "\"THEN\"", "\"ELSE\"", "\"ELSIF\"", "\"END\"", "\"ENDIF\"",
  "\"/=\"", "\":=\"", "\"BV\"", "\"<<\"", "\">>\"", "\"BVPLUS\"",
  "\"BVSUB\"", "\"BVUMINUS\"", "\"BVMULT\"", "\"BVDIV\"", "\"BVMOD\"",
  "\"SBVDIV\"", "\"SBVREM\"", "\"~\"", "\"&\"", "\"|\"", "\"BVXOR\"",
  "\"BVNAND\"", "\"BVNOR\"", "\"BVXNOR\"", "\"@\"", "\"BVLT\"", "\"BVGT\"",
  "\"BVLE\"", "\"BVGE\"", "\"BVSLT\"", "\"BVSGT\"", "\"BVSLE\"",
  "\"BVSGE\"", "\"BOOLBV\"", "\"BVSX\"", "\"BOOLEXTRACT\"", "\"ASSERT\"",
  "\"QUERY\"", "\"BOOLEAN\"", "\"ARRAY\"", "\"OF\"", "\"WITH\"",
  "\"TRUE\"", "\"FALSE\"", "\"IN\"", "\"LET\"", "\"PUSH\"", "\"POP\"",
  "'='", "'['", "'{'", "'.'", "'('", "BVCONST_TOK", "TERMID_TOK",
  "FORMID_TOK", "COUNTEREXAMPLE_TOK", "NUMERAL_TOK", "BIN_BASED_NUMBER",
  "DEC_BASED_NUMBER", "HEX_BASED_NUMBER", "STRING_TOK", "';'", "':'",
  "','", "')'", "']'", "$accept", "cmd", "counterexample", "other_cmd",
  "other_cmd1", "Asserts", "Assert", "Query", "VarDecls", "VarDecl",
  "reverseFORM_IDs", "FORM_IDs", "Type", "BvType", "BoolType", "ArrayType",
  "IfExpr", "ElseRestExpr", "Formula", "IfForm", "ElseRestForm", "Exprs",
  "Expr", "ArrayUpdateExpr", "Updates", "LetDecls", "LetDecl", 0
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
     315,   316,    61,    91,   123,    46,    40,   317,   318,   319,
     320,   321,   322,   323,   324,   325,    59,    58,    44,    41,
      93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    82,    83,    84,    84,    84,    84,    85,
      85,    85,    86,    86,    87,    88,    89,    89,    90,    90,
      90,    91,    91,    92,    93,    93,    93,    94,    95,    96,
      97,    98,    98,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,   100,
     101,   101,   101,   102,   102,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   104,
     105,   105,   106,   106,   107,   107,   107,   107
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     0,     1,     2,     2,     2,
       1,     3,     1,     2,     3,     3,     2,     3,     3,     5,
       5,     1,     3,     1,     1,     1,     1,     4,     1,     4,
       5,     3,     5,     3,     1,     4,     6,     3,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     6,     6,     6,
       6,     6,     6,     6,     6,     1,     1,     1,     4,     5,
       3,     5,     1,     1,     3,     1,     3,     1,     4,     2,
       2,     2,     4,     4,     6,     2,     3,     3,     6,     6,
       6,     6,     6,     3,     3,     3,     3,     6,     8,     4,
       8,     8,     8,     8,     8,     1,     1,     4,     1,     3,
       5,     7,     1,     3,     3,     5,     3,     5
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,    21,     0,     2,     0,    10,    12,     6,
       0,     0,    23,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      56,    57,     0,     0,    67,    65,    34,     0,    98,    95,
       0,    55,     0,    96,     0,     0,     1,     0,     3,     8,
       0,    13,     9,     7,     0,    16,     0,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   102,     0,
       0,     0,    69,    70,    71,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,    22,     4,    11,    17,     0,    28,
       0,    18,    24,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    66,     0,    42,
      40,    46,    43,    41,    44,    45,    38,    84,    85,    86,
      76,    77,    83,     0,    99,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    89,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    68,     0,     0,   106,
     104,     0,    58,    97,   103,    35,     0,     0,    72,    73,
       0,     0,    20,    19,     0,     0,    62,    59,     0,     0,
      30,     0,    63,     0,     0,     0,     0,     0,     0,     0,
      97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,    29,
       0,     0,     0,     0,     0,    87,     0,     0,     0,     0,
       0,     0,    78,    79,    80,    81,    47,    48,    49,    50,
      51,    52,    53,    54,    82,    36,   107,   105,     0,    74,
      60,     0,    31,     0,    64,     0,     0,     0,     0,     0,
       0,   100,     0,     0,    88,    90,    91,    92,    93,    94,
      61,    32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,    58,     5,     6,     7,     8,     9,    10,    11,
      12,    13,   131,   132,   133,   134,    49,   240,    50,    51,
     237,   241,    52,    53,   184,    97,    98
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -122
static const yytype_int16 yypact[] =
{
      -1,   402,   402,   -12,    68,     0,   -14,    28,  -122,  -122,
      -1,    18,  -122,    22,   402,   402,    34,    36,    50,    51,
      52,    55,    56,    66,   100,    76,    77,    78,    80,    90,
      92,    95,    97,    99,   103,   104,   107,   110,   111,   112,
    -122,  -122,   106,   402,  -122,  -122,   113,    41,  -122,  -122,
      25,  -122,  1124,  -122,    35,   109,  -122,   114,  -122,  -122,
     -44,  -122,    28,  -122,   115,  -122,    10,  -122,   216,   122,
     123,   100,   125,   130,   143,   146,   147,   402,   106,   100,
     132,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   402,   100,   100,     2,   -56,  -122,     7,
     434,   100,  -122,  -122,  -122,   402,   402,   402,   402,   402,
     402,   402,  -122,   100,   150,   223,   100,   100,   100,   165,
     100,   345,   100,  -122,  -122,  -122,    28,  -122,   120,  -122,
     168,   169,  -122,  -122,  -122,   402,   152,   162,   443,   166,
     167,   177,   186,   187,   334,   -18,   461,   748,   766,   784,
     786,   813,   833,   837,   851,   872,   886,   899,   923,    16,
     925,   949,   402,    10,   402,   106,  -122,  -122,   470,    79,
      64,   476,   265,  -122,   346,   346,   240,  -122,    41,    30,
     132,   245,   247,   100,  -122,   240,   -69,   117,   496,   204,
     222,   402,    44,  1088,   100,   100,  -122,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,  -122,   210,   213,   380,
    1124,   224,   380,  1124,  -122,  -122,   119,   217,  -122,  -122,
     208,   168,   380,  1124,   402,   402,  -122,  -122,   100,   100,
    -122,    19,   240,   962,   975,   988,  1001,  1014,  1027,  1097,
     240,   510,   525,   539,   546,   560,   575,   589,   610,   624,
     628,   642,   660,   214,   218,   402,   273,   221,  -122,  -122,
      73,  1164,  1115,  1062,   100,  -122,   100,   100,   100,   100,
     100,   100,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,
    -122,  -122,  -122,  -122,  -122,  -122,   380,  1124,   100,  -122,
    -122,   402,  -122,   100,   240,   678,   680,   698,   716,   730,
     734,    26,    44,  1097,  -122,  -122,  -122,  -122,  -122,  -122,
    -122,  -122
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -122,  -122,  -122,  -122,  -122,    -3,    -6,    59,   290,    11,
     250,  -122,   144,  -121,  -122,  -122,  -122,    -4,    -2,  -122,
       5,  -122,   121,  -122,  -122,   236,   174
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      54,    61,   164,   102,   103,   104,     1,    62,   227,   190,
     105,   106,    67,    68,   107,   108,   109,   110,   111,   105,
     106,    64,   165,   107,   108,   109,   110,   111,   105,   106,
     128,     3,   107,   108,   109,   110,   111,     2,   105,   106,
     203,    99,   107,   108,   109,   110,   111,   105,   106,     1,
       2,   107,   108,   109,   110,   111,    61,   126,   234,   235,
     165,     3,   129,   130,   162,    59,    55,   105,    56,    63,
      57,    64,   108,   109,     3,   144,   105,   106,     1,   163,
     107,   108,   109,   110,   111,   119,   166,   108,   109,   121,
     300,   159,   122,   121,    65,   216,   122,   274,   275,    66,
      69,   112,    70,   169,   170,   171,   172,   173,   174,   175,
     269,   123,    77,   102,   103,   104,    71,    72,    73,   236,
      61,    74,    75,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    76,   192,    25,    26,    27,    28,   114,   115,
     114,   115,    81,    82,    83,    80,    84,    37,    38,   116,
     117,   116,   117,   114,   115,   118,    85,   118,    86,    78,
     219,    87,   222,    88,   100,    89,    79,    44,    45,    90,
      91,    47,   119,    92,   119,    48,    93,    94,    95,   101,
     121,    96,   121,   122,     3,   122,   189,   119,   128,   232,
     125,   127,   138,   136,   137,   121,   139,   228,   122,   266,
     146,   140,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   141,   160,   161,   142,   143,   105,
     106,   177,   168,   107,   108,   109,   110,   111,   183,   135,
     194,   191,   270,   271,   176,    77,   179,   180,   181,   182,
     195,   185,   187,   188,   197,   198,    16,    17,    18,    19,
      20,    21,    22,    23,    24,   199,   193,    25,    26,    27,
      28,   114,   115,   296,   200,   201,   114,   115,   114,   115,
      37,    38,   116,   117,   109,   230,   231,   116,   118,   116,
     117,   263,    78,   220,   264,   223,   265,   268,   267,    79,
      44,    45,   298,   294,   178,   119,    60,   295,    48,   312,
     119,   299,   119,   121,   226,   124,   122,   221,   121,   321,
     121,   122,   233,   122,   145,   242,   243,   320,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   105,   106,   224,
       0,   107,   108,   109,   110,   111,     0,   202,     0,   105,
     106,     0,     0,     0,   108,   109,   110,    77,     0,   272,
     273,     0,     0,     0,     0,     0,     0,     0,    16,    17,
      18,    19,    20,    21,    22,    23,    24,     0,     0,    25,
      26,    27,    28,   105,   106,     0,   297,   107,   108,   109,
     110,   111,    37,    38,     0,   304,     0,   305,   306,   307,
     308,   309,   310,     0,    78,     0,     0,    14,     0,     0,
       0,    79,    44,    45,    15,     0,   186,     0,     0,   311,
      48,     0,     0,     0,   313,    16,    17,    18,    19,    20,
      21,    22,    23,    24,     0,     0,    25,    26,    27,    28,
       0,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,   113,     0,     0,   114,   115,     0,    40,    41,
       0,    42,     0,     0,   114,   115,   116,   117,    43,    44,
      45,    46,   118,    47,     0,   116,   117,    48,     0,   105,
     106,   118,   114,   115,   108,   109,   110,   111,     0,   119,
       0,   114,   115,   116,   117,     0,   120,   121,   119,   118,
     122,     0,   116,   117,     0,     0,   121,     0,   118,   122,
       0,     0,     0,   167,     0,     0,   119,   114,   115,     0,
       0,     0,   196,     0,   121,   119,     0,   122,   116,   117,
       0,   114,   115,   121,   118,     0,   122,     0,     0,     0,
     167,     0,   116,   117,     0,     0,   114,   115,   118,   225,
       0,   119,     0,     0,     0,     0,     0,   116,   117,   121,
     114,   115,   122,   118,     0,   119,     0,   114,   115,     0,
       0,   116,   117,   121,     0,   229,   122,   118,   116,   117,
     119,   114,   115,     0,   118,     0,     0,     0,   121,   282,
       0,   122,   116,   117,   119,     0,   114,   115,   118,     0,
       0,   119,   121,     0,   283,   122,     0,   116,   117,   121,
     114,   115,   122,   118,     0,   119,     0,     0,   284,     0,
       0,   116,   117,   121,     0,   285,   122,   118,     0,     0,
     119,   114,   115,     0,     0,     0,     0,     0,   121,   286,
       0,   122,   116,   117,   119,   114,   115,     0,   118,   114,
     115,     0,   121,     0,   287,   122,   116,   117,     0,     0,
     116,   117,   118,   114,   115,   119,   118,     0,   288,     0,
       0,     0,     0,   121,   116,   117,   122,     0,     0,   119,
     118,   114,   115,   119,     0,     0,     0,   121,     0,   289,
     122,   121,   116,   117,   122,     0,     0,   119,   118,   114,
     115,   114,   115,   290,     0,   121,     0,   291,   122,     0,
     116,   117,   116,   117,     0,   119,   118,     0,   118,   114,
     115,   292,     0,   121,     0,     0,   122,     0,     0,     0,
     116,   117,     0,   119,     0,   119,   118,   114,   115,   293,
       0,   121,     0,   121,   122,     0,   122,     0,   116,   117,
       0,   114,   115,   119,   118,   114,   115,   314,     0,   315,
       0,   121,   116,   117,   122,     0,   116,   117,   118,   114,
     115,   119,   118,     0,     0,     0,     0,   316,     0,   121,
     116,   117,   122,     0,     0,   119,   118,   114,   115,   119,
       0,     0,     0,   121,     0,   317,   122,   121,   116,   117,
     122,     0,     0,   119,   118,   114,   115,   114,   115,   318,
       0,   121,     0,   319,   122,     0,   116,   117,   116,   117,
       0,   119,   118,     0,   118,     0,   204,     0,     0,   121,
       0,     0,   122,     0,   114,   115,     0,     0,     0,   119,
       0,   119,     0,     0,   205,   116,   117,   121,     0,   121,
     122,   118,   122,     0,   114,   115,     0,     0,   114,   115,
       0,     0,   206,     0,   207,   116,   117,     0,   119,   116,
     117,   118,   114,   115,     0,   118,   121,     0,     0,   122,
       0,     0,     0,   116,   117,     0,     0,     0,   119,   118,
       0,   208,   119,   114,   115,     0,   121,     0,     0,   122,
     121,     0,     0,   122,   116,   117,   119,   114,   115,     0,
     118,   209,     0,     0,   121,   210,     0,   122,   116,   117,
     114,   115,     0,     0,   118,     0,     0,   119,     0,   211,
       0,   116,   117,     0,     0,   121,     0,   118,   122,     0,
       0,   119,     0,     0,   114,   115,   114,   115,     0,   121,
     212,     0,   122,     0,   119,   116,   117,   116,   117,     0,
       0,   118,   121,   118,   213,   122,     0,     0,     0,     0,
     114,   115,     0,     0,     0,     0,     0,   214,   119,     0,
     119,   116,   117,   114,   115,     0,   121,   118,   121,   122,
       0,   122,     0,     0,   116,   117,   114,   115,     0,     0,
     118,   215,     0,   217,   119,     0,     0,   116,   117,   114,
     115,     0,   121,   118,     0,   122,     0,   119,     0,     0,
     116,   117,   114,   115,     0,   121,   118,   218,   122,     0,
     119,     0,     0,   116,   117,   114,   115,     0,   121,   118,
     276,   122,     0,   119,     0,     0,   116,   117,   114,   115,
       0,   121,   118,   277,   122,     0,   119,     0,     0,   116,
     117,     0,     0,     0,   121,   118,   278,   122,     0,   119,
       0,     0,     0,     0,     0,   303,     0,   121,     0,   279,
     122,     0,   119,   114,   115,     0,     0,     0,     0,     0,
     121,     0,   280,   122,   116,   117,     0,     0,     0,     0,
     118,     0,   238,   239,     0,   281,   113,     0,     0,   114,
     115,   238,   239,     0,     0,     0,     0,   119,   114,   115,
     116,   117,     0,     0,     0,   121,   118,     0,   122,   116,
     117,     0,   302,     0,     0,   118,   114,   115,     0,     0,
       0,     0,   113,   119,     0,   114,   115,   116,   117,     0,
     120,   121,   119,   118,   122,     0,   116,   117,     0,     0,
     121,     0,   118,   122,     0,     0,     0,   105,   106,     0,
     119,   107,   108,   109,   110,   111,     0,   301,   121,   119,
       0,   122,     0,     0,     0,     0,   120,   121,     0,     0,
     122
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-122))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       2,     7,    58,    72,    73,    74,    50,    10,    77,   130,
       3,     4,    14,    15,     7,     8,     9,    10,    11,     3,
       4,    10,    78,     7,     8,     9,    10,    11,     3,     4,
      20,    75,     7,     8,     9,    10,    11,    51,     3,     4,
      58,    43,     7,     8,     9,    10,    11,     3,     4,    50,
      51,     7,     8,     9,    10,    11,    62,    60,    14,    15,
      78,    75,    52,    53,    62,     6,    78,     3,     0,    10,
      70,    60,     8,     9,    75,    77,     3,     4,    50,    77,
       7,     8,     9,    10,    11,    55,    79,     8,     9,    63,
      17,    93,    66,    63,    76,    79,    66,    78,    79,    77,
      66,    76,    66,   105,   106,   107,   108,   109,   110,   111,
     231,    76,    12,    72,    73,    74,    66,    66,    66,    75,
     126,    66,    66,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    66,   135,    34,    35,    36,    37,    21,    22,
      21,    22,    66,    66,    66,    24,    66,    47,    48,    32,
      33,    32,    33,    21,    22,    38,    66,    38,    66,    59,
     162,    66,   164,    66,    43,    66,    66,    67,    68,    66,
      66,    71,    55,    66,    55,    75,    66,    66,    66,    66,
      63,    75,    63,    66,    75,    66,    66,    55,    20,   191,
      76,    76,    71,    71,    71,    63,    71,    80,    66,    80,
      79,    71,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    71,    94,    95,    71,    71,     3,
       4,    71,   101,     7,     8,     9,    10,    11,    63,    13,
      78,    62,   234,   235,   113,    12,   115,   116,   117,   118,
      78,   120,   121,   122,    78,    78,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    78,   135,    34,    35,    36,
      37,    21,    22,   265,    78,    78,    21,    22,    21,    22,
      47,    48,    32,    33,     9,    71,    54,    32,    38,    32,
      33,    71,    59,   162,    71,   164,    62,    79,    71,    66,
      67,    68,    19,    79,    71,    55,     6,    79,    75,   301,
      55,    80,    55,    63,   183,    55,    66,   163,    63,   313,
      63,    66,   191,    66,    78,   194,   195,   312,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,     3,     4,   165,
      -1,     7,     8,     9,    10,    11,    -1,    13,    -1,     3,
       4,    -1,    -1,    -1,     8,     9,    10,    12,    -1,   238,
     239,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    -1,    34,
      35,    36,    37,     3,     4,    -1,   265,     7,     8,     9,
      10,    11,    47,    48,    -1,   274,    -1,   276,   277,   278,
     279,   280,   281,    -1,    59,    -1,    -1,     5,    -1,    -1,
      -1,    66,    67,    68,    12,    -1,    71,    -1,    -1,   298,
      75,    -1,    -1,    -1,   303,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    -1,    34,    35,    36,    37,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    18,    -1,    -1,    21,    22,    -1,    56,    57,
      -1,    59,    -1,    -1,    21,    22,    32,    33,    66,    67,
      68,    69,    38,    71,    -1,    32,    33,    75,    -1,     3,
       4,    38,    21,    22,     8,     9,    10,    11,    -1,    55,
      -1,    21,    22,    32,    33,    -1,    62,    63,    55,    38,
      66,    -1,    32,    33,    -1,    -1,    63,    -1,    38,    66,
      -1,    -1,    -1,    79,    -1,    -1,    55,    21,    22,    -1,
      -1,    -1,    79,    -1,    63,    55,    -1,    66,    32,    33,
      -1,    21,    22,    63,    38,    -1,    66,    -1,    -1,    -1,
      79,    -1,    32,    33,    -1,    -1,    21,    22,    38,    79,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    32,    33,    63,
      21,    22,    66,    38,    -1,    55,    -1,    21,    22,    -1,
      -1,    32,    33,    63,    -1,    79,    66,    38,    32,    33,
      55,    21,    22,    -1,    38,    -1,    -1,    -1,    63,    79,
      -1,    66,    32,    33,    55,    -1,    21,    22,    38,    -1,
      -1,    55,    63,    -1,    79,    66,    -1,    32,    33,    63,
      21,    22,    66,    38,    -1,    55,    -1,    -1,    79,    -1,
      -1,    32,    33,    63,    -1,    79,    66,    38,    -1,    -1,
      55,    21,    22,    -1,    -1,    -1,    -1,    -1,    63,    79,
      -1,    66,    32,    33,    55,    21,    22,    -1,    38,    21,
      22,    -1,    63,    -1,    79,    66,    32,    33,    -1,    -1,
      32,    33,    38,    21,    22,    55,    38,    -1,    79,    -1,
      -1,    -1,    -1,    63,    32,    33,    66,    -1,    -1,    55,
      38,    21,    22,    55,    -1,    -1,    -1,    63,    -1,    79,
      66,    63,    32,    33,    66,    -1,    -1,    55,    38,    21,
      22,    21,    22,    79,    -1,    63,    -1,    79,    66,    -1,
      32,    33,    32,    33,    -1,    55,    38,    -1,    38,    21,
      22,    79,    -1,    63,    -1,    -1,    66,    -1,    -1,    -1,
      32,    33,    -1,    55,    -1,    55,    38,    21,    22,    79,
      -1,    63,    -1,    63,    66,    -1,    66,    -1,    32,    33,
      -1,    21,    22,    55,    38,    21,    22,    79,    -1,    79,
      -1,    63,    32,    33,    66,    -1,    32,    33,    38,    21,
      22,    55,    38,    -1,    -1,    -1,    -1,    79,    -1,    63,
      32,    33,    66,    -1,    -1,    55,    38,    21,    22,    55,
      -1,    -1,    -1,    63,    -1,    79,    66,    63,    32,    33,
      66,    -1,    -1,    55,    38,    21,    22,    21,    22,    79,
      -1,    63,    -1,    79,    66,    -1,    32,    33,    32,    33,
      -1,    55,    38,    -1,    38,    -1,    78,    -1,    -1,    63,
      -1,    -1,    66,    -1,    21,    22,    -1,    -1,    -1,    55,
      -1,    55,    -1,    -1,    78,    32,    33,    63,    -1,    63,
      66,    38,    66,    -1,    21,    22,    -1,    -1,    21,    22,
      -1,    -1,    78,    -1,    78,    32,    33,    -1,    55,    32,
      33,    38,    21,    22,    -1,    38,    63,    -1,    -1,    66,
      -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    55,    38,
      -1,    78,    55,    21,    22,    -1,    63,    -1,    -1,    66,
      63,    -1,    -1,    66,    32,    33,    55,    21,    22,    -1,
      38,    78,    -1,    -1,    63,    78,    -1,    66,    32,    33,
      21,    22,    -1,    -1,    38,    -1,    -1,    55,    -1,    78,
      -1,    32,    33,    -1,    -1,    63,    -1,    38,    66,    -1,
      -1,    55,    -1,    -1,    21,    22,    21,    22,    -1,    63,
      78,    -1,    66,    -1,    55,    32,    33,    32,    33,    -1,
      -1,    38,    63,    38,    78,    66,    -1,    -1,    -1,    -1,
      21,    22,    -1,    -1,    -1,    -1,    -1,    78,    55,    -1,
      55,    32,    33,    21,    22,    -1,    63,    38,    63,    66,
      -1,    66,    -1,    -1,    32,    33,    21,    22,    -1,    -1,
      38,    78,    -1,    78,    55,    -1,    -1,    32,    33,    21,
      22,    -1,    63,    38,    -1,    66,    -1,    55,    -1,    -1,
      32,    33,    21,    22,    -1,    63,    38,    78,    66,    -1,
      55,    -1,    -1,    32,    33,    21,    22,    -1,    63,    38,
      78,    66,    -1,    55,    -1,    -1,    32,    33,    21,    22,
      -1,    63,    38,    78,    66,    -1,    55,    -1,    -1,    32,
      33,    -1,    -1,    -1,    63,    38,    78,    66,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    13,    -1,    63,    -1,    78,
      66,    -1,    55,    21,    22,    -1,    -1,    -1,    -1,    -1,
      63,    -1,    78,    66,    32,    33,    -1,    -1,    -1,    -1,
      38,    -1,    14,    15,    -1,    78,    18,    -1,    -1,    21,
      22,    14,    15,    -1,    -1,    -1,    -1,    55,    21,    22,
      32,    33,    -1,    -1,    -1,    63,    38,    -1,    66,    32,
      33,    -1,    17,    -1,    -1,    38,    21,    22,    -1,    -1,
      -1,    -1,    18,    55,    -1,    21,    22,    32,    33,    -1,
      62,    63,    55,    38,    66,    -1,    32,    33,    -1,    -1,
      63,    -1,    38,    66,    -1,    -1,    -1,     3,     4,    -1,
      55,     7,     8,     9,    10,    11,    -1,    13,    63,    55,
      -1,    66,    -1,    -1,    -1,    -1,    62,    63,    -1,    -1,
      66
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    50,    51,    75,    82,    84,    85,    86,    87,    88,
      89,    90,    91,    92,     5,    12,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    34,    35,    36,    37,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      56,    57,    59,    66,    67,    68,    69,    71,    75,    97,
      99,   100,   103,   104,    99,    78,     0,    70,    83,    88,
      89,    87,    86,    88,    90,    76,    77,    99,    99,    66,
      66,    66,    66,    66,    66,    66,    66,    12,    59,    66,
     103,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    75,   106,   107,    99,
     103,    66,    72,    73,    74,     3,     4,     7,     8,     9,
      10,    11,    76,    18,    21,    22,    32,    33,    38,    55,
      62,    63,    66,    76,    91,    76,    86,    76,    20,    52,
      53,    93,    94,    95,    96,    13,    71,    71,   103,    71,
      71,    71,    71,    71,    99,   106,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,    99,
     103,   103,    62,    77,    58,    78,    79,    79,   103,    99,
      99,    99,    99,    99,    99,    99,   103,    71,    71,   103,
     103,   103,   103,    63,   105,   103,    71,   103,   103,    66,
      94,    62,    99,   103,    78,    78,    79,    78,    78,    78,
      78,    78,    13,    58,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    79,    78,    78,    99,
     103,    93,    99,   103,   107,    79,   103,    77,    80,    79,
      71,    54,    99,   103,    14,    15,    75,   101,    14,    15,
      98,   102,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,    71,    71,    62,    80,    71,    79,    94,
      99,    99,   103,   103,    78,    79,    78,    78,    78,    78,
      78,    78,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    99,   103,    19,    80,
      17,    13,    17,    13,   103,   103,   103,   103,   103,   103,
     103,   103,    99,   103,    79,    79,    79,    79,    79,    79,
     101,    98
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
#line 180 "cvc.y"
    {
  parserInterface->letMgr._parser_symbol_table.clear();
}
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 184 "cvc.y"
    {
  parserInterface->letMgr._parser_symbol_table.clear(); 
}
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 190 "cvc.y"
    {
  parserInterface->getUserFlags().print_counterexample_flag = true;
  (GlobalSTP->Ctr_Example)->PrintCounterExample(true);
}
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 212 "cvc.y"
    { 
  ((ASTVec*)AssertsQuery)->push_back(parserInterface->CreateNode(TRUE));
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[(1) - (1)].node));                       
  delete (yyvsp[(1) - (1)].node);
}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 218 "cvc.y"
    { 
  ((ASTVec*)AssertsQuery)->push_back(parserInterface->CreateNode(TRUE));
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[(2) - (2)].node));
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 224 "cvc.y"
    {
  ASTVec aaa = parserInterface->GetAsserts();
  if(aaa.size() == 0)
    {
      yyerror("Fatal Error: parsing:  GetAsserts() call: no assertions: ");
    }

  ASTNode asserts = 
    aaa.size() == 1 ? 
    aaa[0] :
    parserInterface->CreateNode(AND, aaa);
  ((ASTVec*)AssertsQuery)->push_back(asserts);
  ((ASTVec*)AssertsQuery)->push_back(*(yyvsp[(2) - (2)].node));
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 242 "cvc.y"
    {
  delete (yyvsp[(2) - (2)].vec);
}
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 246 "cvc.y"
    {
  delete (yyvsp[(1) - (1)].vec);
}
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 250 "cvc.y"
    {
  delete (yyvsp[(3) - (3)].vec);
}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 270 "cvc.y"
    {
  (yyval.vec) = new ASTVec;
  (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
  parserInterface->AddAssert(*(yyvsp[(1) - (1)].node));
  delete (yyvsp[(1) - (1)].node);
}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 277 "cvc.y"
    {
  (yyvsp[(1) - (2)].vec)->push_back(*(yyvsp[(2) - (2)].node));
  parserInterface->AddAssert(*(yyvsp[(2) - (2)].node));
  (yyval.vec) = (yyvsp[(1) - (2)].vec);
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 286 "cvc.y"
    { 
  (yyval.node) = (yyvsp[(2) - (3)].node);
 }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 291 "cvc.y"
    { parserInterface->AddQuery(*(yyvsp[(2) - (3)].node)); (yyval.node) = (yyvsp[(2) - (3)].node);}
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 297 "cvc.y"
    {
}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 300 "cvc.y"
    {
}
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 305 "cvc.y"
    {
  for(vector<char*>::iterator i=(yyvsp[(1) - (3)].stringVec)->begin(),iend=(yyvsp[(1) - (3)].stringVec)->end();i!=iend;i++) {
    ASTNode s = BEEV::parserInterface->LookupOrCreateSymbol(*i);
    s.SetIndexWidth((yyvsp[(3) - (3)].indexvaluewidth).indexwidth);
    s.SetValueWidth((yyvsp[(3) - (3)].indexvaluewidth).valuewidth);
    parserInterface->letMgr._parser_symbol_table.insert(s);
    ParserBM->ListOfDeclaredVars.push_back(s);
  }
  delete (yyvsp[(1) - (3)].stringVec);
}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 316 "cvc.y"
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[(5) - (5)].node));
  if((yyvsp[(3) - (5)].indexvaluewidth).indexwidth != (yyvsp[(5) - (5)].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[(3) - (5)].indexvaluewidth).valuewidth != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
                         
  for(vector<char*>::iterator i=(yyvsp[(1) - (5)].stringVec)->begin(),iend=(yyvsp[(1) - (5)].stringVec)->end();i!=iend;i++) {                         
    parserInterface->letMgr.LetExprMgr(*i,*(yyvsp[(5) - (5)].node));
  }
    delete (yyvsp[(5) - (5)].node);
    delete (yyvsp[(1) - (5)].stringVec);
}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 331 "cvc.y"
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[(5) - (5)].node));
  if((yyvsp[(3) - (5)].indexvaluewidth).indexwidth != (yyvsp[(5) - (5)].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[(3) - (5)].indexvaluewidth).valuewidth != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
                         
  for(vector<char*>::iterator i=(yyvsp[(1) - (5)].stringVec)->begin(),iend=(yyvsp[(1) - (5)].stringVec)->end();i!=iend;i++) {                         
    parserInterface->letMgr.LetExprMgr(*i,*(yyvsp[(5) - (5)].node));
  }
  delete (yyvsp[(5) - (5)].node);
  delete (yyvsp[(1) - (5)].stringVec);
}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 348 "cvc.y"
    {
  (yyval.stringVec) = new vector<char*>();                      
  (yyval.stringVec)->push_back((yyvsp[(1) - (1)].str));
 // delete $1;
}
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 354 "cvc.y"
    {
  (yyvsp[(3) - (3)].stringVec)->push_back((yyvsp[(1) - (3)].str));
  (yyval.stringVec) = (yyvsp[(3) - (3)].stringVec);
 // delete $1;
}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 362 "cvc.y"
    {
  (yyval.stringVec) = new vector<char*>((yyvsp[(1) - (1)].stringVec)->rbegin(),(yyvsp[(1) - (1)].stringVec)->rend());
  delete (yyvsp[(1) - (1)].stringVec);
}
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 377 "cvc.y"
    { (yyval.indexvaluewidth) = (yyvsp[(1) - (1)].indexvaluewidth); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 378 "cvc.y"
    { (yyval.indexvaluewidth) = (yyvsp[(1) - (1)].indexvaluewidth); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 379 "cvc.y"
    { (yyval.indexvaluewidth) = (yyvsp[(1) - (1)].indexvaluewidth); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 383 "cvc.y"
    {
  /*((indexwidth is 0) && (valuewidth>0)) iff type is BV*/
  (yyval.indexvaluewidth).indexwidth = 0;
  unsigned int length = (yyvsp[(3) - (4)].uintval);
  if(length > 0) {
    (yyval.indexvaluewidth).valuewidth = length;
  }
  else
    FatalError("Fatal Error: parsing: BITVECTORS must be of positive length: \n");
}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 395 "cvc.y"
    {
  (yyval.indexvaluewidth).indexwidth = 0;
  (yyval.indexvaluewidth).valuewidth = 0;
}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 401 "cvc.y"
    {
  (yyval.indexvaluewidth).indexwidth = (yyvsp[(2) - (4)].indexvaluewidth).valuewidth;
  (yyval.indexvaluewidth).valuewidth = (yyvsp[(4) - (4)].indexvaluewidth).valuewidth;
}
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 409 "cvc.y"
    {
  unsigned int width = (yyvsp[(4) - (5)].node)->GetValueWidth();
  if (width != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");                   
  if((yyvsp[(4) - (5)].node)->GetIndexWidth() != (yyvsp[(5) - (5)].node)->GetIndexWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");

  BVTypeCheck(*(yyvsp[(2) - (5)].node));
  BVTypeCheck(*(yyvsp[(4) - (5)].node));
  BVTypeCheck(*(yyvsp[(5) - (5)].node));
  (yyval.node) = new ASTNode(parserInterface->nf->CreateArrayTerm(ITE,(yyvsp[(5) - (5)].node)->GetIndexWidth(), width, *(yyvsp[(2) - (5)].node), *(yyvsp[(4) - (5)].node), *(yyvsp[(5) - (5)].node)));
  delete (yyvsp[(2) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 426 "cvc.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 428 "cvc.y"
    {
  unsigned int width = (yyvsp[(2) - (5)].node)->GetValueWidth();
  if (width != (yyvsp[(4) - (5)].node)->GetValueWidth() || width != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");
  if ((yyvsp[(2) - (5)].node)->GetIndexWidth() != (yyvsp[(4) - (5)].node)->GetValueWidth() || (yyvsp[(2) - (5)].node)->GetIndexWidth() != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Width mismatch in IF-THEN-ELSE");

  BVTypeCheck(*(yyvsp[(2) - (5)].node));
  BVTypeCheck(*(yyvsp[(4) - (5)].node));
  BVTypeCheck(*(yyvsp[(5) - (5)].node));                     
  (yyval.node) = new ASTNode(parserInterface->nf->CreateArrayTerm(ITE, (yyvsp[(5) - (5)].node)->GetIndexWidth(), width, *(yyvsp[(2) - (5)].node), *(yyvsp[(4) - (5)].node), *(yyvsp[(5) - (5)].node)));
  delete (yyvsp[(2) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 447 "cvc.y"
    {
  (yyval.node) = (yyvsp[(2) - (3)].node); 
}
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 451 "cvc.y"
    {  
  (yyval.node) = new ASTNode(parserInterface->letMgr.ResolveID(*(yyvsp[(1) - (1)].node))); delete (yyvsp[(1) - (1)].node);
}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 455 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(PARAMBOOL,*(yyvsp[(1) - (4)].node),*(yyvsp[(3) - (4)].node)));
  delete (yyvsp[(1) - (4)].node);
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 461 "cvc.y"
    {
  unsigned int width = (yyvsp[(3) - (6)].node)->GetValueWidth();
  if(width <= (unsigned)(yyvsp[(5) - (6)].uintval))
    yyerror("Fatal Error: BOOLEXTRACT: trying to boolextract a bit which beyond range");
                         
  ASTNode hi  =  parserInterface->CreateBVConst(32, (yyvsp[(5) - (6)].uintval));
  ASTNode low =  parserInterface->CreateBVConst(32, (yyvsp[(5) - (6)].uintval));
  ASTNode n = parserInterface->nf->CreateTerm(BVEXTRACT,1,*(yyvsp[(3) - (6)].node),hi,low);
  ASTNode one = parserInterface->CreateBVConst(1,1);                   
  ASTNode * out = new ASTNode(parserInterface->nf->CreateNode(EQ,n,one));

  (yyval.node) = out;
  delete (yyvsp[(3) - (6)].node);
}
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 476 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(EQ, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 483 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(NOT, parserInterface->nf->CreateNode(EQ, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node))));
  (yyval.node) = n;
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 490 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(NOT, *(yyvsp[(2) - (2)].node)));
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 495 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(OR, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 501 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(NOR, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 507 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(AND, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 513 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(NAND, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 519 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(IMPLIES, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 525 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(IFF, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 531 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(XOR, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 537 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVLT, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 544 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVGT, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 551 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVLE, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 558 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVGE, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 565 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVSLT, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 572 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVSGT, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 579 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVSLE, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 586 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateNode(BVSGE, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 594 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->CreateNode(TRUE)); 
  assert((yyval.node)->GetIndexWidth() == 0);
  assert((yyval.node)->GetValueWidth() == 0);
}
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 600 "cvc.y"
    { 
  (yyval.node) = new ASTNode(parserInterface->CreateNode(FALSE)); 
  assert((yyval.node)->GetIndexWidth() == 0);
  assert((yyval.node)->GetValueWidth() == 0);
}
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 607 "cvc.y"
    {
  (yyval.node) = (yyvsp[(4) - (4)].node);
  //Cleanup the LetIDToExprMap
  parserInterface->letMgr.CleanupLetIDMap();
}
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 616 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(ITE, *(yyvsp[(2) - (5)].node), *(yyvsp[(4) - (5)].node), *(yyvsp[(5) - (5)].node)));
  delete (yyvsp[(2) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 624 "cvc.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 626 "cvc.y"
    {
  (yyval.node) = new ASTNode(parserInterface->nf->CreateNode(ITE, *(yyvsp[(2) - (5)].node), *(yyvsp[(4) - (5)].node), *(yyvsp[(5) - (5)].node)));
  delete (yyvsp[(2) - (5)].node);
  delete (yyvsp[(4) - (5)].node);
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 632 "cvc.y"
    {
   cerr << "Unresolved symbol:" << (yyvsp[(1) - (1)].str) << endl;
   yyerror("bad symbol"); 
}
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 640 "cvc.y"
    {
  (yyval.vec) = new ASTVec;
  BVTypeCheck(*(yyvsp[(1) - (1)].node));
  (yyval.vec)->push_back(*(yyvsp[(1) - (1)].node));
  delete (yyvsp[(1) - (1)].node);
}
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 647 "cvc.y"
    {
  (yyvsp[(1) - (3)].vec)->push_back(*(yyvsp[(3) - (3)].node));
  BVTypeCheck(*(yyvsp[(3) - (3)].node));
  (yyval.vec) = (yyvsp[(1) - (3)].vec); 
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 656 "cvc.y"
    { (yyval.node) = new ASTNode(parserInterface->letMgr.ResolveID(*(yyvsp[(1) - (1)].node))); delete (yyvsp[(1) - (1)].node);}
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 657 "cvc.y"
    { (yyval.node) = (yyvsp[(2) - (3)].node); }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 658 "cvc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 660 "cvc.y"
    {
  BVTypeCheck(*(yyvsp[(3) - (4)].node));
  ASTNode one = parserInterface->CreateBVConst(1,1);
  ASTNode zero = parserInterface->CreateBVConst(1,0);

  //return ITE(*$3, length(1), 0bin1, 0bin0)
  (yyval.node) = new ASTNode(parserInterface->nf->CreateTerm(ITE,1,*(yyvsp[(3) - (4)].node),one,zero));
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 670 "cvc.y"
    { 
  std::string vals((yyvsp[(2) - (2)].str));
  (yyval.node) = new ASTNode(parserInterface->CreateBVConst(vals, 2, (yyvsp[(1) - (2)].uintval)));
  free((yyvsp[(2) - (2)].str));
}
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 676 "cvc.y"
    { 
  std::string vals((yyvsp[(2) - (2)].str));
  (yyval.node) = new ASTNode(parserInterface->CreateBVConst(vals, 10, (yyvsp[(1) - (2)].uintval)));
  free((yyvsp[(2) - (2)].str));
}
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 682 "cvc.y"
    { 
  std::string vals((yyvsp[(2) - (2)].str));
  (yyval.node) = new ASTNode(parserInterface->CreateBVConst(vals, 16, (yyvsp[(1) - (2)].uintval)));
  free((yyvsp[(2) - (2)].str));
}
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 688 "cvc.y"
    {                        
  // valuewidth is same as array, indexwidth is 0.
  unsigned int width = (yyvsp[(1) - (4)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(READ, width, *(yyvsp[(1) - (4)].node), *(yyvsp[(3) - (4)].node)));
  (yyval.node) = n;

  delete (yyvsp[(1) - (4)].node);
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 698 "cvc.y"
    {
  // valuewidth is same as array, indexwidth is 0.
  unsigned int width = (yyvsp[(1) - (4)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(READ, width, *(yyvsp[(1) - (4)].node), *(yyvsp[(3) - (4)].node)));
  (yyval.node) = n;

  delete (yyvsp[(1) - (4)].node);
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 708 "cvc.y"
    {
  int width = (yyvsp[(3) - (6)].uintval) - (yyvsp[(5) - (6)].uintval) + 1;
  if (width < 0)
    yyerror("Negative width in extract");
                         
  if((unsigned)(yyvsp[(3) - (6)].uintval) >= (yyvsp[(1) - (6)].node)->GetValueWidth())
    yyerror("Parsing: Wrong width in BVEXTRACT\n");                      

  ASTNode hi  =  parserInterface->CreateBVConst(32, (yyvsp[(3) - (6)].uintval));
  ASTNode low =  parserInterface->CreateBVConst(32, (yyvsp[(5) - (6)].uintval));
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVEXTRACT, width, *(yyvsp[(1) - (6)].node),hi,low));
  (yyval.node) = n;
  delete (yyvsp[(1) - (6)].node);
}
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 723 "cvc.y"
    {
  unsigned int width = (yyvsp[(2) - (2)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNEG, width, *(yyvsp[(2) - (2)].node)));
  (yyval.node) = n;
  delete (yyvsp[(2) - (2)].node);
}
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 730 "cvc.y"
    {
  unsigned int width = (yyvsp[(1) - (3)].node)->GetValueWidth();
  if (width != (yyvsp[(3) - (3)].node)->GetValueWidth()) {
    yyerror("Width mismatch in AND");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVAND, width, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 741 "cvc.y"
    {
  unsigned int width = (yyvsp[(1) - (3)].node)->GetValueWidth();
  if (width != (yyvsp[(3) - (3)].node)->GetValueWidth()) {
    yyerror("Width mismatch in OR");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVOR, width, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node))); 
  (yyval.node) = n;
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 752 "cvc.y"
    {
  unsigned int width = (yyvsp[(3) - (6)].node)->GetValueWidth();
  if (width != (yyvsp[(5) - (6)].node)->GetValueWidth()) {
    yyerror("Width mismatch in XOR");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVXOR, width, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 763 "cvc.y"
    {
  unsigned int width = (yyvsp[(3) - (6)].node)->GetValueWidth();
  if (width != (yyvsp[(5) - (6)].node)->GetValueWidth()) {
    yyerror("Width mismatch in NAND");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNAND, width, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;

  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 775 "cvc.y"
    {
  unsigned int width = (yyvsp[(3) - (6)].node)->GetValueWidth();
  if (width != (yyvsp[(5) - (6)].node)->GetValueWidth()) {
    yyerror("Width mismatch in NOR");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVNOR, width, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;

  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 787 "cvc.y"
    {
  unsigned int width = (yyvsp[(3) - (6)].node)->GetValueWidth();
  if (width != (yyvsp[(5) - (6)].node)->GetValueWidth()) {
    yyerror("Width mismatch in NOR");
  }
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVXNOR, width, *(yyvsp[(3) - (6)].node), *(yyvsp[(5) - (6)].node)));
  (yyval.node) = n;

  delete (yyvsp[(3) - (6)].node);
  delete (yyvsp[(5) - (6)].node);
}
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 799 "cvc.y"
    {
  //width of the expr which is being sign
  //extended. $5 is the resulting length of the
  //signextended expr
  BVTypeCheck(*(yyvsp[(3) - (6)].node));
  if((yyvsp[(3) - (6)].node)->GetValueWidth() == (yyvsp[(5) - (6)].uintval)) {
    (yyval.node) = (yyvsp[(3) - (6)].node);
  }
  else {
    ASTNode width = parserInterface->CreateBVConst(32,(yyvsp[(5) - (6)].uintval));
    ASTNode *n =  
      new ASTNode(parserInterface->nf->CreateTerm(BVSX, (yyvsp[(5) - (6)].uintval),*(yyvsp[(3) - (6)].node),width));
    (yyval.node) = n;
    delete (yyvsp[(3) - (6)].node);
  }
}
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 816 "cvc.y"
    {
  unsigned int width = (yyvsp[(1) - (3)].node)->GetValueWidth() + (yyvsp[(3) - (3)].node)->GetValueWidth();
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT, width, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  (yyval.node) = n;
                         
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 825 "cvc.y"
    {
  if (0 == (yyvsp[(3) - (3)].uintval))
  	{
  	(yyval.node) = (yyvsp[(1) - (3)].node);
  	}
  else
  {
  ASTNode zero_bits = parserInterface->CreateZeroConst((yyvsp[(3) - (3)].uintval));
  ASTNode * n = 
    new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT,
                                     (yyvsp[(1) - (3)].node)->GetValueWidth() + (yyvsp[(3) - (3)].uintval), *(yyvsp[(1) - (3)].node), zero_bits));
  (yyval.node) = n;
  delete (yyvsp[(1) - (3)].node);
  }
}
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 841 "cvc.y"
    {
  ASTNode len = parserInterface->CreateZeroConst((yyvsp[(3) - (3)].uintval));
  unsigned int w = (yyvsp[(1) - (3)].node)->GetValueWidth();

  //the amount by which you are rightshifting
  //is less-than/equal-to the length of input
  //bitvector
  if((unsigned)(yyvsp[(3) - (3)].uintval) < w) {
    ASTNode hi = parserInterface->CreateBVConst(32,w-1);
    ASTNode low = parserInterface->CreateBVConst(32,(yyvsp[(3) - (3)].uintval));
    ASTNode extract = parserInterface->nf->CreateTerm(BVEXTRACT,w-(yyvsp[(3) - (3)].uintval),*(yyvsp[(1) - (3)].node),hi,low);
    ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVCONCAT, w,len, extract));
    (yyval.node) = n;
  }
  else
    (yyval.node) = new ASTNode(parserInterface->CreateZeroConst(w));

  delete (yyvsp[(1) - (3)].node);
}
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 861 "cvc.y"
    {
  // VARIABLE RIGHT SHIFT
  //
  // $1 (THEEXPR) is being shifted
  //
  // $3 is the variable shift amount
  unsigned int width = (yyvsp[(1) - (3)].node)->GetValueWidth();
  ASTNode * ret = new ASTNode(parserInterface->nf->CreateTerm(BVRIGHTSHIFT, width, *(yyvsp[(1) - (3)].node), *(yyvsp[(3) - (3)].node)));
  BVTypeCheck(*ret);
  //cout << *ret;

  (yyval.node) = ret;
  delete (yyvsp[(1) - (3)].node);
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 877 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVPLUS, (yyvsp[(3) - (6)].uintval), *(yyvsp[(5) - (6)].vec)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (6)].vec);
}
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 884 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVSUB, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 892 "cvc.y"
    {
  unsigned width = (yyvsp[(3) - (4)].node)->GetValueWidth();
  ASTNode * n =  new ASTNode(parserInterface->nf->CreateTerm(BVUMINUS,width,*(yyvsp[(3) - (4)].node)));
  (yyval.node) = n;
  delete (yyvsp[(3) - (4)].node);
}
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 899 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVMULT, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 907 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVDIV, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 915 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(BVMOD, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 923 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(SBVDIV, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;

  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 931 "cvc.y"
    {
  ASTNode * n = new ASTNode(parserInterface->nf->CreateTerm(SBVREM, (yyvsp[(3) - (8)].uintval), *(yyvsp[(5) - (8)].node), *(yyvsp[(7) - (8)].node)));
  (yyval.node) = n;
  delete (yyvsp[(5) - (8)].node);
  delete (yyvsp[(7) - (8)].node);
}
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 937 "cvc.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 940 "cvc.y"
    {
  (yyval.node) = (yyvsp[(4) - (4)].node);
}
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 943 "cvc.y"
    {
   cerr << "Unresolved symbol:" << (yyvsp[(1) - (1)].str) << endl;
   yyerror("bad symbol"); 
}
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 951 "cvc.y"
    {
  ASTNode * result;
  unsigned int width = (yyvsp[(1) - (3)].node)->GetValueWidth();

  ASTNodeMap::iterator it = (yyvsp[(3) - (3)].Index_To_UpdateValue)->begin();
  ASTNodeMap::iterator itend = (yyvsp[(3) - (3)].Index_To_UpdateValue)->end();
  result = new ASTNode(parserInterface->nf->CreateArrayTerm(WRITE,
                                            (yyvsp[(1) - (3)].node)->GetIndexWidth(),
                                            width,
                                            *(yyvsp[(1) - (3)].node),
                                            (*it).first,
                                            (*it).second));
  BVTypeCheck(*result);
  for(it++;it!=itend;it++) {
    result = new ASTNode(parserInterface->nf->CreateArrayTerm(WRITE,
                                              (yyvsp[(1) - (3)].node)->GetIndexWidth(),
                                              width,
                                              *result,
                                              (*it).first,
                                              (*it).second));
    BVTypeCheck(*result);
  }
  BVTypeCheck(*result);
  (yyval.node) = result;
  delete (yyvsp[(3) - (3)].Index_To_UpdateValue);
  delete (yyvsp[(1) - (3)].node);
}
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 981 "cvc.y"
    {
  (yyval.Index_To_UpdateValue) = new ASTNodeMap();
  (*(yyval.Index_To_UpdateValue))[*(yyvsp[(2) - (5)].node)] = *(yyvsp[(5) - (5)].node);         
  delete (yyvsp[(2) - (5)].node);
  delete (yyvsp[(5) - (5)].node);        
}
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 988 "cvc.y"
    {                   
  (*(yyvsp[(1) - (7)].Index_To_UpdateValue))[*(yyvsp[(4) - (7)].node)] = *(yyvsp[(7) - (7)].node);
  delete (yyvsp[(4) - (7)].node);
  delete (yyvsp[(7) - (7)].node);
}
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 1001 "cvc.y"
    {
  //Expr must typecheck
  BVTypeCheck(*(yyvsp[(3) - (3)].node));

  //set the valuewidth of the identifier
  
  //populate the hashtable from LET-var -->
  //LET-exprs and then process them:
  //
  //1. ensure that LET variables do not clash
  //1. with declared variables.
  //
  //2. Ensure that LET variables are not
  //2. defined more than once
  parserInterface->letMgr.LetExprMgr((yyvsp[(1) - (3)].str),*(yyvsp[(3) - (3)].node));
  free((yyvsp[(1) - (3)].str));
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 1020 "cvc.y"
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[(5) - (5)].node));
                          
  if((yyvsp[(3) - (5)].indexvaluewidth).indexwidth != (yyvsp[(5) - (5)].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[(3) - (5)].indexvaluewidth).valuewidth != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");

  parserInterface->letMgr.LetExprMgr((yyvsp[(1) - (5)].str),*(yyvsp[(5) - (5)].node));
  free( (yyvsp[(1) - (5)].str));
  delete (yyvsp[(5) - (5)].node);
}
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 1034 "cvc.y"
    {
  //Expr must typecheck
  BVTypeCheck(*(yyvsp[(3) - (3)].node));

  //Do LET-expr management
  parserInterface->letMgr.LetExprMgr((yyvsp[(1) - (3)].str),*(yyvsp[(3) - (3)].node));
  free( (yyvsp[(1) - (3)].str));
  delete (yyvsp[(3) - (3)].node);
}
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 1044 "cvc.y"
    {
  //do type checking. if doesn't pass then abort
  BVTypeCheck(*(yyvsp[(5) - (5)].node));

  if((yyvsp[(3) - (5)].indexvaluewidth).indexwidth != (yyvsp[(5) - (5)].node)->GetIndexWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");
  if((yyvsp[(3) - (5)].indexvaluewidth).valuewidth != (yyvsp[(5) - (5)].node)->GetValueWidth())
    yyerror("Fatal Error: parsing: LET Expr: Type check fail: ");

  //Do LET-expr management
  parserInterface->letMgr.LetExprMgr((yyvsp[(1) - (5)].str),*(yyvsp[(5) - (5)].node));
  free( (yyvsp[(1) - (5)].str));
  delete (yyvsp[(5) - (5)].node);
}
    break;



/* Line 1806 of yacc.c  */
#line 3222 "parsecvc.cpp"
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
#line 1060 "cvc.y"


