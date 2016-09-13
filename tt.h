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
    RPARAN, // )
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // {
    RBRACE, // }
    EQUAL, // ==
    ASSIGN, // =
    ADD, // +
    MINUS, // -
    MUL, // *
    DIV, // /
    DOUBLEQUOTE, // "
    SINGLEQUOTE, // '
    NUMBER, // 0-9
    IDENTIFIER, 
};

struct __lex {
    char *value;
    TOKEN token;
    int line;
};


#define MAX_WORD_SIZE 40
#define MAX_STRING 10000
#define ASSIGN_LEX_LIST(s, t, l) \
    lex_list[lex_index].value = s; \
    lex_list[lex_index].token = t; \
    lex_list[lex_index++].line = l

#define ERRORF(row, str) \
    fprintf(stderr, "in line %d error : "#str"\n", row); \
    exit(-1);

extern lex lex_list[MAX_STRING];
extern int lex_index;

// lexer.c
void lexer(FILE *);
void print_lexer_result();

#endif /* !TT_H */
