#include "semantics.h"
#include "symbtable.h"
#include "production.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static type_t* handle_specifier(ast_node_t *specifier);

static type_t *handle_vardec(type_t *specifier_type, ast_node_t *vardec, const char **name) {
    ast_node_t *first_child = ast_1st_child(vardec);
    if (first_child->node_type == AST_NODE_ID) {
        *name = first_child->attr.identifier_value;
        return specifier_type;
    } else return handle_vardec(type_new_array(specifier_type), first_child, name);
}

static void handle_deflist(ast_node_t *deflist, type_t *upper_struct) {
    while (!production_epsilon(deflist)) {
        ast_node_t *def = ast_1st_child(deflist);
        ast_node_t *rest = ast_2nd_child(deflist);

        ast_node_t *def_spec = ast_1st_child(def);
        ast_node_t *declist = ast_2nd_child(def);
        type_t *spec_type = handle_specifier(def_spec);
        while (1) {
            ast_node_t *dec = ast_1st_child(declist);
            ast_node_t *vardec = ast_1st_child(dec);

            const char *name;
            type_t *type = handle_vardec(spec_type, vardec, &name);
            if (symbtable_query_entry(name)) {
                log_semantics_error_prologue("15", vardec->lineno);
                fprintf(stdout, "Redefined %s \"%s\".\n", upper_struct == NULL ? "variable" : "field", name);
            } else symbtable_add_entry(name, upper_struct == NULL ? SYMB_VAR : SYMB_FIELD, type);
            if (upper_struct)
                type_add_struct_field(upper_struct, type, name);
            
            if (ast_onlyone_child(declist)) break;
            ast_node_t *rest = ast_3rd_child(declist);
            declist = rest;
        }

        deflist = rest;
    }
}

static type_t* handle_specifier(ast_node_t *specifier) {
    ast_node_t *first_child = ast_1st_child(specifier);
    if (first_child->node_type == AST_NODE_TYPE) {
        /* case 1: basic type specifier */
        const char *typename = first_child->attr.typename_value;
        if (!strcmp(typename, "int")) return type_new_basic_int();
        if (!strcmp(typename, "float")) return type_new_basic_float();

    } else if (first_child->node_type == AST_NODE_StructSpecifier) {
        {
            /* case 2: using defined struct specifier */
            production_rec_t recs[2] = {
                {NULL, AST_NODE_STRUCT},
                {NULL, AST_NODE_Tag}
            };
            if (production_match(first_child, recs, 2)) {
                const char *symb = tree2ast((recs[1].ast_node)->tree_node.first_child)->attr.identifier_value;
                symbtable_entry_t *entry = symbtable_query_entry(symb);
                if (!entry || entry->symb_type != SYMB_STRUCT) {
                    log_semantics_error_prologue("17", recs[0].ast_node->lineno);
                    fprintf(stdout, "Undefined structure \"%s\".\n", symb);
                    return type_new_invalid();
                } else return entry->type;
            }
        }
        {
            /* case 3: define new struct specifier */
            production_rec_t recs[5] = {
                {NULL, AST_NODE_STRUCT},
                {NULL, AST_NODE_OptTag},
                {NULL, AST_NODE_LC},
                {NULL, AST_NODE_DefList},
                {NULL, AST_NODE_RC},
            };
            if (production_match(first_child, recs, 5)) {
                const char *symb = production_epsilon(recs[1].ast_node) ?
                    NULL :
                    ast_1st_child(recs[1].ast_node)->attr.identifier_value;

                ast_node_t *deflist = recs[3].ast_node;
                type_t *result = type_new_struct();
                handle_deflist(deflist, result);
                if (symb) {
                    symbtable_entry_t *entry = symbtable_query_entry(symb);
                    if (entry) {
                        log_semantics_error_prologue("16", (recs[1].ast_node)->lineno);
                        fprintf(stdout, "Redefined structure \"%s\".\n", symb);
                    } else symbtable_add_entry(symb, SYMB_STRUCT, result);
                }

                return result;
            }
        }

    }
}

static void handle_compst(ast_node_t *compst) {
    ast_node_t *deflist = ast_2nd_child(compst);
    handle_deflist(deflist, NULL);
}

static void handle_extdef(ast_node_t *extdef) {
    ast_node_t *specifier = ast_1st_child(extdef);
    type_t *spec_type = handle_specifier(specifier);
    ast_node_t *tbd = ast_2nd_child(extdef);
    switch (tbd->node_type) {
    case AST_NODE_ExtDecList:
        {
            ast_node_t *extdeclist = tbd;
            while (1) {
                ast_node_t *vardec = ast_1st_child(extdeclist);
                const char *name;
                type_t *type = handle_vardec(spec_type, vardec, &name);

                if (symbtable_query_entry(name)) {
                    log_semantics_error_prologue("3", vardec->lineno);
                    fprintf(stdout, "Redefined variable \"%s\".\n", name);
                } else symbtable_add_entry(name, SYMB_VAR, type);

                if (ast_onlyone_child(extdeclist)) break;
                extdeclist = ast_3rd_child(extdeclist);
            }
        }
        break;
    case AST_NODE_FunDec:
        {
            ast_node_t *fundec = tbd;
            ast_node_t *id = ast_1st_child(fundec);
            ast_node_t *tbd_ = ast_3rd_child(fundec);
            const char *name = id->attr.identifier_value;
            type_t *func_type = type_new_func(spec_type);
            if (tbd_->node_type == AST_NODE_VarList) {
                ast_node_t *varlist = tbd_;
                while (1) {
                    ast_node_t *paramdec = ast_1st_child(varlist);

                    ast_node_t *param_spec = ast_1st_child(paramdec);
                    ast_node_t *vardec = ast_2nd_child(paramdec);
                    const char *param_name;
                    type_t *param_spec_type = handle_specifier(param_spec);
                    type_t *param_type = handle_vardec(param_spec_type, vardec, &param_name);

                    if (symbtable_query_entry(param_name)) {
                        log_semantics_error_prologue("3", vardec->lineno);
                        fprintf(stdout, "Redefined variable \"%s\".\n", param_name);
                    } else {
                        symbtable_add_entry(param_name, SYMB_VAR, param_type);
                        type_add_func_arg(func_type, param_type);
                    }
                    
                    if (ast_onlyone_child(varlist)) break;
                    ast_node_t *rest = ast_3rd_child(varlist);
                    varlist = rest;
                }
            }
            if (symbtable_query_entry(name)) {
                log_semantics_error_prologue("3", id->lineno);
                fprintf(stdout, "Redefined function \"%s\".\n", name);
            } else symbtable_add_entry(name, SYMB_FUNC, func_type);

        }
        break;
    default:
        break;
    }
}

