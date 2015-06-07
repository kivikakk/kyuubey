#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

ast_t *ast_alloc(void) {
    ast_t *ast = malloc(sizeof(*ast));
    return ast;
}

void ast_append(ast_t *ast, void *x) {
}

void ast_free(ast_t *ast) {
    free(ast);
}

int yywrap(void) {
    return 1;
}

void yyerror(ast_t *ast, char const *s) {
    fprintf(stderr, "boop: %s\n", s);
}

int parser_test(void) {
    ast_t *ast = ast_alloc();

    begin_scan("PRINT \"HELLO\"");
    yyparse(ast);
    finish_scan();

    free(ast);

    return 0;
}

/* vim: set sw=4 et: */
