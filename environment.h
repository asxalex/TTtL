/*
 * environment.h
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "hlist.h"
#include "tt.h"

typedef struct environment_s environment;
typedef struct env_node_s env_node;

struct environment_s {
    hlist_t hlist;
    struct environment_s *prev;
};

struct env_node_s {
    ast_t *variable;
    ast_t *value;
    hlist_node_t node;
};

environment *init_env();
void print_env(environment*);
void define_variable(ast_t *, ast_t *, environment *);
environment *extend_environment(environment*);
ast_t *lookup_variable(ast_t *, environment *);
int set_variable_value(ast_t *, ast_t *, environment*);
