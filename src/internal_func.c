/*
 * internal_func.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

static ast_t* check_format(expression *node, ast_type t, environment *env) {
    if (!node) {
        return NULL;
    }
    ast_t *exp = node->ast;
    if (exp->type == t) 
        return exp;
    ast_t *res = eval(exp, &env);
    if (!res || res->type != t) {
        return NULL;
    }
    return res;
}

ast_t *internal_printf(ast_t *exp, environment *env) {
    call_ast_t *call = (call_ast_t *)exp;
    expressions *args = call->args;

    hlist_node_t *iter;
    expression *node;
    iter = args->first;
    if (!iter) {
        ERRORF(current_file, exp->line, "no argument is passed to printf");
    }
    node = hlist_entry(iter, expression, node);
    if (node->ast->type != STRINGAST) {
        ERRORF(current_file, node->ast->line, "the FMT must be a string");
    }

    char *fmt = ((string_ast_t*)node->ast)->value;
    int line = node->ast->line;
    ast_t *tmp;
    while (*fmt) {
        if (*fmt == '%') {
            iter = iter->next;
            if (!iter) {
                ERRORF(current_file, line, "not enough argument");
            }
            node = hlist_entry(iter, expression, node);
            switch (*(++fmt)) {
                case 'c':
                    tmp = check_format(node, CHARACTERAST, env);
                    if (!tmp) {
                        ERRORF(current_file, line, "invalid percent c format");
                    } else {
                        printf("%c", ((character_ast_t*)tmp)->value);
                    }
                    break;
                case 's':
                    tmp = check_format(node, STRINGAST, env);
                    if (!tmp) {
                        ERRORF(current_file, line, "invalid percent s format");
                    } else {
                        printf("%s", ((string_ast_t*)tmp)->value);
                    }
                    break;
                case 'd':
                    tmp = check_format(node, NUMBERAST, env);
                    if (!tmp) {
                        ERRORF(current_file, line, "invalid percent d format");
                    } else {
                        printf("%ld", ((number_ast_t*)tmp)->value);
                    }
                    break;
                default:
                    ERRORF(current_file, line, "unknown format");
            }
            ++fmt;
        } else {
            printf("%c", *(fmt++));
        }
    }
    return NULL;
}

ast_t *internal_require(ast_t *exp, environment **env) {
    //printf("in require %d\n", exp->type);
    call_ast_t *call = (call_ast_t*)exp;
    expressions *e = call->args;
    hlist_node_t *iter = e->first;
    if (!iter) {
        ERRORF(current_file, call->line, "REQUIRE needs an argument");
    }
    if (iter->next) {
        ERRORF(current_file, call->line, "too many arguments are passed to REQUIRE");
    }
    expression *ee = hlist_entry(iter, expression, node);
    if (ee->ast->type != STRINGAST) {
        ERRORF(current_file, call->line, "REQUIRE needs a string argument, got %d", ee->ast->type);
    }

    lex_index = 0; 
    environment *new = extend_environment(*env);
    char *filename = ((string_ast_t*)ee->ast)->value;
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        ERRORF(current_file, exp->line, "failed to open required file %s", filename);
    }
    char *temp = current_file;
    current_file = filename;
    lexer(fp);
    fclose(fp);
    expressions *exps = parser();
    *env = new;
    eval_expressions(exps, env);
    current_file = temp;
    return NULL;
};

