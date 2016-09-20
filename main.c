/*
 * test_lexer.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

int main() {
    FILE *fp = fopen("a.tt", "r");
    lexer(fp);
    //print_lexer_result();
    //printf("=================================\n");
    fclose(fp);
    expressions *exps = parser();
    environment *env = init_env();
    eval_expressions(exps, env);
    //print_env(env);
    return 0;
}

