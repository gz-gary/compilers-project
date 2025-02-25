#include "tree.h"
#include <unistd.h>
#include <stdlib.h>

tree_node_t *tree_add_child(tree_node_t *father) {
    tree_node_t *new_node = malloc(sizeof(tree_node_t));
    if (!new_node) return NULL;
    new_node->first_child = NULL;
    new_node->next_brother = father->first_child;
    father->first_child = new_node;
    return new_node;
}

void tree_init_root() {
    tree_root = malloc(sizeof(tree_node_t));
    tree_root->first_child = NULL;
}
