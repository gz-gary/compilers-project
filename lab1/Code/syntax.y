%locations
%{
    #include "ast.h"
    #define YYSTYPE ast_node_t*

    #include "lex.yy.c"
    int yyerror(const char *s);
%}

%token INT
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token DOT
%token NOT
%token TYPE
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE

%right ASSIGNOP
%left AND OR
/* RELOP中 != 和 == 优先级更低一点 */
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

Program: ExtDefList {
    $$ = ast_new_node(AST_NODE_Program);
    ast_add_child($$, $1);
    ast_set_root($$);
}
    ;
ExtDefList: ExtDef ExtDefList {
    $$ = ast_new_node(AST_NODE_ExtDefList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | { $$ = ast_new_node(AST_NODE_ExtDefList); } /* empty */
    ;
ExtDef : Specifier ExtDecList SEMI {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Specifier SEMI {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Specifier FunDec CompSt {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
ExtDecList: VarDec {
    $$ = ast_new_node(AST_NODE_ExtDecList);
    ast_add_child($$, $1);
}
    | VarDec COMMA ExtDecList {
    $$ = ast_new_node(AST_NODE_ExtDecList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;

Specifier: TYPE {
    $$ = ast_new_node(AST_NODE_Specifier);
    ast_add_child($$, $1);
}
    | StructSpecifier {
    $$ = ast_new_node(AST_NODE_Specifier);
    ast_add_child($$, $1);
}
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
    $$ = ast_new_node(AST_NODE_StructSpecifier);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | STRUCT Tag {
    $$ = ast_new_node(AST_NODE_StructSpecifier);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
OptTag: ID { $$ = ast_new_node(AST_NODE_OptTag); ast_add_child($$, $1); }
    | { $$ = ast_new_node(AST_NODE_OptTag); } /* empty */
    ;
Tag: ID { $$ = ast_new_node(AST_NODE_Tag); ast_add_child($$, $1); }
    ;

VarDec: ID { $$ = ast_new_node(AST_NODE_VarDec); ast_add_child($$, $1); }
    | VarDec LB INT RB {
    $$ = ast_new_node(AST_NODE_VarDec);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
FunDec: ID LP VarList RP {
    $$ = ast_new_node(AST_NODE_FunDec);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | ID LP RP {
    $$ = ast_new_node(AST_NODE_FunDec);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
VarList: ParamDec COMMA VarList {
    $$ = ast_new_node(AST_NODE_VarList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | ParamDec {
    $$ = ast_new_node(AST_NODE_VarList);
    ast_add_child($$, $1);
}
    ;
ParamDec: Specifier VarDec {
    $$ = ast_new_node(AST_NODE_ParamDec);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;

CompSt: LC DefList StmtList RC {
    $$ = ast_new_node(AST_NODE_CompSt);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
StmtList: Stmt StmtList {
    $$ = ast_new_node(AST_NODE_StmtList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | { $$ = ast_new_node(AST_NODE_StmtList); } /* empty */
    ;
Stmt: Exp SEMI {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | CompSt {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $1);
}
    | RETURN Exp SEMI {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | IF LP Exp RP Stmt ELSE Stmt {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $7);
    ast_add_child($$, $6);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | WHILE LP Exp RP Stmt {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;

DefList: Def DefList {
    $$ = ast_new_node(AST_NODE_DefList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | { $$ = ast_new_node(AST_NODE_DefList); } /* empty */
    ;
Def: Specifier DecList SEMI {
    $$ = ast_new_node(AST_NODE_Def);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
DecList: Dec {
    $$ = ast_new_node(AST_NODE_DecList);
    ast_add_child($$, $1);
}
    | Dec COMMA DecList {
    $$ = ast_new_node(AST_NODE_DecList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;
Dec: VarDec {
    $$ = ast_new_node(AST_NODE_Dec);
    ast_add_child($$, $1);
}
    | VarDec ASSIGNOP Exp {
    $$ = ast_new_node(AST_NODE_Dec);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    ;

Exp: Exp ASSIGNOP Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp AND Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp OR Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp RELOP Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp PLUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp MINUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp STAR Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp DIV Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | LP Exp RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | MINUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | PLUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | NOT Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | ID LP Args RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | ID LP RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp LB Exp RB {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp DOT ID {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | ID {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
}
    | INT {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
}
    | FLOAT {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
}
    ;

Args: Exp COMMA Args {
    $$ = ast_new_node(AST_NODE_Args);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
}
    | Exp {
    $$ = ast_new_node(AST_NODE_Args);
    ast_add_child($$, $1);
}
    ;

%%