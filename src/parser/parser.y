%{
/*
 * Formal Bison grammar for the language specified by the project manual.
 * It constructs the same AST representation consumed by semantic.c and tac.c.
 */
#include <stdio.h>
#include <stdlib.h>

#include "ast/ast.h"
#include "util.h"

int yylex(void);
void yyerror(const char *message);
extern int yylineno;

ASTNode *fb_ast_root = NULL;
int fb_syntax_errors = 0;
int fb_lexical_errors = 0;

static ASTNode *make_binary_node(const char *operator_text, ASTNode *left,
                                 ASTNode *right, int line);
static ASTNode *make_unary_node(const char *operator_text, ASTNode *operand,
                                int line);
static ASTNode *make_statement_list(int line);
static ASTNode *adopt_as_program(ASTNode *list);
%}

%code requires {
#include "common.h"
typedef struct ASTNode ASTNode;
}

%define parse.error detailed
%locations

%union {
    char *text;
    DataType type;
    ASTNode *node;
}

%token INT_KW FLOAT_KW BOOL_KW IF ELSE WHILE PRINT TRUE FALSE
%token <text> IDENTIFIER INT_LITERAL FLOAT_LITERAL
%token PLUS MINUS STAR SLASH PERCENT LT GT LE GE EQ NE AND OR NOT ASSIGN
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON INVALID

%type <type> type
%type <node> program statement_list statement declaration assignment
%type <node> if_statement while_statement print_statement block expression
%type <node> logical_or logical_and equality relational additive multiplicative
%type <node> unary primary

%destructor { free($$); } <text>
%destructor { ast_free($$); } <node>

%start program

%%

program
    : statement_list
      {
          fb_ast_root = adopt_as_program($1);
          $$ = fb_ast_root;
      }
    ;

statement_list
    : /* empty */
      { $$ = make_statement_list(@$.first_line > 0 ? @$.first_line : 1); }
    | statement_list statement
      {
          if ($2) ast_add_child($1, $2);
          $$ = $1;
      }
    ;

statement
    : declaration      { $$ = $1; }
    | assignment       { $$ = $1; }
    | if_statement     { $$ = $1; }
    | while_statement  { $$ = $1; }
    | print_statement  { $$ = $1; }
    | block            { $$ = $1; }
    | SEMICOLON        { $$ = NULL; }
    | INVALID          { $$ = NULL; }
    | error SEMICOLON  { yyerrok; $$ = NULL; }
    ;

type
    : INT_KW           { $$ = TYPE_INT; }
    | FLOAT_KW         { $$ = TYPE_FLOAT; }
    | BOOL_KW          { $$ = TYPE_BOOL; }
    ;

declaration
    : type IDENTIFIER SEMICOLON
      {
          ASTNode *node = ast_create(NODE_DECL, type_to_string($1), @1.first_line);
          node->data_type = $1;
          ast_add_child(node, ast_make_identifier($2, @2.first_line));
          free($2);
          $$ = node;
      }
    | type IDENTIFIER ASSIGN expression SEMICOLON
      {
          ASTNode *node = ast_create(NODE_DECL, type_to_string($1), @1.first_line);
          node->data_type = $1;
          ast_add_child(node, ast_make_identifier($2, @2.first_line));
          ast_add_child(node, $4);
          free($2);
          $$ = node;
      }
    ;

assignment
    : IDENTIFIER ASSIGN expression SEMICOLON
      {
          ASTNode *node = ast_create(NODE_ASSIGN, "=", @1.first_line);
          ast_add_child(node, ast_make_identifier($1, @1.first_line));
          ast_add_child(node, $3);
          free($1);
          $$ = node;
      }
    ;

if_statement
    : IF LPAREN expression RPAREN block
      {
          ASTNode *node = ast_create(NODE_IF, "if", @1.first_line);
          ast_add_child(node, $3);
          ast_add_child(node, $5);
          $$ = node;
      }
    | IF LPAREN expression RPAREN block ELSE block
      {
          ASTNode *node = ast_create(NODE_IF, "if", @1.first_line);
          ast_add_child(node, $3);
          ast_add_child(node, $5);
          ast_add_child(node, $7);
          $$ = node;
      }
    ;

while_statement
    : WHILE LPAREN expression RPAREN block
      {
          ASTNode *node = ast_create(NODE_WHILE, "while", @1.first_line);
          ast_add_child(node, $3);
          ast_add_child(node, $5);
          $$ = node;
      }
    ;

