/*
 * parser.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

static int parse_index = 0;
static lex *curtok;

ast_t *parse_expression();

static int get_precedence(lex *current) {
    if (!current)
        return -2;
    TOKEN token = current->token;
    if (token == ASSIGN)
        return 5;
    if (token == LT || token == GT || token == EQUAL || token == LTE || token == GTE)
        return 10;
    if (token == ADD || token == MINUS)
        return 20;
    if (token == MUL || token == DIV)
        return 40;
    return -1;
}

static void get_next_token() {
    if (parse_index == lex_index) {
        curtok = NULL;
        return;
    }
    curtok = &lex_list[parse_index++];
}

static void unget_token() {
    if (parse_index > 0)
        curtok = &lex_list[--parse_index];
}

ast_t *parse_identifier() {
    lex *current = curtok; 
    //get_next_token(); // skip identifier
    lex *next = curtok;
    if (next->token == LPARAN) {
        // function call;
        call_ast_t *call;
        new_call_ast(call);
        get_next_token(); // consume LPARAN
        next = curtok;
        while (1) {
            if (next->token == RPARAN)
                break;
        }
        return (ast_t*)call;
    } else {
        // symbol reference;
        variable_ast_t *variable;
        new_variable_ast(variable, current->value);
        current->value = NULL;
        return (ast_t*)variable;
    }
}

ast_t *parse_number() {
    lex *current = curtok;
    long sum = 0;
    int length = strlen(current->value);
    for (int i = 0; i < length; i++) {
        sum = 10 * sum + current->value[i] - '0';
    }
    number_ast_t *number;
    new_number_ast(number, sum);
    return (ast_t*)number;
}

// parse (exp)
ast_t *parse_paran() {
    get_next_token(); // consume (
    ast_t *inside = parse_expression();
    if (curtok->token != RPARAN) {
        ERRORF(curtok->line, expected right parenthesis here);
    }
    //get_next_token(); // consume )
    return inside;
}

ast_t *parse_primary() {
    if (!curtok) {
        return NULL;
    }
    ast_t *res;
    switch(curtok->token) {
        case IDENTIFIER:
            res = parse_identifier();
            get_next_token(); // skip identifier;
            return res;
        case NUMBER:
            res = parse_number();
            get_next_token(); // skip number;
            return res;
        case LPARAN:
            res = parse_paran();
            get_next_token(); // skip right paran )
            return res;
        default:
            ERRORF(curtok->line, unexpected token);
    }
}

ast_t *parse_binary_ops(int precedence, ast_t *lhs) {
    while(1) {
        int cur_prec = get_precedence(curtok);
        if (cur_prec < precedence) {
            return lhs;
        }
        string_ast_t *ops;
        new_string_ast(ops, curtok->value);
        get_next_token(); // eat binop

        ast_t *rhs = parse_primary();
        if (!rhs) {
            return lhs;
        }

        //get_next_token();
        int next_prec = get_precedence(curtok);
        if (cur_prec < next_prec) {
            rhs = parse_binary_ops(cur_prec+1, rhs);
            if (!rhs) {
                // should parse error 2;
                ERRORF(-2, parse error 2);
                return lhs;
            }
        }

        binary_ast_t *n;
        new_binary_ast(n, (ast_t*)ops, lhs, rhs);
        lhs = (ast_t *)n;
    }
}

ast_t *parse_expression() {
    ast_t *lhs = parse_primary();
    if (!lhs) {
        return NULL;
    }
    ast_t *res = parse_binary_ops(0, lhs);
    return res;
}

void pretty_format(int depth) {
    return;
    for(int i = 0; i < depth; i++) {
        printf(" ");
    }
}

void print_ast(ast_t *t, int depth) {
    if (!t) {
        printf("ast is NULL;\n");
        return;
    }

    //PRINTF_ENUM(t->type);
    switch (t->type) {
        case NUMBERAST:
            pretty_format(depth);
            printf("%ld ", ((number_ast_t*)t)->value);
            break;
        case STRINGAST:
            pretty_format(depth);
            printf("%s ", ((string_ast_t*)t)->value);
            break;
        case BINARYAST:
            //binary_ast_t *temp = (binary_ast_t*)t;
            pretty_format(depth);
            printf("(");
            print_ast(((binary_ast_t*)t)->op, depth+1);
            print_ast(((binary_ast_t*)t)->lhs, depth+1);
            print_ast(((binary_ast_t*)t)->rhs, depth+1);
            pretty_format(depth);
            printf(") ");
            break;
        case VARIABLEAST:
            pretty_format(depth);
            printf("%s ", ((variable_ast_t*)t)->value);
            break;
        case CHARACTERAST:
            pretty_format(depth);
            printf("%c ", ((character_ast_t*)t)->value);
            break;
        default:
            printf("ast type is %d\n", t->type);
            ERRORF(-1, no such ast type);
    }
}

ast_t *parser() {
    get_next_token();
    ast_t *ast;
    while(1) {
        ast = parse_expression();
        if (!ast) 
            break;

        lex *current = curtok;
        // handle the end of expression
        if (current && current->token == SEMICOLON) {
            get_next_token();
        }
        print_ast(ast, 0);
        printf("\n");
    }
    return NULL;
}
