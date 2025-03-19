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
