/*
 * lib.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

void init_expressions(expressions* exps) {
    //exps = (expressions*)malloc(sizeof(expressions));
    INIT_HLIST_HEAD(exps);
}

void add_after_expression(expression *last, expression *new) {
    hlist_add_after(&last->node, &new->node);
}


