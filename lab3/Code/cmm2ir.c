#include "ast.h"
#include "production.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
            exp->address = ir_new_temp_variable();
            exp->code = ir_new_code_block();
            if (!strcmp(recs[0].ast_node->attr.identifier_value, "read")) {
                /* call special function read() */
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_read(exp->address)
                );
                return;
            }
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_call(
                    exp->address,
                    recs[0].ast_node->attr.identifier_value
                )
            );
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
            exp->address = ir_new_temp_variable();
            exp->code = ir_new_code_block();
            ast_node_t *args = recs[2].ast_node;
            if (!strcmp(recs[0].ast_node->attr.identifier_value, "write")) {
                /* call special function write(x) */
                ast_node_t *exp_ = ast_1st_child(args);
                exp->code = ir_concat_code_block(
                    exp->code,
                    exp_->code
                );
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_write(exp_->address)
                );
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_op(exp->address, ir_get_int_variable(0), NULL, "ASSIGN") /* write(x) always return 0 */
                );
                return;
            }
            while (1) {
                ast_node_t *exp_ = ast_1st_child(args);
                exp->code = ir_concat_code_block(
                    exp->code,
                    exp_->code
                );
                if (ast_onlyone_child(args)) break;
                args = ast_3rd_child(args);
            }
            exp->code = ir_concat_code_block(
                exp->code,
                args->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_call(
                    exp->address,
                    recs[0].ast_node->attr.identifier_value
                )
            );
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
            switch (production_is_leftvalue_exp(recs[0].ast_node)) {
            case 1:
                exp->address = recs[0].ast_node->address;
                exp->code = ir_concat_code_block(recs[2].ast_node->code, recs[0].ast_node->code);
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_op(recs[0].ast_node->address, recs[2].ast_node->address, NULL, "ASSIGN")
                );
                break;
            case 2:
                exp->address = recs[0].ast_node->address;
                ir_remove_last_code(recs[0].ast_node->code);
                exp->code = ir_concat_code_block(recs[2].ast_node->code, recs[0].ast_node->code);
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_op(recs[0].ast_node->address, recs[2].ast_node->address, NULL, "DEREF_L")
                );
                break;
            default:
                break;
            }
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
            ir_code_t *go_out = ir_new_code_label();
            exp->address = ir_new_temp_variable();
            exp->code = ir_new_code_block();
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(0), NULL, "ASSIGN")
            );
            exp->code = ir_concat_code_block(
                exp->code,
                recs[0].ast_node->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_relop_goto(
                    recs[0].ast_node->address,
                    ir_get_int_variable(0),
                    "==",
                    go_out
                )
            );
            exp->code = ir_concat_code_block(
                exp->code,
                recs[2].ast_node->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[2].ast_node->address, NULL, "ASSIGN")
            );
            exp->code = ir_append_code(
                exp->code,
                go_out
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
            ir_code_t *go_out = ir_new_code_label();
            exp->address = ir_new_temp_variable();
            exp->code = ir_new_code_block();
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(1), NULL, "ASSIGN")
            );
            exp->code = ir_concat_code_block(
                exp->code,
                recs[0].ast_node->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_relop_goto(
                    recs[0].ast_node->address,
                    ir_get_int_variable(0),
                    "!=",
                    go_out
                )
            );
            exp->code = ir_concat_code_block(
                exp->code,
                recs[2].ast_node->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, recs[2].ast_node->address, NULL, "ASSIGN")
            );
            exp->code = ir_append_code(
                exp->code,
                go_out
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
            exp->address = ir_new_temp_variable();
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            ir_code_t *true_skip = ir_new_code_label();
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(1), NULL, "ASSIGN")
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_relop_goto(
                    recs[0].ast_node->address,
                    recs[2].ast_node->address,
                    recs[1].ast_node->attr.relop_name,
                    true_skip
                )
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(0), NULL, "ASSIGN")
            );
            exp->code = ir_append_code(
                exp->code,
                true_skip
            );
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
            ir_code_t *go_out = ir_new_code_label();
            exp->address = ir_new_temp_variable();
            exp->code = ir_new_code_block();
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(0), NULL, "ASSIGN")
            );
            exp->code = ir_concat_code_block(
                exp->code,
                recs[1].ast_node->code
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_relop_goto(
                    recs[1].ast_node->address,
                    ir_get_int_variable(0),
                    "!=",
                    go_out
                )
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, ir_get_int_variable(1), NULL, "ASSIGN")
            );
            exp->code = ir_append_code(
                exp->code,
                go_out
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
            exp->address = ir_new_temp_variable();
            ir_variable_t *offset = ir_new_temp_variable();
            ir_variable_t *base = production_is_leftvalue_exp(recs[0].ast_node) == 1 ? ir_get_ref_variable(recs[0].ast_node->address) : recs[0].ast_node->address;
            ir_variable_t *size = ir_get_int_variable(recs[0].ast_node->exp_type->elem_type->size_in_bytes);
            ir_variable_t *index = recs[2].ast_node->address;
            exp->code = ir_concat_code_block(recs[0].ast_node->code, recs[2].ast_node->code);
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(offset, index, size, "MULT")
            );
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, base, offset, "PLUS")
            );
            if (exp->exp_type->primitive != PRIM_ARRAY) {
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_op(exp->address, exp->address, NULL, "DEREF_R")
                );
            }
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
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_CompSt},
        };
        if (production_match(stmt, recs, 1)) {
            stmt->code = recs[0].ast_node->code;
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_SEMI},
        };
        if (production_match(stmt, recs, 2)) {
            stmt->code = recs[0].ast_node->code;
            return;
        }
    }
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_RETURN},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_SEMI},
        };
        if (production_match(stmt, recs, 3)) {
            stmt->code = recs[1].ast_node->code;
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_return(recs[1].ast_node->address)
            );
            return;
        }
    }
    {
        production_rec_t recs[5] = {
            {NULL, AST_NODE_IF},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RP},
            {NULL, AST_NODE_Stmt},
        };
        if (production_match(stmt, recs, 5)) {
            stmt->code = recs[2].ast_node->code;
            ir_code_t *false_branch = ir_new_code_label();
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_relop_goto(
                    recs[2].ast_node->address,
                    ir_get_int_variable(0),
                    "==",
                    false_branch
                )
            );
            stmt->code = ir_concat_code_block(
                stmt->code,
                recs[4].ast_node->code
            );
            stmt->code = ir_append_code(
                stmt->code,
                false_branch
            );
            return;
        }
    }
    {
        production_rec_t recs[7] = {
            {NULL, AST_NODE_IF},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RP},
            {NULL, AST_NODE_Stmt},
            {NULL, AST_NODE_ELSE},
            {NULL, AST_NODE_Stmt},
        };
        if (production_match(stmt, recs, 7)) {
            stmt->code = recs[2].ast_node->code;
            ir_code_t *false_branch = ir_new_code_label();
            ir_code_t *go_out = ir_new_code_label();
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_relop_goto(
                    recs[2].ast_node->address,
                    ir_get_int_variable(0),
                    "==",
                    false_branch
                )
            );
            stmt->code = ir_concat_code_block(
                stmt->code,
                recs[4].ast_node->code
            );
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_goto(go_out)
            );
            stmt->code = ir_append_code(
                stmt->code,
                false_branch
            );
            stmt->code = ir_concat_code_block(
                stmt->code,
                recs[6].ast_node->code
            );
            stmt->code = ir_append_code(
                stmt->code,
                go_out
            );
            return;
        }
    }
    {
        production_rec_t recs[5] = {
            {NULL, AST_NODE_WHILE},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_RP},
            {NULL, AST_NODE_Stmt},
        };
        if (production_match(stmt, recs, 5)) {
            ir_code_t *very_begin = ir_new_code_label();
            ir_code_t *go_out = ir_new_code_label();
            stmt->code = ir_new_code_block();
            stmt->code = ir_append_code(
                stmt->code,
                very_begin
            );
            stmt->code = ir_concat_code_block(
                stmt->code,
                recs[2].ast_node->code
            );
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_relop_goto(
                    recs[2].ast_node->address,
                    ir_get_int_variable(0),
                    "==",
                    go_out
                )
            );
            stmt->code = ir_concat_code_block(
                stmt->code,
                recs[4].ast_node->code
            );
            stmt->code = ir_append_code(
                stmt->code,
                ir_new_code_goto(very_begin)
            );
            stmt->code = ir_append_code(
                stmt->code,
                go_out
            );
            return;
        }
    }
}

