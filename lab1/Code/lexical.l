%option yylineno
%{
    #include "syntax.tab.h"
    
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}

digit    [0-9]
letter   [_a-zA-Z]

%%
{digit}+ { printf("Integer value %d\n", atoi(yytext)); yylval = atoi(yytext); return INT; }
{letter}+ { printf("String value %s\n", yytext); yylval = yyleng; return WORD; }
\n { yycolumn = 1; }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%