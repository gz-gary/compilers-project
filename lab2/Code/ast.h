#ifndef AST_H
#define AST_H

#include "type.h"
#include "tree.h"
#define tree2ast(node) ((ast_node_t *)((void *)(node) - (void *)(&(((ast_node_t *)(0))->tree_node))))
#define ast_1st_child(node) tree2ast((node)->tree_node.first_child)
#define ast_2nd_child(node) tree2ast((node)->tree_node.first_child->next_brother)
#define ast_3rd_child(node) tree2ast((node)->tree_node.first_child->next_brother->next_brother)
#define ast_onlyone_child(node) (((node)->tree_node.first_child->next_brother) == NULL)

const char *AST_NODE_TYPE_NAMES[48];

enum ast_node_type_t {
    AST_NODE_INT = 0,
    AST_NODE_FLOAT,
    AST_NODE_ID,
    AST_NODE_SEMI,
    AST_NODE_COMMA,
    AST_NODE_ASSIGNOP,
    AST_NODE_RELOP,
    AST_NODE_PLUS,
    AST_NODE_MINUS,
    AST_NODE_STAR,
    AST_NODE_DIV,
    AST_NODE_AND,
    AST_NODE_OR,
    AST_NODE_DOT,
    AST_NODE_NOT,
    AST_NODE_TYPE,
    AST_NODE_LP,
    AST_NODE_RP,
    AST_NODE_LB,
    AST_NODE_RB,
    AST_NODE_LC,
    AST_NODE_RC,
    AST_NODE_STRUCT,
    AST_NODE_RETURN,
    AST_NODE_IF,
    AST_NODE_ELSE,
    AST_NODE_WHILE,
    AST_NODE_Program,
    AST_NODE_ExtDefList,
    AST_NODE_ExtDef,
    AST_NODE_ExtDecList,
    AST_NODE_Specifier,
    AST_NODE_StructSpecifier,
    AST_NODE_OptTag,
    AST_NODE_Tag,
    AST_NODE_VarDec,
    AST_NODE_FunDec,
    AST_NODE_VarList,
    AST_NODE_ParamDec,
    AST_NODE_CompSt,
    AST_NODE_StmtList,
    AST_NODE_Stmt,
    AST_NODE_DefList,
    AST_NODE_Def,
    AST_NODE_DecList,
    AST_NODE_Dec,
    AST_NODE_Exp,
    AST_NODE_Args,
};

struct ast_node_t {
    enum ast_node_type_t node_type;
    int lineno; // line number
    type_t *exp_type; // extension for type checking
    int symb_to_del; // extension for scope
    union {
        int int_value;
        float float_value;
        const char *identifier_value;
        const char *typename_value;
    } attr;
    tree_node_t tree_node;
};
typedef struct ast_node_t ast_node_t;

ast_node_t *ast_root;
ast_node_t *ast_set_root(ast_node_t *ast_node);
ast_node_t *ast_new_node(enum ast_node_type_t node_type);
ast_node_t *ast_add_child(ast_node_t *father, ast_node_t *child);
/*
    ast_walk() will traverse the AST by depth-first-search algorithm
    and call preorder_action(ast_node, depth) and postorder_action(ast_node, depth) on each node.
    - ast_node: a pointer to the node
    - depth: depth of the node
*/ 
typedef void (*ast_walk_action_t)(ast_node_t*, int);
void ast_walk(ast_walk_action_t preorder_action, ast_walk_action_t postorder_action);
void ast_walk_action_nop(ast_node_t*, int);
int ast_is_leaf_node(ast_node_t *ast_node);
int ast_is_term_node(ast_node_t *ast_node);

#endif