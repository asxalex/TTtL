/*
 * environment.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

environment *init_env() {
    environment *env = (environment*)malloc(sizeof(environment));
    INIT_HLIST_HEAD(&env->hlist);
    env->prev = NULL;
    return env;
}

void print_env(environment *e) {
    environment *env = e;
    int i = 0;
    while (env != NULL) {
        printf("========== frame %d ==========\n", i++);
        hlist_t *hlist = &env->hlist;
        hlist_node_t *iter;
        env_node *node;
        for (iter = hlist->first; iter != NULL; iter = iter->next) {

        }
        hlist_for_each(hlist, iter) {
            node = hlist_entry(iter, env_node, node);
            print_ast(node->variable, 0);
            print_ast(node->value, 0);
            printf("\n");
        }
        env = env->prev;
    }
}

void define_variable(ast_t* var, ast_t *val, environment *env) {
    if (var->type != VARIABLEAST){
        ERRORF(-1, variable is required);
    }
    env_node *node = (env_node*)malloc(sizeof(env_node));
    INIT_HLIST_NODE(&node->node);
    node->variable = var;
    node->value = val;
    hlist_add_head(&node->node, &env->hlist);
}

environment *extend_environment(environment *env) {
    environment *new_frame = (environment*)malloc(sizeof(environment));
    INIT_HLIST_HEAD(&new_frame->hlist);
    new_frame->prev = env;
    return new_frame;
}

ast_t **lookup_variable_in_current_frame(ast_t *var, environment *env) {
    environment *temp_env = env;
    char *dest = ((variable_ast_t*)var)->value;
    env_node *node;
    hlist_t *hlist = &temp_env->hlist;
    hlist_node_t *iter;
    hlist_for_each(hlist, iter) {
        node = hlist_entry(iter, env_node, node);
        char *v = ((string_ast_t*)(node->variable))->value;
        if (strcmp(dest, v) == 0) {
            return &node->value;
        }
    }
    return NULL;
}

ast_t **lookup_variable(ast_t *var, environment* env) {
    environment *temp_env = env;
    char *dest = ((variable_ast_t*)var)->value;
    env_node *node;
    while (temp_env != NULL) {
        hlist_t *hlist = &temp_env->hlist;
        hlist_node_t *iter;
        hlist_for_each(hlist, iter) {
            node = hlist_entry(iter, env_node, node);
            char *v = ((string_ast_t*)(node->variable))->value;
            if (strcmp(dest, v) == 0) {
                return &node->value;
            }
        }

        temp_env = temp_env->prev;
    }
    return NULL;
}

int set_variable_value(ast_t *var, ast_t *val, environment *env) {
    ast_t **target = lookup_variable(var, env);
    if (!target) {
        return -1;
    }
    ast_t **temp = target;
    *target = val;
    free(*temp);
    return 0;
}
