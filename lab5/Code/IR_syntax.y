%define api.prefix {IR_yy}
%define api.token.prefix {IR_TOKEN_}
%define parse.error verbose
%locations

%{
#include <stdio.h>
#include <stdlib.h>
#include "lex.yy.c"
#include "IR_parse.h"

// TODO: 为这里添加一个语法树节点
// #define YYSTYPE ast_node_t*
%}


%token EOL
%token COLON
%token FUNCTION
%token LABEL
%token SHARP
%token INT
%token ASSIGN
%token STAR
%token OP
%token IF
%token RELOP
%token ADDR_OF
%token GOTO
%token RETURN
%token DEC
%token ARG
%token CALL
%token PARAM
%token READ
%token WRITE
%token ID


%%

IR_globol   : MUL_EOL IR_program                        { /*ir_program_global = $2;*/ }
            ;       
IR_program  : IR_program IR_function                    {
                                                            // $$ = $1;
                                                            // // 为函数构造闭包
                                                            // IR_function_closure($2);
                                                            // // 将function放进program
                                                            // VCALL($$->functions, push_back, $2);
                                                        }
            |                                           { /*$$ = NEW(IR_program);*/ }
            ;
IR_function : FUNCTION ID COLON EOL                     { /*$$ = NEW(IR_function, $2); now_function = $$; free($2);*/ }
            | IR_function PARAM IR_var EOL              { /*$$ = $1; VCALL($$->params, push_back, $3);*/ }
            | IR_function DEC IR_var INT EOL            { /*$$ = $1; IR_function_insert_dec($$, $3, (unsigned)$4);*/ }
            | IR_function ARG IR_val_rs EOL             { /*$$ = $1; args_stack_push($3); */}
            | IR_function LABEL IR_label COLON EOL      { /*$$ = $1; IR_function_push_label($$, $3);*/ }
            | IR_function IR_stmt EOL                   { /*$$ = $1; IR_function_push_stmt($$, $2); */}
            ;
IR_stmt     : IR_var ASSIGN IR_val                      { /*$$ = (IR_stmt*)NEW(IR_assign_stmt, $1, $3);*/ }
            | IR_var ASSIGN val_deref                   { /*$$ = (IR_stmt*)NEW(IR_load_stmt, $1, $3);*/ }
            | val_deref ASSIGN IR_val_rs                { /*$$ = (IR_stmt*)NEW(IR_store_stmt, $1, $3);*/ }
            | IR_var ASSIGN IR_val_rs OP IR_val_rs      { /*$$ = (IR_stmt*)NEW(IR_op_stmt, $4, $1, $3, $5);*/ }
            | IR_var ASSIGN IR_val_rs STAR IR_val_rs    { /*$$ = (IR_stmt*)NEW(IR_op_stmt, $4, $1, $3, $5);*/ }
            | GOTO IR_label                             { /*$$ = (IR_stmt*)NEW(IR_goto_stmt, $2);*/ }
            | IF IR_val_rs RELOP IR_val_rs GOTO IR_label
                                                        { /*$$ = (IR_stmt*)NEW(IR_if_stmt, $3, $2, $4, $6, IR_LABEL_NONE);*/ }
            | RETURN IR_val                             { /*$$ = (IR_stmt*)NEW(IR_return_stmt, $2);*/ }
            | IR_var ASSIGN CALL ID                     {
                                                            //  unsigned argc;
                                                            //  IR_val *argv;
                                                            //  args_stack_pop(&argc, &argv);
                                                            //  $$ = (IR_stmt*)NEW(IR_call_stmt, $1, $4, argc, argv);
                                                            //  free($4);
                                                        }
            | READ IR_var                               { /*$$ = (IR_stmt*)NEW(IR_read_stmt, $2);*/ }
            | WRITE IR_val_rs                           { /*$$ = (IR_stmt*)NEW(IR_write_stmt, $2);*/ }
            ;
IR_val      : IR_var                                    { /*$$ = (IR_val){.is_const = false, .var = $1};*/ }
            | SHARP INT                                 { /*$$ = (IR_val){.is_const = true, .const_val = $2};*/ }
            | ADDR_OF IR_var                            { /*$$ = (IR_val){.is_const = false,*/
                                                          /*              .var = VCALL(now_function->map_dec, get, $2).dec_addr}; */}
            ;
val_deref   : STAR IR_val                               { /*$$ = $2;*/ }
IR_val_rs   : IR_val                                    { /*$$ = $1;*/ }
            | val_deref                                 {
                                                            // IR_var content = ir_var_generator();
                                                            // IR_function_push_stmt(now_function,
                                                            //                       (IR_stmt*)NEW(IR_load_stmt, content, $1));
                                                            // $$ = (IR_val){.is_const = false, .var = content};
                                                        }
IR_var      : ID                                        { /*$$ = get_IR_var($1); free($1);*/ }
            ;
IR_label    : ID                                        { /*$$ = get_IR_label($1); free($1);*/ }
            ;
MUL_EOL     : MUL_EOL EOL
            |
            ;

%%

int IR_yyerror(const char *msg) {
    fprintf(stderr, "Error: %s at line %d\n", msg, IR_yylineno);
    return 0;
}
