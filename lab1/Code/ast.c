#include "ast.h"
#include <unistd.h>
#include <stdlib.h>

const char *AST_NODE_TYPE_NAMES[] = {
    "INT",
    "FLOAT",
    "ID",
    "SEMI",
    "COMMA",
    "ASSIGNOP",
    "RELOP",
    "PLUS",
    "MINUS",
    "STAR",
    "DIV",
    "AND",
    "OR",
    "DOT",
    "NOT",
    "TYPE",
    "LP",
    "RP",
    "LB",
    "RB",
    "LC",
    "RC",
    "STRUCT",
    "RETURN",
    "IF",
    "ELSE",
    "WHILE",
    "Program",
    "ExtDefList",
    "ExtDef",
    "ExtDecList",
    "Specifier",
    "StructSpecifier",
    "OptTag",
    "Tag",
    "VarDec",
    "FunDec",
    "VarList",
    "ParamDec",
    "CompSt",
    "StmtList",
    "Stmt",
    "DefList",
    "Def",
    "DecList",
    "Dec",
    "Exp",
    "Args",
};

ast_node_t *ast_new_node() {
    ast_node_t *new_node = malloc(sizeof(ast_node_t));
    new_node->tree_node = tree_new_node();
    return new_node;
}

ast_node_t *ast_add_child(ast_node_t *father, ast_node_t *child) {
    tree_add_child(father->tree_node, child->tree_node);
}
