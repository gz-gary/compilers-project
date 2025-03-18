%locations
%{
    #include "ast.h"
    #include "log.h"
    #define YYSTYPE ast_node_t*
    #define YYDEBUG 1

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
%token NEWLINE

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
    $$->attr.lineno = $1->attr.lineno;
}
    ;
ExtDefList: ExtDef ExtDefList {
    $$ = ast_new_node(AST_NODE_ExtDefList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | { $$ = ast_new_node(AST_NODE_ExtDefList); } /* empty */
    ;
ExtDef: Specifier ExtDecList SEMI {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Specifier SEMI {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Specifier FunDec CompSt {
    $$ = ast_new_node(AST_NODE_ExtDef);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
ExtDecList: VarDec {
    $$ = ast_new_node(AST_NODE_ExtDecList);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | VarDec COMMA ExtDecList {
    $$ = ast_new_node(AST_NODE_ExtDecList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

Specifier: TYPE {
    $$ = ast_new_node(AST_NODE_Specifier);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | StructSpecifier {
    $$ = ast_new_node(AST_NODE_Specifier);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
StructSpecifier: STRUCT OptTag LC DefList RC {
    $$ = ast_new_node(AST_NODE_StructSpecifier);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | STRUCT Tag {
    $$ = ast_new_node(AST_NODE_StructSpecifier);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
OptTag: ID {
    $$ = ast_new_node(AST_NODE_OptTag);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | { $$ = ast_new_node(AST_NODE_OptTag); } /* empty */
    ;
Tag: ID {
    $$ = ast_new_node(AST_NODE_Tag);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

VarDec: ID { $$ = ast_new_node(AST_NODE_VarDec); ast_add_child($$, $1); }
    | VarDec LB INT RB {
    $$ = ast_new_node(AST_NODE_VarDec);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
FunDec: ID LP VarList RP {
    $$ = ast_new_node(AST_NODE_FunDec);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ID LP RP {
    $$ = ast_new_node(AST_NODE_FunDec);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ID LP error RP
    ;
VarList: ParamDec COMMA VarList {
    $$ = ast_new_node(AST_NODE_VarList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ParamDec {
    $$ = ast_new_node(AST_NODE_VarList);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
ParamDec: Specifier VarDec {
    $$ = ast_new_node(AST_NODE_ParamDec);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

CompSt: LC DefList StmtList RC {
    $$ = ast_new_node(AST_NODE_CompSt);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | error RC
    ;
StmtList: Stmt StmtList {
    $$ = ast_new_node(AST_NODE_StmtList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | { $$ = ast_new_node(AST_NODE_StmtList); } /* empty */
    ;
Stmt: Exp SEMI {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | CompSt {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | RETURN Exp SEMI {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
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
    $$->attr.lineno = $1->attr.lineno;
}
    | WHILE LP Exp RP Stmt {
    $$ = ast_new_node(AST_NODE_Stmt);
    ast_add_child($$, $5);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE
    | IF LP error RP Stmt ELSE Stmt
    | WHILE LP error RC
    | WHILE LP error RP Stmt
    | error SEMI
    ;

DefList: Def DefList {
    $$ = ast_new_node(AST_NODE_DefList);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | { $$ = ast_new_node(AST_NODE_DefList); } /* empty */
    ;
Def: Specifier DecList SEMI {
    $$ = ast_new_node(AST_NODE_Def);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | error SEMI
    ;
DecList: Dec {
    $$ = ast_new_node(AST_NODE_DecList);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Dec COMMA DecList {
    $$ = ast_new_node(AST_NODE_DecList);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;
Dec: VarDec {
    $$ = ast_new_node(AST_NODE_Dec);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | VarDec ASSIGNOP Exp {
    $$ = ast_new_node(AST_NODE_Dec);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

Exp: Exp ASSIGNOP Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp AND Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp OR Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp RELOP Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp PLUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp MINUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp STAR Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp DIV Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | LP Exp RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | MINUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | PLUS Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | NOT Exp {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ID LP Args RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ID LP RP {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp LB Exp RB {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $4);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp DOT ID {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | ID {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | INT {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | FLOAT {
    $$ = ast_new_node(AST_NODE_Exp);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

Args: Exp COMMA Args {
    $$ = ast_new_node(AST_NODE_Args);
    ast_add_child($$, $3);
    ast_add_child($$, $2);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    | Exp {
    $$ = ast_new_node(AST_NODE_Args);
    ast_add_child($$, $1);
    $$->attr.lineno = $1->attr.lineno;
}
    ;

%%