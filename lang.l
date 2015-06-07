%{
    #include "parser.h"
    #include "lang.tab.h"
%}

%option yywrap stack

%x abc_x
%s abc_s

%{
    /* ... */
%}

%%

REM[^\n]+       { yylval.comment = ast_comment_alloc(yytext + 3, 1); return COMMENT; }
'[^\n]+       { yylval.comment = ast_comment_alloc(yytext + 1, 0); return COMMENT; }
[a-zA-Z]+       { yylval.token = ast_token_alloc(yytext); return TOKEN; }
[\t ]+          { }
.               { return yytext[0]; }
\n              { return NL; }

%%

void begin_scan(char const *s) {
    yy_switch_to_buffer(yy_scan_string(s));
}

void finish_scan() {
    yy_delete_buffer(YY_CURRENT_BUFFER);
}

/* vim: set sw=4 et: */
