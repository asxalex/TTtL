/*
 * lexer.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

lex lex_list[MAX_STRING];
int lex_index = 0;
int line = 1;

int peek(FILE* fp) {
    int c = getc(fp);
    ungetc(c, fp);
    return c;
}

void consume_char(FILE* fp) {
    getc(fp);
}

void scan_word(FILE *input, int c) {
    lex_list[lex_index].value = malloc(MAX_WORD_SIZE);
    int i = 0;
    lex_list[lex_index].value[i++] = c;

    if (isdigit(c)) {
        while (isdigit(c = getc(input))) {
            lex_list[lex_index].value[i++] = c;
        }
        if (isalpha(c)) {
            ERRORF(line, identifier should not starts with digit);
        }
        ungetc(c, input);
        lex_list[lex_index].token = NUMBER;
    } else {
        while(isalnum(c = getc(input)) || (c == '_')) {
            lex_list[lex_index].value[i++] = c;
        }
        ungetc(c, input);
        lex_list[lex_index].token = IDENTIFIER;
    }
    lex_list[lex_index].value[i] = '\0';
    lex_list[lex_index++].line = line;
}

void lexer(FILE *input) {
    //int c = getc(input);
    int c = ' ';
    while (c != EOF) {
        c = getc(input);
        if (isspace(c)) {
            continue;
        }
        if (c == '\n') {
            printf("1\n");
            line++;
            continue;
        }
        if (c == EOF) {
            printf("2\n");
            break;
        }

        if (c == ';') {
            printf("3\n");
            ASSIGN_LEX_LIST(";", SEMICOLON, line);
            continue;
        }
        if (c == ':') {
            printf("4\n");
            ASSIGN_LEX_LIST(":", COLON, line);
            continue;
        }
        if (c == ',') {
            printf("5\n");
            ASSIGN_LEX_LIST(",", COMMA, line);
            continue;
        }
        if (c == '(') {
            printf("6\n");
            ASSIGN_LEX_LIST("(", LPARAN, line);
            continue;
        }
        if (c == ')') {
            printf("7\n");
            ASSIGN_LEX_LIST(")", RPARAN, line);
            continue;
        }
        if (c == '[') {
            printf("8\n");
            ASSIGN_LEX_LIST("[", LBRACK, line);
            continue;
        }
        if (c == ']') {
            printf("9\n");
            ASSIGN_LEX_LIST("]", RBRACK, line);
            continue;
        }
        if (c == '{') {
            printf("10\n");
            ASSIGN_LEX_LIST("{", LBRACE, line);
            continue;
        }
        if (c == '}') {
            printf("11\n");
            ASSIGN_LEX_LIST("}", RBRACE, line);
            continue;
        }
        if (c == '=') {
            printf("12\n");
            if (peek(input) == '=') {
                consume_char(input);
                ASSIGN_LEX_LIST("==", EQUAL, line);
            } else {
                ASSIGN_LEX_LIST("=", ASSIGN, line);
            }
            continue;
        }
        if (c == '+') {
            printf("13\n");
            ASSIGN_LEX_LIST("+", ADD, line);
            continue;
        }
        if (c == '-') {
            printf("14\n");
            ASSIGN_LEX_LIST("-", MINUS, line);
            continue;
        }
        if (c == '*') {
            printf("15\n");
            ASSIGN_LEX_LIST("*", MUL, line);
            continue;
        }
        if (c == '/') {
            printf("16\n");
            ASSIGN_LEX_LIST("/", DIV, line);
            continue;
        }
        if (c == '"') {
            printf("17\n");
            ASSIGN_LEX_LIST("\"", DOUBLEQUOTE, line);
            continue;
        }
        if (c == '\'') {
            printf("18\n");
            ASSIGN_LEX_LIST("'", SINGLEQUOTE, line);
            continue;
        }
        if (isalnum(c) || c == '_'){
            printf("19\n");
            scan_word(input, c);
            continue;
        }
        ERRORF(123, unknown character);
    }
}


void print_lexer_result() {
    for (int i = 0; i < lex_index; i++) {
        printf("===== %d =====\n", i);
        printf("%s\n", lex_list[i].value);
        printf("%d\n", lex_list[i].token);
        printf("%d\n", lex_list[i].line);
        printf("\n");
    }
}
