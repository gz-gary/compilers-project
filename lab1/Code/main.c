#include <stdio.h>
#include "ast.h"

extern int yyparse();
extern int yyrestart(FILE *);

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error: %s\n", s);
}

void print_ast_node(ast_node_t *ast_node, int depth) {
    /* 
        (handle epsilon production)
        do nothing with leaf-node term
    */
    if (ast_is_term_node(ast_node) && ast_is_leaf_node(ast_node)) return;

    for (int i = 0; i < depth * 2; ++i) putchar(' ');
    printf("%s", AST_NODE_TYPE_NAMES[ast_node->attr.type]);

    switch (ast_node->attr.type) {
        case AST_NODE_INT:
            printf(": %d", ast_node->attr.int_value);
            break;
        case AST_NODE_FLOAT:
            printf(": %f", ast_node->attr.float_value);
            break;
        case AST_NODE_ID:
            printf(": %s", ast_node->attr.identifier_value);
            break;
        case AST_NODE_TYPE:
            printf(": %s", ast_node->attr.typename_value);
            break;
        default:
            /* do something with line number */
            break;
    }
    
    putchar('\n');
}

int main(int argc, char const *argv[]) {
    FILE *f = NULL;
    if (argc > 1) {
        if (!(f = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
    }
    yyparse();
    ast_walk(print_ast_node);
    return 0;
}
