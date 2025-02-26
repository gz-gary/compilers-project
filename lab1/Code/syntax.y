%locations
%{
    #include "ast.h"
    #define YYSTYPE ast_node_t*

    #include "lex.yy.c"
    int yyerror(const char *s);
%}

%union {
    int type_int;
    double type_float;
}

%type <type_int> UINT PINT INT_UNION
%type <type_float> FLOAT_UNION PFLOAT Exp Calc

%union {
    int type_int;
    double type_float;
}

%type <type_int> UINT PINT INT_UNION
%type <type_float> FLOAT_UNION PFLOAT Exp Calc

%token <type_int> <type_int> INT
%token <type_float> <type_float> FLOAT
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

%%

Calc: Exp { printf("Calc %lf\n", $1);}
    ;

/* 删除了bool相关的求值 */
Exp:  Exp AND Exp { $$ = $1 && $3; }
    | Exp OR Exp { $$ = $1 || $3; }
    | Exp PLUS Exp { $$ = $1 + $3; }
    | Exp MINUS Exp { $$ = $1 - $3; }
    | Exp STAR Exp { $$ = $1 * $3; }
    | Exp DIV Exp { $$ = $1 / $3; }
    | LP Exp RP { $$ = $2; }
    | MINUS Exp { $$ = -$2; }
    | INT_UNION { $$ = $1; }
    | FLOAT_UNION { $$ = $1; }
    ;

PINT: PLUS INT { $$ = $2; }
    ;

PFLOAT: PLUS FLOAT { $$ = $2; }
%%

Calc: Exp { printf("Calc %lf\n", $1);}
    ;

/* 删除了bool相关的求值 */
Exp:  Exp AND Exp { $$ = $1 && $3; }
    | Exp OR Exp { $$ = $1 || $3; }
    | Exp PLUS Exp { $$ = $1 + $3; }
    | Exp MINUS Exp { $$ = $1 - $3; }
    | Exp STAR Exp { $$ = $1 * $3; }
    | Exp DIV Exp { $$ = $1 / $3; }
    | LP Exp RP { $$ = $2; }
    | MINUS Exp { $$ = -$2; }
    | INT_UNION { $$ = $1; }
    | FLOAT_UNION { $$ = $1; }
    ;

INT_UNION: UINT
    | PINT
    ;
UINT: INT
    ;
PINT: PLUS INT { $$ = $2; }
    ;

FLOAT_UNION: FLOAT
    | PFLOAT
    ;
PFLOAT: PLUS FLOAT { $$ = $2; }
    ;

%%