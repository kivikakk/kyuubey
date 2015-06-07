#include <stdio.h>

#include "parser.h"

int yywrap(void) {
    return 1;
}

void yyerror(ast_t *ast, char const *s) {
    fprintf(stderr, "boop: %s\n", s);
}

int parser_test(void) {
    void *state = begin_scan("PRINT \"HELLO\"");
    delete_buffer(state);

    return 0;
}

/* vim: set sw=4 et: */
