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

void pass_word(FILE *fp) {
    char c;
    while((c = getc(fp)) != EOF && c != '\n');
    if (c == '\n'){
        line++;
    }
}

void scan_word(FILE *input, int c, int indicator) {
    lex_list[lex_index].value = malloc(MAX_WORD_SIZE);
    int i = 0;
    lex_list[lex_index].value[i++] = c;

    if (indicator == 0 && isdigit(c)) {
        while (isdigit(c = getc(input))) {
            lex_list[lex_index].value[i++] = c;
        }
        if (isalpha(c)) {
            ERRORF(line, identifier should not starts with digit);
        }
        ungetc(c, input);
        lex_list[lex_index].token = NUMBER;
    } else {
        if (indicator == 1) {
            while((c = getc(input)) != EOF && c != '"') {
                if (c == '\n')
                    line++;
                lex_list[lex_index].value[i++] = c;
            }
            if (c == EOF) {
                ERRORF(line, unmatched double quote);
            }
        } else if (indicator == 0) {
            while(isalnum(c = getc(input)) || (c == '_')) {
                lex_list[lex_index].value[i++] = c;
            }
        } else if (indicator == 2) {
            if ((c = getc(input)) != '\'') {
                //printf("c = %d\n", c);
                ERRORF(line, unmatched single quote);
            }
        }
        ungetc(c, input);

        char *value = lex_list[lex_index].value;
        if(strcmp(value, "if") == 0) {
            lex_list[lex_index].token = IF;
        } else if (strcmp(value, "else") == 0) {
            lex_list[lex_index].token = ELSE;
        } else if (strcmp(value, "while") == 0) {
            lex_list[lex_index].token = WHILE;
        } else if (strcmp(value, "define") == 0) {
            lex_list[lex_index].token = DEFINE;
        } else if (strcmp(value, "return") == 0) {
            lex_list[lex_index].token = RETURN;
        } else {
            lex_list[lex_index].token = IDENTIFIER;
        }
    }
    lex_list[lex_index].value[i] = '\0';
    lex_list[lex_index++].line = line;
}

void lexer(FILE *input) {
    //int c = getc(input);
    int c = ' ';

    /* if pair == 0, there's no pair;
     * if pair == 1, this is a string pair;
     * if pair == 2, this is a character pair;
     */
    int pair = 0;
    while (c != EOF) {
        c = getc(input);
        if (c == '\n') {
            printf("1\n");
            line++;
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
        if (c == '>') {
            if(peek(input) == '=') {
                consume_char(input);
                ASSIGN_LEX_LIST(">=", GTE, line);
            } else {
                ASSIGN_LEX_LIST(">", GT, line);
            }
            continue;
        }
        if (c == '<') {
            if (peek(input) == '=') {
                consume_char(input);
                ASSIGN_LEX_LIST("<=", LTE, line);
            } else {
                ASSIGN_LEX_LIST("<", LT, line);
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
            if (pair == 1 || pair == 0)
                pair = 1 - pair;
            ASSIGN_LEX_LIST("\"", DOUBLEQUOTE, line);
            continue;
        }
        if (c == '\'') {
            printf("18\n");
            if (pair == 2 || pair == 0)
                pair = 2 - pair;
            ASSIGN_LEX_LIST("'", SINGLEQUOTE, line);
            continue;
        }
        if (pair != 0) {
            printf("19\n");
            scan_word(input, c, pair);
            continue;
        }
        if (c == '#') {
            pass_word(input);
            continue;
        }
        if (isalnum(c) || c == '_'){
            printf("20\n");
            scan_word(input, c, 0);
            continue;
        }
        if (isspace(c)) {
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
