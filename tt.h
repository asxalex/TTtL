/*
 * tt.h
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TT_H
#define TT_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct __lex lex;
typedef enum __token TOKEN;

enum __token {
    SEMICOLON = 1, // ;
    COLON, // :
    COMMA, // ,
    LPARAN, // (
    RPARAN, // ) 5
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // {
    RBRACE, // }
    EQUAL, // == 10
    ASSIGN, // =
    GT, // >
    LT, // <
    GTE, // >=
    LTE, // <= 15
    ADD, // +
    MINUS, // -
    MUL, // *
    DIV, // / 
    DOUBLEQUOTE, // " 20
    SINGLEQUOTE, // '
    NUMBER, // 0-9
    IDENTIFIER,
    IF, // if 
    ELSE, // else 25
    WHILE, // while
    DEFINE, // define
    RETURN, // return
};

struct __lex {
    char *value;
    TOKEN token;
    int line;
};

typedef enum __ast_type ast_type;

enum __ast_type {
    NUMBERAST,
    VARIABLEAST,
    BINARYAST,
    CHARACTERAST,
    STRINGAST,
    IDENTIFIERAST,
    CALLAST,
    FUNCTIONAST,
};

#define ast_object ast_type type

typedef struct env_s {

} env_t;

typedef struct ast_s {
    ast_object;
} ast_t;

typedef struct _number_ast_s {
    ast_object;
    long value;
} number_ast_t;

#define new_number_ast(x, v) \
    x = malloc(sizeof(number_ast_t)); \
    x->type = NUMBERAST; \
    x->value = v

typedef struct _variable_ast_s {
    ast_object;
    char *value;
} variable_ast_t;

#define new_variable_ast(x, v) \
    x = (variable_ast_t*)malloc(sizeof(variable_ast_t)); \
    x->type = VARIABLEAST; \
    x->value = v

typedef struct _binary_ast_s {
    ast_object;
    ast_t *op;
    ast_t *lhs;
    ast_t *rhs;
} binary_ast_t;

#define new_binary_ast(x, o, l, r) \
    x = (binary_ast_t*)malloc(sizeof(binary_ast_t)); \
    x->type = BINARYAST; \
    x->op = o; \
    x->lhs = l; \
    x->rhs = r


typedef struct _character_ast_s {
    ast_object;
    char value;
} character_ast_t;

typedef struct _string_ast_s {
    ast_object;
    char *value;
} string_ast_t;

#define new_string_ast(x, v) \
    x = (string_ast_t*)malloc(sizeof(string_ast_t)); \
    x->type = STRINGAST; \
    x->value = v

typedef struct _identifier_ast_s {
    ast_object;
    char *value;
} identifier_ast_t;

typedef struct _call_ast_s {
    ast_object;
    ast_t **args;
} call_ast_t;

#define new_call_ast(x) x = (call_ast_t *)malloc(sizeof(call_ast_t))

struct _function_ast_s {
    ast_object;
    ast_t **params;
    ast_t **body;
    env_t **env;
} function_ast_t;

#define MAX_WORD_SIZE 40
#define MAX_STRING 10000
#define ASSIGN_LEX_LIST(s, t, l) \
    lex_list[lex_index].value = s; \
    lex_list[lex_index].token = t; \
    lex_list[lex_index++].line = l

#define ERRORF(row, str) \
    fprintf(stderr, "in line %d error : "#str"\n", row); \
    exit(-1);

#define PRINTF_ENUM(e) \
    printf("[print] ENUM %d"#e"\n", e)

extern lex lex_list[MAX_STRING];
extern int lex_index;

// lexer.c
void lexer(FILE *);
void print_lexer_result();

// parser.c
ast_t *parser();

#endif /* !TT_H */
