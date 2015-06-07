#ifndef PARSER_H
#define PARSER_H

/* ast_expr_t */

typedef enum {
    EXPR_STRING,
    EXPR_BINARY,
    EXPR_INTEGER,
} ast_expr_type_t;

typedef struct ast_expr {
    ast_expr_type_t type;

    union {
        char *string;
        struct {
            char op;
            struct ast_expr *a;
            struct ast_expr *b;
        } binary;
        int integer;
    };

    struct ast_expr *next;
    char nexttype;
} ast_expr_t;

ast_expr_t *ast_string_alloc(char const *value);
ast_expr_t *ast_binary_alloc(char op, ast_expr_t *a, ast_expr_t *b);
ast_expr_t *ast_integer_alloc(int i);
void ast_expr_pp(ast_expr_t *expr);
void ast_expr_free(ast_expr_t *expr);
void ast_expr_free_list(ast_expr_t *expr);

/* ast_comment_t */

typedef struct {
    char *value;
    int is_rem;
} ast_comment_t;

ast_comment_t *ast_comment_alloc(char const *value, int is_rem);
void ast_comment_free(ast_comment_t *comment);

/* ast_token_t */

typedef struct {
    char *value;
} ast_token_t;

ast_token_t *ast_token_alloc(char const *value);
void ast_token_free(ast_token_t *token);

/* ast_stmt_t */

typedef enum {
    STMT_CALL,
    STMT_COMMENT,
} ast_stmt_type_t;

typedef struct ast_stmt {
    ast_stmt_type_t type;

    union {
        struct {
            ast_token_t *target;
            ast_expr_t *args;
        } call;
        ast_comment_t *comment;
    };

    struct ast_stmt *next;
} ast_stmt_t;

ast_stmt_t *ast_stmt_alloc(ast_stmt_type_t type);
void ast_stmt_pp(ast_stmt_t *stmt);
void ast_stmt_free(ast_stmt_t *stmt);
void ast_stmt_free_list(ast_stmt_t *stmt);

/* ast_t */

typedef struct {
    ast_stmt_t *stmts;
} ast_t;

ast_t *ast_alloc(void);
void ast_append_stmt(ast_t *ast, ast_stmt_t *stmt);
void ast_pp(ast_t *ast);
void ast_free(ast_t *ast);

/* flex/bison */

int yylex(void);
int yyparse(ast_t *ast);
int yywrap(void);
void yyerror(ast_t *ast, char const *s);

int parser_test(void);

/* from lang.l */
void begin_scan(char const *s);
void finish_scan();

#endif

/* vim: set sw=4 et: */
