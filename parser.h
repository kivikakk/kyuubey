#ifndef PARSER_H
#define PARSER_H

typedef struct {

} ast_t;

int yylex(void);
int yyparse(ast_t *ast);

ast_t *ast_alloc(void);
void ast_append(ast_t *ast, void *x);

int yywrap(void);
void yyerror(ast_t *ast, char const *s);
int parser_test(void);

/* from lang.l */
void begin_scan(char const *s);
void finish_scan();

#endif

/* vim: set sw=4 et: */
