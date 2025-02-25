#ifndef TREE_H
#define TREE_H

#define forall_children(father, child) \
    for (tree_node_t *child = (father)->first_child; \
         child != NULL; \
         child = child->next_brother)

struct tree_node_t {
    struct tree_node_t *first_child, *next_brother;
};
typedef struct tree_node_t tree_node_t;

tree_node_t* tree_root;
void tree_add_child(tree_node_t *father, tree_node_t *child);
tree_node_t* tree_new_node();
void tree_init_root();

#endif