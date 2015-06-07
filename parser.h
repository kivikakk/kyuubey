#ifndef PARSER_H
#define PARSER_H

typedef struct {

} ast_t;

int yywrap(void);
void yyerror(ast_t *ast, char const *s);

#endif

/* vim: set sw=4 et: */
