/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
     ARRAY = 258,
     DO = 259,
     ELSE = 260,
     END = 261,
     FUNCTION = 262,
     IF = 263,
     NOT = 264,
     OF = 265,
     PROCEDURE = 266,
     PROGRAM = 267,
     THEN = 268,
     VAR = 269,
     WHILE = 270,
     IDENTIFIER = 271,
     ASSIGNMENT = 272,
     COLON = 273,
     COMMA = 274,
     DIGSEQ = 275,
     DOT = 276,
     DOTDOT = 277,
     EQUAL = 278,
     GE = 279,
     GT = 280,
     LBRAC = 281,
     LE = 282,
     LPAREN = 283,
     LT = 284,
     MINUS = 285,
     PLUS = 286,
     RBRAC = 287,
     REALNUMBER = 288,
     RPAREN = 289,
     SEMICOLON = 290,
     SLASH = 291,
     STAR = 292,
     notEQUAL = 293,
     STRING = 294,
     real = 295,
     INTEGER = 296,
     begin = 297,
     string_v = 298,
     READ = 299,
     WRITELN = 300
   };
#endif
/* Tokens.  */
#define ARRAY 258
#define DO 259
#define ELSE 260
#define END 261
#define FUNCTION 262
#define IF 263
#define NOT 264
#define OF 265
#define PROCEDURE 266
#define PROGRAM 267
#define THEN 268
#define VAR 269
#define WHILE 270
#define IDENTIFIER 271
#define ASSIGNMENT 272
#define COLON 273
#define COMMA 274
#define DIGSEQ 275
#define DOT 276
#define DOTDOT 277
#define EQUAL 278
#define GE 279
#define GT 280
#define LBRAC 281
#define LE 282
#define LPAREN 283
#define LT 284
#define MINUS 285
#define PLUS 286
#define RBRAC 287
#define REALNUMBER 288
#define RPAREN 289
#define SEMICOLON 290
#define SLASH 291
#define STAR 292
#define notEQUAL 293
#define STRING 294
#define real 295
#define INTEGER 296
#define begin 297
#define string_v 298
#define READ 299
#define WRITELN 300




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 24 "Parser.y"
{ 
    struct node * node_t ;
}
/* Line 1529 of yacc.c.  */
#line 143 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

