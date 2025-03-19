#include <stdio.h>
#include <assert.h>
#include "ast.h"
#include "symbtable.h"
#include "log.h"
#include "production.h"

extern int yyparse();
extern int yyrestart(FILE *);

int define_new_symb(const char *symb) {
    if (symbtable_query_entry(symb)) return 0;
    symbtable_add_entry(symb);
    return 1;
}

void handle_ast_vardec(ast_node_t *ast_vardec) {
    forall_children(&(ast_vardec->tree_node), child) {
        ast_node_t *ast_child = tree2ast(child);
        if (ast_child->node_type == AST_NODE_ID) {
            const char *symb = ast_child->attr.identifier_value;
            if (!define_new_symb(symb)) {
                log_semantics_error_prologue("3", ast_child->lineno);
                fprintf(stdout, "Redefined variable \"%s\".\n", symb);
            }
        } else if (ast_child->node_type == AST_NODE_VarDec) {
            handle_ast_vardec(ast_child);
        }
    }
}

void handle_ast_dec(ast_node_t *ast_dec) {
    forall_children(&(ast_dec->tree_node), child) {
        ast_node_t *ast_child = tree2ast(child);
        if (ast_child->node_type == AST_NODE_VarDec) {
            handle_ast_vardec(ast_child);
        }
    }
}

void handle_ast_declist(ast_node_t *ast_declist) {
    forall_children(&(ast_declist->tree_node), child) {
        ast_node_t *ast_child = tree2ast(child);
        if (ast_child->node_type == AST_NODE_DecList) {
            handle_ast_declist(ast_child);
            break;
        } else if (ast_child->node_type == AST_NODE_Dec) handle_ast_dec(ast_child);
    }
}

void handle_ast_def(ast_node_t *ast_def) {
    forall_children(&(ast_def->tree_node), child) {
        ast_node_t *ast_child = tree2ast(child);
        if (ast_child->node_type == AST_NODE_DecList) {
            handle_ast_declist(ast_child);
            break;
        }
    }
}

void handle_ast_exp(ast_node_t *ast_exp) {
    /*
        检查最底层的标识符产生式
        1. Exp -> ID
        2. Exp -> ID LP RP
        3. Exp -> ID LP Args RP
    */
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_ID}
        };
        if (production_match(ast_exp, recs, 1)) {
            const char *symb = recs[0].ast_node->attr.identifier_value;
            if (!symbtable_query_entry(symb)) {
                log_semantics_error_prologue("1", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined variable \"%s\".\n", symb);
            }
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_ID},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_RP}
        };
        if (production_match(ast_exp, recs, 3)) {
            const char *symb = recs[0].ast_node->attr.identifier_value;
            if (!symbtable_query_entry(symb)) {
                log_semantics_error_prologue("2", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
            }
        }
    }
    {
        production_rec_t recs[4] = {
            {NULL, AST_NODE_ID},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Args},
            {NULL, AST_NODE_RP}
        };
        if (production_match(ast_exp, recs, 4)) {
            const char *symb = recs[0].ast_node->attr.identifier_value;
            if (!symbtable_query_entry(symb)) {
                log_semantics_error_prologue("2", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
            }
        }
    }
}

void handle_ast_node(ast_node_t *ast_node, int depth) {
    switch (ast_node->node_type) {
    case AST_NODE_Def:
        handle_ast_def(ast_node);
        break;
    case AST_NODE_Exp:
        handle_ast_exp(ast_node);
        break;
    default:
        break;
    }
}

int parse_error = 0;

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
    assert(!parse_error);
    ast_walk(handle_ast_node, ast_walk_action_nop);
    return 0;
}
