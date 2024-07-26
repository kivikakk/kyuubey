%{
    #include "parser.h"
%}

%parse-param {ast_t *ast}
%define parse.error verbose

%union {
    ast_comment_t *comment;
    ast_token_t *token;
    ast_expr_t *expr;

    ast_stmt_t *stmt;
}

%token END_OF_FILE 0 "$end"

%token NL

%token <token> TOKEN
%token <expr> IMM
%token <comment> COMMENT

%type <stmt> line stmt
%type <expr> expr exprlist opt_exprlist

%left '+' '-'
%left '*' '/'

%left '(' ')'

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

exprlist:       expr                    { $$ = $1; }
              | expr ',' exprlist       { $$ = $1; $1->next = $3; $1->nexttype = ','; }
              | expr ';' exprlist       { $$ = $1; $1->next = $3; $1->nexttype = ';'; }
              | expr exprlist           { $$ = $1; $1->next = $2; $1->nexttype = ';'; }
;

opt_exprlist:   /* empty */             { $$ = 0; }
              | exprlist                { $$ = $1; }
;

expr:           IMM                     { $$ = $1; }
              | '(' expr ')'            { $$ = $2; }
              | expr '+' expr           { $$ = ast_binary_alloc('+', $1, $3); }
              | expr '-' expr           { $$ = ast_binary_alloc('-', $1, $3); }
              | expr '*' expr           { $$ = ast_binary_alloc('*', $1, $3); }
              | expr '/' expr           { $$ = ast_binary_alloc('/', $1, $3); }
;

/* vim: set sw=4 et: */
