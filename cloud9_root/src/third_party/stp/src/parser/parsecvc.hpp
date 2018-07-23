/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
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

/* Line 2068 of yacc.c  */
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



/* Line 2068 of yacc.c  */
#line 145 "parsecvc.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE cvclval;


