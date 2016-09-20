/*
 * eval.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

int is_self_evaluating(ast_t *exp) {
    switch(exp->type){
        case STRINGAST:
        case NUMBERAST:
        case BOOLEANAST:
        case CHARACTERAST:
            return 1;
        default:
            return 0;
    }
}

int is_variable(ast_t *exp) {
    return IS(exp, VARIABLEAST);
}

ast_t *eval_binary(ast_t *exp, environment *env) {
    if (exp->type != BINARYAST) {
        ERRORF(exp->line, parse assignment error);
    }
    binary_ast_t *binary = (binary_ast_t*)exp;
    char *ops = ((string_ast_t *)binary->op)->value;
    if(strcmp(ops, "==") == 0) {
        return eval_equal(binary, env);
    }
    if(strcmp(ops, "!=") == 0) {
        return eval_notequal(binary, env);
    }
    if (strcmp(ops, "=") == 0) {
        return eval_assignment(binary, env);
    }
    if (strcmp(ops, ">") == 0) {
        return eval_greater(binary, env);
    }
    if (strcmp(ops, ">=") == 0) {
        return eval_ge(binary, env);
    }
    if (strcmp(ops, "<") == 0) {
        return eval_less(binary, env);
    }
    if (strcmp(ops, "<=") == 0) {
        return eval_le(binary, env);
    }
    if (strcmp(ops, "+") == 0) {
        return eval_add(binary, env);
    }
    if (strcmp(ops, "-") == 0) {
        return eval_sub(binary, env);
    }
    if (strcmp(ops, "*") == 0) {
        return eval_mul(binary, env);
    }
    if (strcmp(ops, "/") == 0) {
        return eval_div(binary, env);
    }
    ERRORF(exp->line, unknown binary operator);
}

ast_t *eval_variable(ast_t *exp, environment *env) {
    ast_t **r = lookup_variable(exp, env);
    if (!r) {
        ERRORF(exp->line, cannot find variable);
    }
    return *r;
}

// define a function
ast_t *eval_definition(ast_t *exp, environment *env) {
    if (exp->type != BINARYAST) {
        ERRORF(exp->line, parse assignment error);
    }
    binary_ast_t *binary = (binary_ast_t*)exp;
    ast_t *var = binary->lhs;
    ast_t *value = eval(binary->rhs, env);
    if (!IS(var, VARIABLEAST)) {
        ERRORF(var->line, incorrect assign)
    }
}

ast_t *eval_while(ast_t *exp, environment *env) {
    if (exp->type != WHILEAST) {
        ERRORF(exp->line, eval whileast error);
    }
    while_ast *wh = (while_ast*)exp;

    while(1) {
        ast_t *bool = eval(wh->condition, env);
        if (!IS(bool, BOOLEANAST)) {
            ERRORF(exp->line, the condition should be boolean);
        }
        boolean_ast_t *b = (boolean_ast_t *)bool;
        if (b->value == 0) {
            break;
        }
        eval_expressions(wh->body, env);
    }
    return NULL;
}

ast_t *eval_if(ast_t *exp, environment *env) {
    if_ast *ifast = (if_ast*) exp;
    ast_t *cond = eval(ifast->condition, env);
    if (!IS(cond, BOOLEANAST)) {
        ERRORF(cond->line, a boolean is required in condition);
    }
    if (((boolean_ast_t*)cond)->value == 1) {
        eval_expressions(ifast->then, env);
    } else {
        if (ifast->els) {
            eval_expressions(ifast->els, env);
        }
    }
    return NULL;
}

ast_t *eval_function_call(ast_t *exp, environment *env) {
    return apply(exp, env);
}

ast_t *eval_function(ast_t *exp, environment *env) {
    function_ast_t *exps = (function_ast_t *)exp;
    exps->env = env;
    return (ast_t*)exps;
}

ast_t *eval(ast_t *exp, environment *env) {
    if (is_self_evaluating(exp))
        return exp;
    switch (exp->type) {
        case VARIABLEAST:
            return eval_variable(exp, env);
        case FUNCTIONAST:
            return eval_function(exp, env);
        case IFAST:
            return eval_if(exp, env);
        case WHILEAST:
            return eval_while(exp, env);
        case CALLAST:
            return eval_function_call(exp, env);
        case BINARYAST:
            return eval_binary(exp, env);
        default:
            ERRORF(exp->line, invalid usage);
    }
    
}

ast_t *eval_expressions(expressions* exp, environment *env) {
    ast_t *res;
    hlist_node_t *iter;
    expression *e;
    hlist_for_each(exp, iter) {
        e = hlist_entry(iter, expression, node);
        res = eval(e->ast, env);
    }
    return res;
}
