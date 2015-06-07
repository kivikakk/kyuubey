#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/* ast_comment_t */

ast_comment_t *ast_comment_alloc(char const *value, int is_rem) {
    ast_comment_t *comment = malloc(sizeof(*comment));
    memset(comment, 0, sizeof(*comment));
    comment->value = strdup(value);
    comment->is_rem = is_rem;
    return comment;
}

void ast_comment_free(ast_comment_t *comment) {
    free(comment->value);
    free(comment);
}

/* ast_token_t */

ast_token_t *ast_token_alloc(char const *value) {
    ast_token_t *token = malloc(sizeof(*token));
    memset(token, 0, sizeof(*token));
    token->value = strdup(value);
    return token;
}

void ast_token_free(ast_token_t *token) {
    free(token->value);
    free(token);
}

/* ast_stmt_t */

ast_stmt_t *ast_stmt_alloc(ast_stmt_type_t type) {
    ast_stmt_t *stmt = malloc(sizeof(*stmt));
    memset(stmt, 0, sizeof(*stmt));
    stmt->type = type;
    return stmt;
}

void ast_stmt_pp(ast_stmt_t *stmt) {
    switch (stmt->type) {
    case STMT_CALL:
        printf("%s\n", stmt->call.target->value);
        break;

    case STMT_COMMENT:
        printf("%s%s\n", stmt->comment->is_rem ? "REM" : "'", stmt->comment->value);
        break;

    default:
        fprintf(stderr, "UNKNOWN STMT TYPE %d\n", stmt->type);
        break;
    }
}

void ast_stmt_free(ast_stmt_t *stmt) {
    free(stmt);
}

/* ast_t */

ast_t *ast_alloc(void) {
    ast_t *ast = malloc(sizeof(*ast));
    memset(ast, 0, sizeof(*ast));
    return ast;
}

void ast_append_stmt(ast_t *ast, ast_stmt_t *stmt) {
    ast_stmt_t **writer = &ast->stmts;
    while (*writer) {
        writer = &(*writer)->next;
    }
    *writer = stmt;
}

void ast_pp(ast_t *ast) {
    ast_stmt_t *stmt = ast->stmts;
    while (stmt) {
        ast_stmt_pp(stmt);
        stmt = stmt->next;
    }
}

void ast_free(ast_t *ast) {
    free(ast);
}

/* flex/bison */

int yywrap(void) {
    return 1;
}

void yyerror(ast_t *ast, char const *s) {
    fprintf(stderr, "parse error: %s\n", s);
}

int parser_test(void) {
    ast_t *ast = ast_alloc();

    begin_scan(
        "PRINT\n"
        "GOTO\n"
        "\n"
        "REM Okay, sure thing.\n"
        "'I guess.\n");
    yyparse(ast);
    finish_scan();

    ast_pp(ast);

    free(ast);

    return 0;
}

/* vim: set sw=4 et: */
