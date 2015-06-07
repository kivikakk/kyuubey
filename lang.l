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

/* vim: set sw=4 et: */
