#include "ast.h"
#include "production.h"
#include <stdio.h>
#include <stdlib.h>

static void handle_exp(ast_node_t *exp) {
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_ID}
        };
        if (production_match(exp, recs, 1)) {
            exp->address = ir_get_id_variable(recs[0].ast_node->attr.identifier_value);
            exp->code = ir_new_code_block();
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_ID},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_RP}
        };
        if (production_match(exp, recs, 3)) {
            /* function call */
            return;
        }
    }
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_INT}
        };
        if (production_match(exp, recs, 1)) {
            exp->address = ir_get_int_variable(recs[0].ast_node->attr.int_value);
            exp->code = ir_new_code_block();
            return;
        }
    }
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_FLOAT}
        };
        if (production_match(exp, recs, 1)) {
            exp->address = ir_get_float_variable(recs[0].ast_node->attr.float_value);
            exp->code = ir_new_code_block();
            return;
        }
    }
    {
        production_rec_t recs[4] = {
            {NULL, AST_NODE_ID},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Args},
            {NULL, AST_NODE_RP}
        };
        if (production_match(exp, recs, 4)) {
            /* function call */
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_ASSIGNOP},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = recs[0].ast_node->address;
            exp->code = ir_concat_code_block(recs[2].ast_node->code, recs[0].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(recs[0].ast_node->address, recs[2].ast_node->address, NULL, "ASSIGN")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_AND},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "AND")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_OR},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "OR")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RELOP},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            /* RELOP */
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_PLUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "PLUS")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_MINUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "MINUS")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_STAR},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "MULT")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_DIV},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[0].ast_node->address, recs[2].ast_node->address, "DIV")
            );
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RP}
        };
        if (production_match(exp, recs, 3)) {
            exp->address = recs[1].ast_node->address;
            exp->code = recs[1].ast_node->code;
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_MINUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            exp->address = ir_new_temp_variable();
            exp->code = recs[1].ast_node->code;
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[1].ast_node->address, NULL, "NEG")
            );
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_PLUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            exp->address = ir_new_temp_variable();
            exp->code = recs[1].ast_node->code;
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[1].ast_node->address, NULL, "POS")
            );
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_NOT},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            exp->address = ir_new_temp_variable();
            exp->code = recs[1].ast_node->code;
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[1].ast_node->address, NULL, "NOT")
            );
            return;
        }
    }
    {
        production_rec_t recs[4] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_LB},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RB}
        };
        if (production_match(exp, recs, 4)) {
            /* array access */
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_DOT},
            {NULL, AST_NODE_ID}
        };
        if (production_match(exp, recs, 3)) {
            /* field access */
            return;
        }
    }
}

static void handle_stmt(ast_node_t *stmt) {
}

static void handler_entry(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Exp) {
        handle_exp(ast_node);
        ir_dump(stdout, ast_node->code);
        fprintf(stdout, "\n");
    }
    if (ast_node->node_type == AST_NODE_Stmt) handle_stmt(ast_node);
}

void cmm2ir() {
    ast_walk(ast_walk_action_nop, handler_entry);
}