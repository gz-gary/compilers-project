%option noyywrap
%option yylineno
%option prefix="IR_yy"

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntax.tab.h"
#include "IR_parse.h"

#define IR_LEXICAL_ERROR(fmt, ...) \
        fprintf(stderr, "IR lexical error: " fmt "\n", ##__VA_ARGS__)

%}

%%

":"                     { return IR_TOKEN_COLON; }
"FUNCTION"              { return IR_TOKEN_FUNCTION; }
"LABEL"                 { return IR_TOKEN_LABEL; }
"#"                     { return IR_TOKEN_SHARP; }
-?[0-9]+                {
                            //TODO: 解决一串强劲的读取数据
                            // char *endptr;
                            // int val = strtol(yytext, &endptr, 0);
                            // if(endptr != yytext + strlen(yytext)) {
                            //     IR_LEXICAL_ERROR("Illegal integer '%s'.", yytext);
                            // }
                            // IR_yylval.INT = val;
                            return IR_TOKEN_INT;
                        }
":="                    { return IR_TOKEN_ASSIGN; }
"+"                     { 
                            // IR_yylval.IR_op_type = IR_OP_ADD;
                            return IR_TOKEN_OP;
                        }
"-"                     { /* 为op赋值 */  return IR_TOKEN_OP; }
"*"                     { /* 为op赋值 */  return IR_TOKEN_STAR; }
"/"                     { /* 为op赋值 */  return IR_TOKEN_OP; }
"=="                    { /* 为op赋值 */  return IR_TOKEN_RELOP; }
"!="                    { /* 为op赋值 */  return IR_TOKEN_RELOP; }
">"                     { /* 为op赋值 */  return IR_TOKEN_RELOP; }
">="                    { /* 为op赋值 */  return IR_TOKEN_RELOP; }
"<"                     { /* 为op赋值 */  return IR_TOKEN_RELOP; }
"<="                    { /* 为op赋值 */  return IR_TOKEN_RELOP; }
"&"                     { return IR_TOKEN_ADDR_OF; }
"IF"                    { return IR_TOKEN_IF; }
"GOTO"                  { return IR_TOKEN_GOTO; }
"RETURN"                { return IR_TOKEN_RETURN; }
"DEC"                   { return IR_TOKEN_DEC; }
"ARG"                   { return IR_TOKEN_ARG; }
"CALL"                  { return IR_TOKEN_CALL; }
"PARAM"                 { return IR_TOKEN_PARAM; }
"READ"                  { return IR_TOKEN_READ; }
"WRITE"                 { return IR_TOKEN_WRITE; }
[a-zA-Z_][a-zA-Z0-9_]*  {
                            // IR_yylval.id = strdup(yytext);
                            return IR_TOKEN_ID;
                        }

[ \t]                   /* ignored */
\n+                     { return IR_TOKEN_EOL; }

.	                    { IR_LEXICAL_ERROR("Mysterious token \"%s\".", yytext); }

%%
