#include "ast.h"
#include <unistd.h>
#include <stdlib.h>

ast_node_t *ast_new_node() {
    ast_node_t *new_node = malloc(sizeof(ast_node_t));
    new_node->tree_node = tree_new_node();
    return new_node;
}

ast_node_t *ast_add_child(ast_node_t *father, ast_node_t *child) {
    tree_add_child(father->tree_node, child->tree_node);
}
