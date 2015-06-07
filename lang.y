%{
    #include "parser.h"
%}

%parse-param {ast_t *ast}
%error-verbose

%union {
    ast_comment_t *comment;
    ast_token_t *token;
    ast_expr_t *expr;

    ast_stmt_t *stmt;
}

%token END_OF_FILE 0 "$end"

%token NL

%token <token> TOKEN
%token <expr> EXPR
%token <comment> COMMENT

%type <stmt> line stmt
%type <expr> exprlist opt_exprlist

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

stmt:           TOKEN opt_exprlist      { $$ = ast_stmt_alloc(STMT_CALL); $$->call.target = $1; $$->call.args = $2; }
              | COMMENT                 { $$ = ast_stmt_alloc(STMT_COMMENT); $$->comment = $1; }
;

exprlist:       EXPR                    { $$ = $1; }
              | EXPR ',' exprlist       { $$ = $1; $1->next = $3; $1->nexttype = ','; }
              | EXPR ';' exprlist       { $$ = $1; $1->next = $3; $1->nexttype = ';'; }
              | EXPR exprlist           { $$ = $1; $1->next = $2; $1->nexttype = ';'; }
;

opt_exprlist:   /* empty */             { $$ = 0; }
              | exprlist                { $$ = $1; }
;

/* vim: set sw=4 et: */
