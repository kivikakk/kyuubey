#ifndef PARSER_H
#define PARSER_H

typedef struct {

} ast_t;

int yywrap(void);
void yyerror(ast_t *ast, char const *s);
int parser_test(void);

/* from lang.l */
void *begin_scan(char const *s);
void delete_buffer(void *buffer);

#endif

/* vim: set sw=4 et: */
