%{
    #include "syntax.tab.h"
%}
digit    [0-9]
letter   [_a-zA-Z]
%%
{digit}+ { printf("Integer value %d\n", atoi(yytext)); return INT; }
{letter}+ { printf("String value %s\n", yytext); return WORD; }
%%