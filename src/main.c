/*
 * test_lexer.c
 * Copyright (C) 2016 alex <alex@alex>
 *
 * Distributed under terms of the MIT license.
 */

#include "tt.h"

void interactive_mode() {
    // TODO
}

void file_mode(const char *filename) {
    FILE *fp = fopen(filename, "r");
    lexer(fp);
    //print_lexer_result();
    //printf("=================================\n");
    fclose(fp);
    expressions *exps = parser();
    environment *env = init_env();
    eval_expressions(exps, env);
    //print_env(env);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        interactive_mode();
        fprintf(stderr, "usage: ./tt program_file.tt\n");
        exit(-1);
    } else {
        file_mode(argv[1]);
    }
    return 0;
}

