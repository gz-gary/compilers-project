#ifndef PRODUCTION_H
#define PRODUCTION_H

#include "ast.h"

typedef struct production_rec_t production_rec_t;

struct production_rec_t {
    ast_node_t *ast_node;
    enum AST_NODE_TYPE node_type;
};

int production_match(ast_node_t *ast_node, production_rec_t *rec, int length);

#endif