static void handle_node(ast_node_t *ast_node, int depth) {
    switch (ast_node->node_type) {
    /* case AST_NODE_Def: */
    /* 不直接处理Def而是在Toplevel处理: CompSt/ExtDef */
    case AST_NODE_CompSt:
        handle_compst(ast_node);
        break;
    case AST_NODE_ExtDef:
        handle_extdef(ast_node);
        break;
    default:
        break;
    }
}

static void handle_exp(ast_node_t *exp) {
    /*
        最底层, 无需递归的几个产生式
        1. Exp -> ID
        2. Exp -> ID LP RP
        3. Exp -> INT
        4. Exp -> FLOAT
    */
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_ID}
        };
        if (production_match(exp, recs, 1)) {
            const char *symb = recs[0].ast_node->attr.identifier_value;
            symbtable_entry_t *entry = symbtable_query_entry(symb);
            if (!entry) {
                log_semantics_error_prologue("1", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined variable \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (entry->symb_type != SYMB_VAR) {
                log_semantics_error_prologue("?", recs[0].ast_node->lineno);
                fprintf(stdout, "Not a variable \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            exp->exp_type = entry->type;
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
            const char *symb = recs[0].ast_node->attr.identifier_value;
            symbtable_entry_t *entry = symbtable_query_entry(symb);
            if (!entry) {
                log_semantics_error_prologue("?", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (entry->symb_type != SYMB_FUNC) {
                log_semantics_error_prologue("?", recs[0].ast_node->lineno);
                fprintf(stdout, "Not a function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (!type_func_noarg(entry->type)) {
                log_semantics_error_prologue("9", recs[0].ast_node->lineno);
                fprintf(stdout, "Arglist not matched \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            exp->exp_type = entry->type->return_type;
            return;
        }
    }
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_INT}
        };
        if (production_match(exp, recs, 1)) {
            exp->exp_type = type_new_basic_int();
            return;
        }
    }
    {
        production_rec_t recs[1] = {
            {NULL, AST_NODE_FLOAT}
        };
        if (production_match(exp, recs, 1)) {
            exp->exp_type = type_new_basic_float();
            return;
        }
    }
    /* 需要递归的部分 */
    {
        production_rec_t recs[4] = {
            {NULL, AST_NODE_ID},
            {NULL, AST_NODE_LP},
            {NULL, AST_NODE_Args},
            {NULL, AST_NODE_RP}
        };
        if (production_match(exp, recs, 4)) {
            const char *symb = recs[0].ast_node->attr.identifier_value;
            symbtable_entry_t *entry = symbtable_query_entry(symb);
            if (!entry) {
                log_semantics_error_prologue("?", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (entry->symb_type != SYMB_FUNC) {
                log_semantics_error_prologue("?", recs[0].ast_node->lineno);
                fprintf(stdout, "Not a function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            arglist_t *arglist = entry->type->firstarg;
            ast_node_t *args = recs[2].ast_node;

            while (1) {
                ast_node_t *arg = ast_1st_child(args);
                type_t *arg_type = arg->exp_type;

                if (arglist == NULL) goto arglist_not_matched;
                if (!type_check_equality(arg_type, arglist->type)) {
                    if (arg_type->primitive == PRIM_INVALID) goto handle_exp_failed;
                    goto arglist_not_matched;
                }

                arglist = arglist->nextarg;
                if (ast_onlyone_child(args)) break;
                ast_node_t *rest = ast_3rd_child(args);
                args = rest;
            }
            if (arglist != NULL) goto arglist_not_matched;

            exp->exp_type = entry->type->return_type;
            return;

            arglist_not_matched:
                log_semantics_error_prologue("9", recs[0].ast_node->lineno);
                fprintf(stdout, "Arglist not matched \"%s\".\n", symb);
                goto handle_exp_failed;
        }
    }

handle_exp_failed:
    exp->exp_type = type_new_invalid();
    return;
}

static void handle_node_for_type_checking(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Exp) handle_exp(ast_node);
}

void semantics_check() {
    /* definition checking */
    ast_walk(handle_node, ast_walk_action_nop);
    /* type checking */
    /* 后序遍历, 自下而上为每个exp确定type */
    ast_walk(ast_walk_action_nop, handle_node_for_type_checking);
}
