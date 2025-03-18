#include "tree.h"
#include <unistd.h>
#include <stdlib.h>

void tree_add_child(tree_node_t *father, tree_node_t *child) {
    child->next_brother = father->first_child;
    father->first_child = child;
}

void tree_init_node(tree_node_t *node) {
    node->first_child = NULL;
    node->next_brother = NULL;
}
