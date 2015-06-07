%{
    #include "parser.h"
%}

%parse-param {ast_t *ast}
%error-verbose

%token TOKEN NL
%token END_OF_FILE 0 "$end"

%%

input:          /* empty */
              | input line { if ($2) { ast_append(ast, $2); } }
;

line_separator: NL
              | ':'
;

line:           line_separator          { $$ = 0; }
              | stmt line_separator     { $$ = $1; }
              | stmt END_OF_FILE        { $$ = $1; }
;

stmt: ;

/* vim: set sw=4 et: */
