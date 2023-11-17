/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOK_LBRACE = 258,
    TOK_COUT = 259,
    TOK_STREAMOUTPUT = 260,
    TOK_RBRACE = 261,
    TOK_RETURN = 262,
    TOK_ADD = 263,
    TOK_SUB = 264,
    TOK_MUL = 265,
    TOK_DIV = 266,
    TOK_NEG = 267,
    TOK_LPAREN = 268,
    TOK_RPAREN = 269,
    TOK_SEMI = 270,
    TOK_LETTER = 271,
    TOK_EQUALS = 272,
    TOK_UINT = 273,
    TOK_TYPE = 274,
    TOK_IDENTIFIER = 275
  };
#endif
/* Tokens.  */
#define TOK_LBRACE 258
#define TOK_COUT 259
#define TOK_STREAMOUTPUT 260
#define TOK_RBRACE 261
#define TOK_RETURN 262
#define TOK_ADD 263
#define TOK_SUB 264
#define TOK_MUL 265
#define TOK_DIV 266
#define TOK_NEG 267
#define TOK_LPAREN 268
#define TOK_RPAREN 269
#define TOK_SEMI 270
#define TOK_LETTER 271
#define TOK_EQUALS 272
#define TOK_UINT 273
#define TOK_TYPE 274
#define TOK_IDENTIFIER 275

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 101 "parser.y"

    int number;
    char* string;

#line 102 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
