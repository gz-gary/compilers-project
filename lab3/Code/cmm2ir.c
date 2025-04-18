#include "ast.h"
#include "production.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

FILE *output_file = NULL;

static struct ir_dec_t *current_param_head = NULL;

static int is_ref(const char *name) {
    struct ir_dec_t *head = current_param_head;
    while (head) {
        if (!strcmp(head->name, name)) return 1;
        head = head->next;
    }
    return 0;
}

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
            case 3:
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
            if (exp->exp_type->primitive == PRIM_BASIC) {
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
            exp->address = ir_new_temp_variable();
            ir_variable_t *base = 
                production_is_leftvalue_exp(recs[0].ast_node) == 1
                && !is_ref(ast_1st_child(recs[0].ast_node)->attr.identifier_value)
                ? ir_get_ref_variable(recs[0].ast_node->address)
                : recs[0].ast_node->address;
            type_t *type = recs[0].ast_node->exp_type;
            field_t *field = type_query_struct_field(type, recs[2].ast_node->attr.identifier_value);
            ir_variable_t *offset = ir_get_int_variable(field->offset);
            exp->code = recs[0].ast_node->code;
            exp->code = ir_append_code(
                exp->code,
                ir_new_code_op(exp->address, base, offset, "PLUS")
            );
            if (exp->exp_type->primitive == PRIM_BASIC) {
                exp->code = ir_append_code(
                    exp->code,
                    ir_new_code_op(exp->address, exp->address, NULL, "DEREF_R")
                );
            }
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

static void handle_vardec(ast_node_t *vardec) {
    vardec->code = ir_new_code_block();
    if (vardec->ir_dec_head != NULL) { // 定义为数组的情况
        vardec->code = ir_append_code(
            vardec->code,
            ir_new_code_dec(vardec->ir_dec_head->name, vardec->ir_dec_head->size)
        );
    }
}

static void handle_dec(ast_node_t *dec) {
    ast_node_t *vardec = ast_1st_child(dec);
    dec->code = vardec->code;
    if (!ast_onlyone_child(dec)) {
        ast_node_t *exp = ast_3rd_child(dec);
        /* 
            还没处理数组的赋值
            d[12] = 3; 符合语法
            d[12] = {3}; 不符合
        */
        ast_node_t *first_child = ast_1st_child(vardec);
        vardec->address = ir_get_id_variable(first_child->attr.identifier_value);
        dec->address = vardec->address;
        dec->code = ir_concat_code_block(exp->code, vardec->code);
        dec->code = ir_append_code(
            dec->code,
            ir_new_code_op(vardec->address, exp->address, NULL, "ASSIGN")
        );
    }
}

static void handle_declist(ast_node_t *declist) {
    ast_node_t *dec = ast_1st_child(declist);
    declist->code = dec->code;
    if (!ast_onlyone_child(declist)) {
        ast_node_t *declist_ = ast_3rd_child(declist);
        declist->code = ir_concat_code_block(
            dec->code,
            declist_->code
        );
    }
    assert(declist->code != NULL);
}

static void handle_deflist(ast_node_t *deflist) {
    if (production_epsilon(deflist)) {
        deflist->code = ir_new_code_block();
    } else {
        /*
            DefList -> Def DefList
            Def -> Specifier DecList SEMI
            DecList -> Dec
            | Dec COMMA DecList
            Dec -> VarDec
            | VarDec ASSIGNOP Exp
        */
        ast_node_t *def = ast_1st_child(deflist);
        ast_node_t *declist = ast_2nd_child(def);
        def->code = declist->code;
        ast_node_t *rest = ast_2nd_child(deflist);
        deflist->code = ir_concat_code_block(
            def->code,
            rest->code
        );
    }
}

static void handle_compst(ast_node_t *compst) {
    ast_node_t *stmtlist = ast_3rd_child(compst);
    ast_node_t *deflist = ast_2nd_child(compst);
    compst->code = ir_new_code_block();
    compst->code = ir_concat_code_block(
        compst->code,
        deflist->code
    );
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
    if (output_file == NULL) {
        output_file = stdout;
    }
    ir_dump(output_file, extdeflist->code);
}

static void handle_args(ast_node_t *args) {
    if (ast_onlyone_child(args)) {
        ast_node_t *exp = ast_1st_child(args);
        int flag = exp->exp_type->primitive == PRIM_STRUCT && production_is_leftvalue_exp(exp) == 1 ? 1 : 0;
        args->code = ir_new_code_block();
        args->code = ir_append_code(
            args->code,
            ir_new_code_arg(exp->address, flag)
        );
    } else {
        ast_node_t *exp = ast_1st_child(args);
        int flag = exp->exp_type->primitive == PRIM_STRUCT && production_is_leftvalue_exp(exp) == 1 ? 1 : 0;
        ast_node_t *rest = ast_3rd_child(args);
        args->code = rest->code;
        args->code = ir_append_code(
            args->code,
            ir_new_code_arg(exp->address, flag)
        );
    }
}

static void handle_fundec(ast_node_t *fundec) {
    current_param_head = fundec->ir_dec_head;
}

static void handler_entry(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Exp) handle_exp(ast_node);
    else if (ast_node->node_type == AST_NODE_Stmt) handle_stmt(ast_node);
    else if (ast_node->node_type == AST_NODE_StmtList) handle_stmtlist(ast_node);
    else if (ast_node->node_type == AST_NODE_VarDec) handle_vardec(ast_node);
    else if (ast_node->node_type == AST_NODE_Dec) handle_dec(ast_node);
    else if (ast_node->node_type == AST_NODE_DecList) handle_declist(ast_node);
    else if (ast_node->node_type == AST_NODE_DefList) handle_deflist(ast_node);
    else if (ast_node->node_type == AST_NODE_CompSt) handle_compst(ast_node);
    else if (ast_node->node_type == AST_NODE_ExtDef) handle_extdef(ast_node);
    else if (ast_node->node_type == AST_NODE_ExtDefList) handle_extdeflist(ast_node);
    else if (ast_node->node_type == AST_NODE_Program) handle_program(ast_node);
    else if (ast_node->node_type == AST_NODE_Args) handle_args(ast_node);
}

static void preorder_entry(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_FunDec) handle_fundec(ast_node);
}

void cmm2ir() {
    ast_walk(preorder_entry, handler_entry);
}

void cmm2ir_and_dump(FILE *f) {
    output_file = f;
    cmm2ir();
}