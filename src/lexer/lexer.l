%option noyywrap nodefault yylineno nounput noinput
%x BLOCK_COMMENT

%{
/*
 * Formal Flex specification for the course-required scanner.
 * The default portable build uses lexer.c so the repository can build on a
 * machine without generator tools. Run `make flex-bison-check` to regenerate
 * the scanner and parser sources into build/generated/.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "common.h"
#include "parser.tab.h"

extern int fb_lexical_errors;
static int block_comment_start_line = 1;
%}

DIGIT       [0-9]
LETTER      [A-Za-z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*
INT_LIT     {DIGIT}+
FLOAT_LIT   {DIGIT}+\.{DIGIT}+
WS          [ \t\r]+

%%

{WS}                    ;
\n                      ;
"//"[^\n]*              ;
"/*"                    { block_comment_start_line = yylineno; BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/"     { BEGIN(INITIAL); }
<BLOCK_COMMENT>\n       ;
<BLOCK_COMMENT>.        ;
<BLOCK_COMMENT><<EOF>>  {
                            fprintf(stderr,
                                    "[%s] Unterminated block comment at line %d: %s\n",
                                    error_code_to_string(ERR_LEXICAL_UNTERMINATED_COMMENT),
                                    block_comment_start_line,
                                    error_code_description(ERR_LEXICAL_UNTERMINATED_COMMENT));
                            fb_lexical_errors++;
                            BEGIN(INITIAL);
                            return INVALID;
                        }

"int"                   { return INT_KW; }
"float"                 { return FLOAT_KW; }
"bool"                  { return BOOL_KW; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"print"                 { return PRINT; }
"true"                  { return TRUE; }
"false"                 { return FALSE; }

{FLOAT_LIT}             { yylval.text = xstrdup(yytext); return FLOAT_LITERAL; }
{INT_LIT}               { yylval.text = xstrdup(yytext); return INT_LITERAL; }
{IDENT}                 { yylval.text = xstrdup(yytext); return IDENTIFIER; }

"&&"                    { return AND; }
"||"                    { return OR; }
"=="                    { return EQ; }
"!="                    { return NE; }
"<="                    { return LE; }
">="                    { return GE; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return STAR; }
"/"                     { return SLASH; }
"%"                     { return PERCENT; }
"<"                     { return LT; }
">"                     { return GT; }
"!"                     { return NOT; }
"="                     { return ASSIGN; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
";"                     { return SEMICOLON; }

.                       {
                            fprintf(stderr,
                                    "[%s] Invalid character '%s' at line %d: %s\n",
                                    error_code_to_string(ERR_LEXICAL_INVALID_CHAR),
                                    yytext, yylineno,
                                    error_code_description(ERR_LEXICAL_INVALID_CHAR));
                            fb_lexical_errors++;
                            return INVALID;
                        }

%%
