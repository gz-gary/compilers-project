#include "production.h"
#include <stdlib.h>

int production_match(ast_node_t *ast_node, production_rec_t *rec, int length) {
    forall_children(&(ast_node->tree_node), child) {
        if (length == 0) return 0; // production is to long
        ast_node_t *ast_child = tree2ast(child);
        if (ast_child->node_type == rec->node_type) rec->ast_node = ast_child;
        else return 0;
        --length;
        ++rec;
    }
    if (length > 0) return 0; // production is to short
    return 1;
}

int production_epsilon(ast_node_t *ast_node) {
    return ast_node->tree_node.first_child == NULL;
}

int production_is_leftvalue_exp(ast_node_t *exp) {
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_ID}
        };
        if (production_match(exp, recs, 1)) return 1;
    }
    {
        production_rec_t recs[4] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_LB},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RB}
        };
        if (production_match(exp, recs, 4)) return 2;
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_DOT},
            {NULL, AST_NODE_ID}
        };
        if (production_match(exp, recs, 3)) return 3;
    }
    return 0;
}