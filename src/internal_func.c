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
    if (exp->type == VARIABLEAST) {
        ast_t **val = lookup_variable(exp, env);
        if (!val) {
            return NULL;
        }
        if((*val)->type != t) {
            return NULL;
        }
        return *val;
    }
    return NULL;
}

ast_t *internal_printf(ast_t *exp, environment *env) {
    call_ast_t *call = (call_ast_t *)exp;
    expressions *args = call->args;

    hlist_node_t *iter;
    expression *node;
    iter = args->first;
    if (!iter) {
        ERRORF(exp->line, no argument is passed to printf);
    }
    node = hlist_entry(iter, expression, node);
    if (node->ast->type != STRINGAST) {
        ERRORF(node->ast->line, the FMT must be a string);
    }

    char *fmt = ((string_ast_t*)node->ast)->value;
    int line = node->ast->line;
    ast_t *tmp;
    while (*fmt) {
        if (*fmt == '%') {
            iter = iter->next;
            if (!iter) {
                ERRORF(line, not enough argument);
            }
            node = hlist_entry(iter, expression, node);
            switch (*(++fmt)) {
                case 'c':
                    tmp = check_format(node, CHARACTERAST, env);
                    if (!tmp) {
                        ERRORF(line, invalid percent c format);
                    } else {
                        printf("%c", ((character_ast_t*)tmp)->value);
                    }
                    break;
                case 's':
                    tmp = check_format(node, STRINGAST, env);
                    if (!tmp) {
                        ERRORF(line, invalid percent s format);
                    } else {
                        printf("%s", ((string_ast_t*)tmp)->value);
                    }
                    break;
                case 'd':
                    tmp = check_format(node, NUMBERAST, env);
                    if (!tmp) {
                        ERRORF(line, invalid percent d format);
                    } else {
                        printf("%ld", ((number_ast_t*)tmp)->value);
                    }
                    break;
                default:
                    ERRORF(line, unknown format);
            }
            ++fmt;
        } else {
            printf("%c", *(fmt++));
        }
    }
    return NULL;
}

