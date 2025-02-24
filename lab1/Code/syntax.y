%locations
%{
    #include "lex.yy.c"
    int yyerror(const char *s);
%}

%token INT
%token WORD

%%
Program : INT WORD { printf("haha: %d, hehe %d\n", $1, $2); }
%%