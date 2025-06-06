%option yylineno
%{
    #include "ast.h"
    #include "util.h"
    #include "err.h"
    #include "syntax.tab.h"

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

    extern int parse_error;
    int lexerr_thisline = 0;
%}

digit    [0-9]
letter   [_a-zA-Z]
relop    (!=)|(==)|(<)|(>)|(<=)|(>=)
type     (int)|(float)
newline  (\r|\n|\r\n)
float    ({digit}*\.{digit}+|{digit}+\.)

%%
"//".* {}
(0|([1-9]{digit}*)) {
    yylval = ast_new_node(AST_NODE_INT);
    yylval->attr.int_value = atoi(yytext);
    return INT;
}
{float} {
    yylval = ast_new_node(AST_NODE_FLOAT);
    yylval->attr.float_value = atof(yytext);
    return FLOAT;
}
{float}[eE][+-]?{digit}+ {
    yylval = ast_new_node(AST_NODE_FLOAT);
    yylval->attr.float_value = atof(yytext);
    return FLOAT;
}

{type} {
    yylval = ast_new_node(AST_NODE_TYPE);
    yylval->attr.typename_value = str_new_copy(yytext);
    return TYPE;
}
struct {
    yylval = ast_new_node(AST_NODE_STRUCT);
    return STRUCT;
}
return {
    yylval = ast_new_node(AST_NODE_RETURN);
    return RETURN;
}
if {
    yylval = ast_new_node(AST_NODE_IF);
    return IF;
}
else {
    yylval = ast_new_node(AST_NODE_ELSE);
    return ELSE;
}
while {
    yylval = ast_new_node(AST_NODE_WHILE);
    return WHILE;
}

{letter}({letter}|{digit})* {
    yylval = ast_new_node(AST_NODE_ID);
    yylval->attr.identifier_value = str_new_copy(yytext);
    return ID;
}

; {
    yylval = ast_new_node(AST_NODE_SEMI);
    return SEMI;
}
, {
    yylval = ast_new_node(AST_NODE_COMMA);
    return COMMA;
}
= {
    yylval = ast_new_node(AST_NODE_ASSIGNOP);
    return ASSIGNOP;
}
{relop} {
    yylval = ast_new_node(AST_NODE_RELOP);
    yylval->attr.relop_name = str_new_copy(yytext);
    return RELOP;
}
\+ {
    yylval = ast_new_node(AST_NODE_PLUS);
    return PLUS;
}
- {
    yylval = ast_new_node(AST_NODE_MINUS);
    return MINUS;
}
\* {
    yylval = ast_new_node(AST_NODE_STAR);
    return STAR;
}
\/ {
    yylval = ast_new_node(AST_NODE_DIV);
    return DIV;
}
&& {
    yylval = ast_new_node(AST_NODE_AND);
    return AND;
}
\|\| {
    yylval = ast_new_node(AST_NODE_OR);
    return OR;
}
\. {
    yylval = ast_new_node(AST_NODE_DOT);
    return DOT;
}
\! {
    yylval = ast_new_node(AST_NODE_NOT);
    return NOT;
}
\( {
    yylval = ast_new_node(AST_NODE_LP);
    return LP;
}
\) {
    yylval = ast_new_node(AST_NODE_RP);
    return RP;
}
\[ {
    yylval = ast_new_node(AST_NODE_LB);
    return LB;
}
\] {
    yylval = ast_new_node(AST_NODE_RB);
    return RB;
}
\{ {
    yylval = ast_new_node(AST_NODE_LC);
    return LC;
}
\} {
    yylval = ast_new_node(AST_NODE_RC);
    return RC;
}

{newline} { lexerr_thisline = 0; yycolumn = 1; }
\t { /* do nothing */ }
" " { /* do nothing */ }

. {
    lexerr_thisline = 1;
    parse_error = 1; // Error type A
    log_mystery_char_error(yytext[0], yylineno);
}
%%