%{
    #include "lex.yy.c"
%}

%token INT
%token WORD

%%
Program : INT WORD
%%