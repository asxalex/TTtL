/*
 * binary_eval.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

ast_t *eval_equal(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value == ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_notequal(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value != ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_assignment(binary_ast_t *binary, environment *env) {
    ast_t *var = binary->lhs;
    if (!is_variable(var)) {
        ERRORF(binary->line, 被赋值变量有误);
    }

    ast_t *val = eval(binary->rhs, env);
    ast_t **v = lookup_variable_in_current_frame(var, env); 
    if (!v) {
        define_variable(var, val, env);
    } else {
        set_variable_value(var, val, env);
    }
    return val;
}

ast_t *eval_greater(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value > ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_ge(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value >= ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_less(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value < ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_le(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    boolean_ast_t *b;
    if (((number_ast_t*)left)->value <= ((number_ast_t*)right)->value) {
        new_boolean_ast(b, 1, binary->line);
    } else {
        new_boolean_ast(b, 0, binary->line);
    }
    return (ast_t *)b;
}

ast_t *eval_add(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    number_ast_t *b;
    long v = ((number_ast_t*)left)->value + ((number_ast_t*)right)->value;
    new_number_ast(b, v, binary->line);
    return (ast_t *)b;
}

ast_t *eval_sub(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    number_ast_t *b;
    long v = ((number_ast_t*)left)->value - ((number_ast_t*)right)->value;
    new_number_ast(b, v, binary->line);
    return (ast_t *)b;
}

ast_t *eval_mul(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    number_ast_t *b;
    long v = ((number_ast_t*)left)->value * ((number_ast_t*)right)->value;
    new_number_ast(b, v, binary->line);
    return (ast_t *)b;
}

ast_t *eval_div(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    ast_t *right = eval(binary->rhs, env);
    if (left->type != NUMBERAST || right->type != NUMBERAST) {
        ERRORF(binary->line, 比较操作只能用于数字);
    }
    number_ast_t *b;
    if (((number_ast_t*)right)->value == 0) {
        ERRORF(right->line, 除数不能为0);
    }
    long v = ((number_ast_t*)left)->value / ((number_ast_t*)right)->value;
    new_number_ast(b, v, binary->line);
    return (ast_t *)b;
}

ast_t *eval_or(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    if (left->type != BOOLEANAST) {
        ERRORF(left->line, needs boolean here);
    }
    boolean_ast_t *res;
    if (((boolean_ast_t*)left)->value == 1) {
        new_boolean_ast(res, 1, left->line);
        return (ast_t*)res;
    }
    
    ast_t *right = eval(binary->rhs, env);
    if (right->type != BOOLEANAST) {
        ERRORF(right->line, needs boolean here);
    }

    if (((boolean_ast_t*)right)->value == 1) {
        new_boolean_ast(res, 1, left->line);
    } else {
        new_boolean_ast(res, 0, left->line);
    }
    return (ast_t*)res;
}

ast_t *eval_and(binary_ast_t *binary, environment *env) {
    ast_t *left = eval(binary->lhs, env);
    if (left->type != BOOLEANAST) {
        ERRORF(left->line, needs boolean here);
    }
    boolean_ast_t *res;
    if (((boolean_ast_t*)left)->value == 0) {
        new_boolean_ast(res, 0, left->line);
        return (ast_t*)res;
    }
    
    ast_t *right = eval(binary->rhs, env);
    if (right->type != BOOLEANAST) {
        ERRORF(right->line, needs boolean here);
    }

    if (((boolean_ast_t*)right)->value == 1) {
        new_boolean_ast(res, 1, left->line);
    } else {
        new_boolean_ast(res, 0, left->line);
    }
    return (ast_t*)res;
}
