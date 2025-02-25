#include "tree.h"
#include <unistd.h>
#include <stdlib.h>

void tree_add_child(tree_node_t *father, tree_node_t *child) {
    child->next_brother = father->first_child;
    father->first_child = child;
}

tree_node_t *tree_new_node() {
    tree_node_t *new_node = malloc(sizeof(tree_node_t));
    new_node->first_child = NULL;
    new_node->next_brother = NULL;
    return new_node;
}

void tree_init_root() {
    tree_root = tree_new_node();
}
