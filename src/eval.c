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
        ERRORF(current_file, exp->line, "parse binary error, got %d", exp->type);
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
    if (strcmp(ops, "||") == 0) {
        return eval_or(binary, env);
    }
    if (strcmp(ops, "&&") == 0) {
        return eval_and(binary, env);
    }
    ERRORF(current_file, exp->line, "unknown binary operator %s", ops);
    return NULL;
}

ast_t *eval_variable(ast_t *exp, environment *env) {
    ast_t **r = lookup_variable(exp, env);
    if (!r) {
        ERRORF(current_file, exp->line, "cannot find variable %s", ((variable_ast_t*)exp)->value);
    }

    return *r;
}

// define a function
ast_t *eval_definition(ast_t *exp, environment *env) {
    if (exp->type != FUNCTIONAST) {
        ERRORF(current_file, exp->line, "incorrect Function ast type");
    }
    function_ast_t *temp = (function_ast_t *)exp;
    variable_ast_t *var;

    new_variable_ast(var, temp->name, exp->line);

    ast_t **v = lookup_variable((ast_t*)var, env);
    if (!v) {
        define_variable((ast_t*)var, (ast_t*)temp, env);
    } else {
        ERRORF(current_file, exp->line, "redifinition of function %s", temp->name);
    }
    return NULL;
}

ast_t *eval_function_call(ast_t *exp, environment **env) {
    if(exp->type != CALLAST) {
        ERRORF(current_file, exp->line, "not a function call type");
    }
    call_ast_t *call = (call_ast_t*)exp;

    if (strcmp(call->name, "printf") == 0) {
        return internal_printf(exp, *env);
    }
    if (strcmp(call->name, "require") == 0) {
        ast_t *res = internal_require(exp, env);
        //print_env(*env);
        return res;
    }

    variable_ast_t *variable;
    new_variable_ast(variable, call->name, exp->line);


    ast_t **v = lookup_variable((ast_t *)variable, *env);
    if (!v) {
        ERRORF(current_file, exp->line, "function %s has not been defined", call->name);
    }

    function_ast_t *function = (function_ast_t*)*v;
    environment *new_frame = extend_environment(*env);
    
    hlist_node_t *call_arg_list = call->args->first;
    hlist_node_t *func_arg_list = function->params->first;

    expression *temp_call;
    expression *temp_func;


    while((call_arg_list != NULL) && (func_arg_list != NULL)) {
        temp_call = hlist_entry(call_arg_list, expression, node);
        temp_func = hlist_entry(func_arg_list, expression, node);

        ast_t *ast_func;
        ast_t *ast_call;
        ast_func = temp_func->ast;
        ast_call = eval(temp_call->ast, env);
        define_variable(ast_func, ast_call, new_frame);

        call_arg_list = call_arg_list->next;
        func_arg_list = func_arg_list->next;
    }
    if (call_arg_list != NULL || func_arg_list != NULL) {
        ERRORF(current_file, exp->line, "mismatch number of arguments");
    }
    ast_t *res = eval_expressions(function->body, new_frame);
    //print_ast(res, 0);
    return res;
}

ast_t *eval_while(ast_t *exp, environment *env) {
    ast_t *res = NULL;
    if (exp->type != WHILEAST) {
        ERRORF(current_file, exp->line, "eval while ast error");
    }
    while_ast_t *wh = (while_ast_t*)exp;

    while(1) {
        ast_t *bool = eval(wh->condition, &env);
        if (!IS(bool, BOOLEANAST)) {
            ERRORF(current_file, exp->line, "a boolean is required in condition");
        }
        boolean_ast_t *b = (boolean_ast_t *)bool;
        if (b->value == 0) {
            break;
        }
        res = eval_expressions(wh->body, env);
    }
    return res;
}

ast_t *eval_if(ast_t *exp, environment *env) {
    ast_t *res = NULL;
    if_ast_t *ifast = (if_ast_t*) exp;
    ast_t *cond = eval(ifast->condition, &env);
    if (!IS(cond, BOOLEANAST)) {
        ERRORF(current_file, cond->line, "a boolean is required in condition");
    }
    if (((boolean_ast_t*)cond)->value == 1) {
        res = eval_expressions(ifast->then, env);
    } else {
        if (ifast->els) {
            res = eval_expressions(ifast->els, env);
        }
    }
    return res;
}

/*
ast_t *eval_function_call(ast_t *exp, environment *env) {
    return apply(exp, env);
}
*/

ast_t *eval(ast_t *exp, environment **env) {
    if (is_self_evaluating(exp)) {
        return exp;
    }
    switch (exp->type) { 
        case VARIABLEAST:
            return eval_variable(exp, *env);
        case FUNCTIONAST:
            return eval_definition(exp, *env);
        case CALLAST:
            return eval_function_call(exp, env);
        case IFAST:
            return eval_if(exp, *env);
        case WHILEAST:
            return eval_while(exp, *env);
        case BINARYAST:
            return eval_binary(exp, *env);
        default:
            ERRORF(current_file, exp->line, "unable to eval ast %d", exp->type);
    }
    return NULL;
}

ast_t *eval_expressions(expressions* exp, environment *env) {
    ast_t *res = NULL;
    hlist_node_t *iter;
    expression *e;
    hlist_for_each(exp, iter) {
        e = hlist_entry(iter, expression, node);
        //print_ast(e->ast, 0);
        if (!e->ast) {
            continue;
        }
        res = eval(e->ast, &env);
    }
    //return NULL;
    return res;
}