static void handle_stmtlist(ast_node_t *stmtlist) {
    if (!production_epsilon(stmtlist)) {
        ast_node_t *stmt = ast_1st_child(stmtlist);
        ast_node_t *rest = ast_2nd_child(stmtlist);
        stmtlist->code = ir_concat_code_block(stmt->code, rest->code);
    } else {
        stmtlist->code = ir_new_code_block();
    }
}

static void handle_compst(ast_node_t *compst) {
    ast_node_t *stmtlist = ast_3rd_child(compst);
    ast_node_t *deflist = ast_2nd_child(compst);
    struct ir_dec_t *head = deflist->ir_dec_head;
    compst->code = ir_new_code_block();
    while (head != NULL) {
        compst->code = ir_append_code(
            compst->code,
            ir_new_code_dec(head->name, head->size)
        );
        head = head->next;
    }
    compst->code = ir_concat_code_block(
        compst->code,
        stmtlist->code
    );
}

static void handle_extdef(ast_node_t *extdef) {
    if (ast_2nd_child(extdef)->node_type == AST_NODE_FunDec) {
        extdef->code = ir_new_code_block();
        ast_node_t *fundec = ast_2nd_child(extdef);
        ast_node_t *id = ast_1st_child(fundec);
        ast_node_t *compst = ast_3rd_child(extdef);
        extdef->code = ir_append_code(
            extdef->code,
            ir_new_code_fundec(id->attr.identifier_value)
        );

        ast_node_t *varlist = ast_3rd_child(fundec);
        if (varlist->node_type == AST_NODE_VarList) {
            while (1) {
                ast_node_t *paramdec = ast_1st_child(varlist);
                ast_node_t *spec = ast_1st_child(paramdec);
                ast_node_t *vardec = ast_2nd_child(paramdec);

                /* assert that there is no array param */
                ast_node_t *id = ast_1st_child(vardec);
                extdef->code = ir_append_code(
                    extdef->code,
                    ir_new_code_param(ir_get_id_variable(id->attr.identifier_value))
                );
                
                if (ast_onlyone_child(varlist)) break;
                varlist = ast_3rd_child(varlist);
            }
        }

        extdef->code = ir_concat_code_block(
            extdef->code,
            compst->code
        );
    }
}

