/*
 * test_lexer.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: ./tt program_file.tt\n");
        exit(-1);
    }
    FILE *fp = fopen(argv[1], "r");
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

