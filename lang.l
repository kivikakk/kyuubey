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

a       { return A; }

%%

YY_BUFFER_STATE current_state = NULL;

void *begin_scan(char const *s) {
    return yy_scan_string(s);
}

void delete_buffer(void *buffer) {
    yy_delete_buffer(buffer);
}

/* vim: set sw=4 et: */