print_statement
    : PRINT expression SEMICOLON
      {
          ASTNode *node = ast_create(NODE_PRINT, "print", @1.first_line);
          ast_add_child(node, $2);
          $$ = node;
      }
    ;

block
    : LBRACE statement_list RBRACE
      {
          $2->kind = NODE_BLOCK;
          free($2->text);
          $2->text = xstrdup("block");
          $2->line = @1.first_line;
          $$ = $2;
      }
    ;

expression       : logical_or { $$ = $1; } ;

logical_or
    : logical_and                    { $$ = $1; }
    | logical_or OR logical_and      { $$ = make_binary_node("||", $1, $3, @2.first_line); }
    ;

logical_and
    : equality                       { $$ = $1; }
    | logical_and AND equality       { $$ = make_binary_node("&&", $1, $3, @2.first_line); }
    ;

equality
    : relational                     { $$ = $1; }
    | equality EQ relational         { $$ = make_binary_node("==", $1, $3, @2.first_line); }
    | equality NE relational         { $$ = make_binary_node("!=", $1, $3, @2.first_line); }
    ;

relational
    : additive                       { $$ = $1; }
    | relational LT additive         { $$ = make_binary_node("<", $1, $3, @2.first_line); }
    | relational GT additive         { $$ = make_binary_node(">", $1, $3, @2.first_line); }
    | relational LE additive         { $$ = make_binary_node("<=", $1, $3, @2.first_line); }
    | relational GE additive         { $$ = make_binary_node(">=", $1, $3, @2.first_line); }
    ;

additive
    : multiplicative                 { $$ = $1; }
    | additive PLUS multiplicative   { $$ = make_binary_node("+", $1, $3, @2.first_line); }
    | additive MINUS multiplicative  { $$ = make_binary_node("-", $1, $3, @2.first_line); }
    ;

multiplicative
    : unary                          { $$ = $1; }
    | multiplicative STAR unary      { $$ = make_binary_node("*", $1, $3, @2.first_line); }
    | multiplicative SLASH unary     { $$ = make_binary_node("/", $1, $3, @2.first_line); }
    | multiplicative PERCENT unary   { $$ = make_binary_node("%", $1, $3, @2.first_line); }
    ;

unary
    : primary                        { $$ = $1; }
    | NOT unary                      { $$ = make_unary_node("!", $2, @1.first_line); }
    | MINUS unary                    { $$ = make_unary_node("-", $2, @1.first_line); }
    ;

primary
    : IDENTIFIER
      { $$ = ast_make_identifier($1, @1.first_line); free($1); }
    | INT_LITERAL
      { $$ = ast_make_literal($1, TYPE_INT, @1.first_line); free($1); }
    | FLOAT_LITERAL
      { $$ = ast_make_literal($1, TYPE_FLOAT, @1.first_line); free($1); }
    | TRUE
      { $$ = ast_make_literal("true", TYPE_BOOL, @1.first_line); }
    | FALSE
      { $$ = ast_make_literal("false", TYPE_BOOL, @1.first_line); }
    | LPAREN expression RPAREN
      { $$ = $2; }
    ;

%%

static ASTNode *make_binary_node(const char *operator_text, ASTNode *left,
                                 ASTNode *right, int line) {
    ASTNode *node = ast_create(NODE_BINARY, operator_text, line);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}

static ASTNode *make_unary_node(const char *operator_text, ASTNode *operand,
                                int line) {
    ASTNode *node = ast_create(NODE_UNARY, operator_text, line);
    ast_add_child(node, operand);
    return node;
}

static ASTNode *make_statement_list(int line) {
    return ast_create(NODE_BLOCK, "statement-list", line);
}

static ASTNode *adopt_as_program(ASTNode *list) {
    ASTNode *program = ast_create(NODE_PROGRAM, "program", 1);
    if (!list) return program;
    for (int i = 0; i < list->child_count; ++i) {
        ast_add_child(program, list->children[i]);
        list->children[i] = NULL;
    }
    list->child_count = 0;
    ast_free(list);
    return program;
}

void yyerror(const char *message) {
    fprintf(stderr, "[ERR_SYNTAX_UNEXPECTED_TOKEN] %s at line %d\n",
            message, yylineno);
    fb_syntax_errors++;
}
