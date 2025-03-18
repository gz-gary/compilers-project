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

ast_node_t *ast_root = NULL;

ast_node_t *ast_set_root(ast_node_t *ast_node) {
    ast_root = ast_node;
}

extern int yylineno;
ast_node_t *ast_new_node(enum AST_NODE_TYPE type)
{
    ast_node_t *new_node = malloc(sizeof(ast_node_t));
    tree_init_node(&(new_node->tree_node));
    new_node->attr.type = type;
    new_node->attr.lineno = yylineno;
    return new_node;
}

ast_node_t *ast_add_child(ast_node_t *father, ast_node_t *child) {
    tree_add_child(&(father->tree_node), &(child->tree_node));
}

static void ast_walk_dfs(ast_node_t *ast_node, int depth, ast_walk_action_t preorder_action, ast_walk_action_t postorder_action) {
    preorder_action(ast_node, depth);
    forall_children(&(ast_node->tree_node), child) {
        ast_node_t *ast_child = tree2ast(child);
        ast_walk_dfs(ast_child, depth + 1, preorder_action, postorder_action);
    }
    postorder_action(ast_node, depth);
}

void ast_walk(ast_walk_action_t preorder_action, ast_walk_action_t postorder_action) {
    ast_walk_dfs(ast_root, 0, preorder_action, postorder_action);
}

void ast_walk_action_nop(ast_node_t *ast_node, int depth) {
    return;
}

int ast_is_leaf_node(ast_node_t *ast_node) {
    return (ast_node->tree_node.first_child == NULL) ? 1 : 0;
}

int ast_is_term_node(ast_node_t *ast_node) {
    return (ast_node->attr.type >= AST_NODE_Program) ? 1 : 0;
}
