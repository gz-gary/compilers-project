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
relop    (!=)|(==)|(<)|(>)|(<=)|(>=)
type     (int)|(float)
newline  (\r|\n|\r\n)

%%
(0|([1-9]{digit}*)) { 
    yylval.type_int = atoi(yytext);
    printf("got int %s\n", yytext);
    return INT; 
}
({digit}*\.{digit}+|{digit}+\.) { 
    yylval.type_float = atof(yytext);
    printf("got float %s\n", yytext); 
    return FLOAT; 
}

{type} { return TYPE; }
struct { return STRUCT; }
return { return RETURN; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }

{letter}({letter}|{digit})* { return ID; }

; { return SEMI; }
, { return COMMA; }
= { return ASSIGNOP; }
{relop} { return RELOP; }
\+ { return PLUS; }
- { return MINUS; }
\* { return STAR; }
\/ { return DIV; }
&& { return AND; }
\|\| { return OR; }
\. { return DOT; }
\! { return NOT; }
\( { return LP; }
\) { return RP; }
\[ { return LB; }
\] { return RB; }
\{ { return LC; }
\} { return RC; }

{newline} { yycolumn = 1; }
\t { /* do nothing */ }
" " { /* do nothing */ }
. { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%