%{
    #include "parser.h"
%}

%parse-param {ast_t *ast}
%error-verbose

%union {
    ast_comment_t *comment;
    ast_token_t *token;

    ast_stmt_t *stmt;
}

%token END_OF_FILE 0 "$end"

%token NL

%token <token> TOKEN
%token <comment> COMMENT

%type <stmt> line stmt

%%

input:          /* empty */
              | input line { if ($2) { ast_append_stmt(ast, $2); } }
;

line_separator: NL
              | ':'
;

line:           line_separator          { $$ = 0; }
              | stmt line_separator     { $$ = $1; }
              | stmt END_OF_FILE        { $$ = $1; }
;

stmt:           TOKEN                   { $$ = ast_stmt_alloc(STMT_CALL); $$->call.target = $1; }
              | COMMENT                 { $$ = ast_stmt_alloc(STMT_COMMENT); $$->comment = $1; }
;

/* vim: set sw=4 et: */