static void handle_extdeflist(ast_node_t *extdeflist) {
    if (production_epsilon(extdeflist)) {
        extdeflist->code = ir_new_code_block();
    } else {
        ast_node_t *extdef = ast_1st_child(extdeflist);
        ast_node_t *rest = ast_2nd_child(extdeflist);
        extdeflist->code = ir_concat_code_block(
            extdef->code,
            rest->code
        );
    }
}

static void handle_program(ast_node_t *program) {
    ast_node_t *extdeflist = ast_1st_child(program);
    ir_dump(stdout, extdeflist->code);
}

static void handle_args(ast_node_t *args) {
    if (ast_onlyone_child(args)) {
        ast_node_t *exp = ast_1st_child(args);
        args->code = ir_new_code_block();
        args->code = ir_append_code(
            args->code,
            ir_new_code_arg(exp->address)
        );
    } else {
        ast_node_t *exp = ast_1st_child(args);
        ast_node_t *rest = ast_3rd_child(args);
        args->code = rest->code;
        args->code = ir_append_code(
            args->code,
            ir_new_code_arg(exp->address)
        );
    }
}

static void handler_entry(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Exp) handle_exp(ast_node);
    else if (ast_node->node_type == AST_NODE_Stmt) handle_stmt(ast_node);
    else if (ast_node->node_type == AST_NODE_StmtList) handle_stmtlist(ast_node);
    else if (ast_node->node_type == AST_NODE_CompSt) handle_compst(ast_node);
    else if (ast_node->node_type == AST_NODE_ExtDef) handle_extdef(ast_node);
    else if (ast_node->node_type == AST_NODE_ExtDefList) handle_extdeflist(ast_node);
    else if (ast_node->node_type == AST_NODE_Program) handle_program(ast_node);
    else if (ast_node->node_type == AST_NODE_Args) handle_args(ast_node);
}

void cmm2ir() {
    ast_walk(ast_walk_action_nop, handler_entry);
}