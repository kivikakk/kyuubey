%{
    #include "lang.tab.h"
%}

%option yywrap stack

%x abc_x
%s abc_s

%{
    /* ... */
%}

%%

[a-zA-Z]        { return TOKEN; }
\n              { return NL; }

%%

void begin_scan(char const *s) {
    yy_switch_to_buffer(yy_scan_string(s));
}

void finish_scan() {
    yy_delete_buffer(YY_CURRENT_BUFFER);
}

/* vim: set sw=4 et: */
