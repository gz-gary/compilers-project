#include "semantics.h"
#include "symbtable.h"
#include "production.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

extern int semantics_error;

static int scope_depth = 0;

static type_t* handle_specifier(ast_node_t *specifier);

static void handle_compst(ast_node_t *compst);

static type_t *handle_vardec(type_t *specifier_type, ast_node_t *vardec, const char **name) {
    ast_node_t *first_child = ast_1st_child(vardec);
    if (first_child->node_type == AST_NODE_ID) {
        *name = first_child->attr.identifier_value;
        return specifier_type;
    } else {
        return handle_vardec(type_new_array(specifier_type, ast_3rd_child(vardec)->attr.int_value), first_child, name);
    }
}

static void handle_deflist(ast_node_t *deflist, type_t *upper_struct) {
    while (!production_epsilon(deflist)) {
        ast_node_t *def = ast_1st_child(deflist);
        ast_node_t *rest = ast_2nd_child(deflist);
        /*
            Def -> Specifier DecList SEMI
        */
        ast_node_t *def_spec = ast_1st_child(def);
        ast_node_t *declist = ast_2nd_child(def);
        type_t *spec_type = handle_specifier(def_spec);
        while (1) {
            /*
                DecList -> Dec
                | Dec COMMA DecList
                Dec -> VarDec
                | VarDec ASSIGNOP Exp
            */
            ast_node_t *dec = ast_1st_child(declist);
            ast_node_t *vardec = ast_1st_child(dec);

            const char *name;
            type_t *type = handle_vardec(spec_type, vardec, &name);
            if (type->primitive == PRIM_ARRAY && type->elem_type->primitive == PRIM_ARRAY) {
                semantics_error = 1;
            }
            if (!upper_struct && (type->primitive == PRIM_ARRAY || type->primitive == PRIM_STRUCT)) {
                struct ir_dec_t *temp = malloc(sizeof(struct ir_dec_t));
                temp->name = name;
                temp->size = type->size_in_bytes;
                temp->next = deflist->ir_dec_head;
                deflist->ir_dec_head = temp;
                vardec->ir_dec_head = temp;
            }
            // redefine check
            symbtable_entry_t *entry = symbtable_query_entry(name);
            if (upper_struct) {
                if (entry) {
                    log_semantics_error_prologue("15", vardec->lineno);
                    fprintf(stdout, "Redefined field \"%s\".\n", name);
                } else symbtable_add_entry(name, SYMB_FIELD, type, scope_depth);
                if (!ast_onlyone_child(dec)) {
                    /* try to initialize fields */
                    log_semantics_error_prologue("15", vardec->lineno);
                    fprintf(stdout, "Try to initialize fields \"%s\".\n", name);
                }
                type_add_struct_field(upper_struct, type, name);
            } else {
                if (entry && (entry->symb_type != SYMB_VAR || entry->depth == scope_depth)) {
                    log_semantics_error_prologue("3", vardec->lineno);
                    fprintf(stdout, "Redefined variable \"%s\".\n", name);
                } else {
                    dec->symb_to_del = 1;
                    symbtable_add_entry(name, SYMB_VAR, type, scope_depth);
                }
            }
            
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
                {NULL, AST_NODE_OptTag}, // 可能是epsilon，匿名结构体
                {NULL, AST_NODE_LC},
                {NULL, AST_NODE_DefList},
                {NULL, AST_NODE_RC},
            };
            if (production_match(first_child, recs, 5)) {
                const char *symb = production_epsilon(recs[1].ast_node) ?
                    NULL :
                    ast_1st_child(recs[1].ast_node)->attr.identifier_value;

                ast_node_t *deflist = recs[3].ast_node;
                type_t *result = type_new_struct(symb);
                handle_deflist(deflist, result);
                if (symb) {
                    symbtable_entry_t *entry = symbtable_query_entry(symb);
                    if (entry) {
                        log_semantics_error_prologue("16", (recs[1].ast_node)->lineno);
                        fprintf(stdout, "Redefined structure \"%s\".\n", symb);
                    } else symbtable_add_entry(symb, SYMB_STRUCT, result, 0);
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

static void clean_deflist(ast_node_t *deflist) {
    while (!production_epsilon(deflist)) {
        ast_node_t *def = ast_1st_child(deflist);
        ast_node_t *rest = ast_2nd_child(deflist);
        ast_node_t *declist = ast_2nd_child(def);
        while (1) {
            ast_node_t *dec = ast_1st_child(declist);
            ast_node_t *vardec = ast_1st_child(dec);

            while (!ast_onlyone_child(vardec)) vardec = ast_1st_child(vardec);
            const char *name = ast_1st_child(vardec)->attr.identifier_value;
            if (dec->symb_to_del) {
                dec->symb_to_del = 0;
                symbtable_remove_head(name);
            }

            if (ast_onlyone_child(declist)) break;
            ast_node_t *rest = ast_3rd_child(declist);
            declist = rest;
        }

        deflist = rest;
    }
}

static void clean_varlist(ast_node_t *varlist) {
    while (1) {
        ast_node_t *paramdec = ast_1st_child(varlist);
        ast_node_t *vardec = ast_2nd_child(paramdec);

        while (!ast_onlyone_child(vardec)) vardec = ast_1st_child(vardec);
        const char *param_name = ast_1st_child(vardec)->attr.identifier_value;
        if (paramdec->symb_to_del) {
            paramdec->symb_to_del = 0;
            symbtable_remove_head(param_name);
        }

        if (ast_onlyone_child(varlist)) break;
        ast_node_t *rest = ast_3rd_child(varlist);
        varlist = rest;
    }
}

static void handle_extdef(ast_node_t *extdef) {
    ast_node_t *specifier = ast_1st_child(extdef);
    type_t *spec_type = handle_specifier(specifier);
    ast_node_t *tbd = ast_2nd_child(extdef);
    switch (tbd->node_type) {
    /*
        ExtDef -> Specifier ExtDecList SEMI
        | Specifier FunDec CompSt 
        | Specifier SEMI
    */
    case AST_NODE_ExtDecList:
        {
            ast_node_t *extdeclist = tbd;
            while (1) {
                ast_node_t *vardec = ast_1st_child(extdeclist);
                const char *name;
                type_t *type = handle_vardec(spec_type, vardec, &name);
                if (type->primitive == PRIM_ARRAY && type->elem_type->primitive == PRIM_ARRAY) {
                    semantics_error = 1;
                }
                // redefine check
                if (symbtable_query_entry(name)) {
                    log_semantics_error_prologue("3", vardec->lineno);
                    fprintf(stdout, "Redefined variable \"%s\".\n", name);
                } else symbtable_add_entry(name, SYMB_VAR, type, scope_depth);
                /*
                    ExtDecList -> VarDec
                    | ExtDecList COMMA ExtDecList
                */
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
            /*
                FunDec -> ID LP VarList RP
                | ID LP RP
            */
            if (tbd_->node_type == AST_NODE_VarList) {
                ast_node_t *varlist = tbd_;
                while (1) {
                    ast_node_t *paramdec = ast_1st_child(varlist);

                    ast_node_t *param_spec = ast_1st_child(paramdec);
                    ast_node_t *vardec = ast_2nd_child(paramdec);
                    const char *param_name;
                    type_t *param_spec_type = handle_specifier(param_spec);
                    type_t *param_type = handle_vardec(param_spec_type, vardec, &param_name);
                    // check array type, 不允许一维数组
                    if (param_type->primitive == PRIM_ARRAY) {
                        semantics_error = 1;
                    }

                    {
                        struct ir_dec_t *temp = malloc(sizeof(struct ir_dec_t));
                        temp->name = param_name;
                        temp->next = fundec->ir_dec_head;
                        fundec->ir_dec_head = temp;
                    }

                    if (symbtable_query_entry(param_name)) {
                        log_semantics_error_prologue("3", vardec->lineno);
                        fprintf(stdout, "Redefined variable \"%s\".\n", param_name);
                    } else {
                        paramdec->symb_to_del = 1;
                        symbtable_add_entry(param_name, SYMB_VAR, param_type, scope_depth + 1);
                        type_add_func_arg(func_type, param_type);
                    }
                    /*
                        Varlist -> ParamDec COMMA VarList
                        | ParamDec
                    */
                    if (ast_onlyone_child(varlist)) break;
                    ast_node_t *rest = ast_3rd_child(varlist);
                    varlist = rest;
                }
            }
            if (symbtable_query_entry(name)) {
                log_semantics_error_prologue("4", id->lineno);
                fprintf(stdout, "Redefined function \"%s\".\n", name);
            } else symbtable_add_entry(name, SYMB_FUNC, func_type, scope_depth);

        }
        break;
    default:
        break;
    }
}

static void handle_node_preorder(ast_node_t *ast_node, int depth) {
    switch (ast_node->node_type) {
    /* case AST_NODE_Def: */
    /* 不直接处理Def而是在Toplevel处理: CompSt/ExtDef */
    case AST_NODE_CompSt:
        ++scope_depth;
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
                log_semantics_error_prologue("2", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (entry->symb_type != SYMB_FUNC) {
                log_semantics_error_prologue("11", recs[0].ast_node->lineno);
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
                log_semantics_error_prologue("2", recs[0].ast_node->lineno);
                fprintf(stdout, "Undefined function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            if (entry->symb_type != SYMB_FUNC) {
                log_semantics_error_prologue("11", recs[0].ast_node->lineno);
                fprintf(stdout, "Not a function \"%s\".\n", symb);
                goto handle_exp_failed;
            }
            /*
                Args -> Exp COMMA Args
                | Exp
            */
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
    {
        production_rec_t recs[3] = {
            {NULL, AST_NODE_Exp},
            {NULL, AST_NODE_ASSIGNOP},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 3)) {
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!production_is_leftvalue_exp(exp1)) {
                log_semantics_error_prologue("6", exp1->lineno);
                fprintf(stdout, "Assign to a not-left value.\n");
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type)) {
                log_semantics_error_prologue("5", exp1->lineno);
                fprintf(stdout, "Incompatible type about \"=\".\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_int(exp1->exp_type)) {
                log_semantics_error_prologue("7", exp1->lineno);
                if (exp1->attr.identifier_value != NULL) {
                    fprintf(stdout, "Left expression on the \"&&\" not boolean.\n");
                } else {
                    fprintf(stdout, "Left expression on the \"&&\" not boolean.\n");
                }
                goto handle_exp_failed;
            }
            if (!type_check_int(exp2->exp_type)) {
                log_semantics_error_prologue("7", exp2->lineno);
                fprintf(stdout, "Right expression on the \"&&\" not boolean.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = type_new_basic_int();
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_int(exp1->exp_type)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Left expression on the \"||\" not boolean.\n");
                goto handle_exp_failed;
            }
            if (!type_check_int(exp2->exp_type)) {
                log_semantics_error_prologue("7", exp2->lineno);
                fprintf(stdout, "Right expressionon the \"||\" not boolean.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = type_new_basic_int();
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type) || !(exp1->exp_type->primitive == PRIM_BASIC)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Incompatible type about relop.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = type_new_basic_int();
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type) || !(exp1->exp_type->primitive == PRIM_BASIC)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Incompatible type about \"+\".\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type) || !(exp1->exp_type->primitive == PRIM_BASIC)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Incompatible type about \"-\".\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type) || !(exp1->exp_type->primitive == PRIM_BASIC)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Incompatible type about \"*\".\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_equality(exp1->exp_type, exp2->exp_type) || !(exp1->exp_type->primitive == PRIM_BASIC)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Incompatible type about \"/\".\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
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
            ast_node_t *exp1 = recs[1].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_MINUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            ast_node_t *exp1 = recs[1].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (exp1->exp_type->primitive != PRIM_BASIC) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Expression can't be negative.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_PLUS},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            ast_node_t *exp1 = recs[1].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (exp1->exp_type->primitive != PRIM_BASIC) {
                fprintf(stdout, "%d\n", exp1->exp_type->primitive);
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Expression can't be positive.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type;
            return;
        }
    }
    {
        production_rec_t recs[2] = {
            {NULL, AST_NODE_NOT},
            {NULL, AST_NODE_Exp}
        };
        if (production_match(exp, recs, 2)) {
            ast_node_t *exp1 = recs[1].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (!type_check_int(exp1->exp_type)) {
                log_semantics_error_prologue("7", exp1->lineno);
                fprintf(stdout, "Right expression on the \"!\" not boolean.\n");
                goto handle_exp_failed;
            }
            exp->exp_type = type_new_basic_int();
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
            ast_node_t *exp1 = recs[0].ast_node;
            ast_node_t *exp2 = recs[2].ast_node;
            if (exp1->exp_type->primitive == PRIM_INVALID || exp2->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (exp1->exp_type->primitive != PRIM_ARRAY) {
                log_semantics_error_prologue("10", exp1->lineno);
                fprintf(stdout, "Not an array.\n");
                goto handle_exp_failed;
            }
            if (!type_check_int(exp2->exp_type)) {
                log_semantics_error_prologue("12", exp2->lineno);
                fprintf(stdout, "Not an integer\n");
                goto handle_exp_failed;
            }
            exp->exp_type = exp1->exp_type->elem_type;
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
            ast_node_t *exp1 = recs[0].ast_node;
            const char *field_name = recs[2].ast_node->attr.identifier_value;
            if (exp1->exp_type->primitive == PRIM_INVALID) {
                goto handle_exp_failed;
            }
            if (exp1->exp_type->primitive != PRIM_STRUCT) {
                log_semantics_error_prologue("13", exp1->lineno);
                fprintf(stdout, "Not a struct.\n");
                goto handle_exp_failed;
            }
            field_t *field = type_query_struct_field(exp1->exp_type, field_name);
            if (!field) {
                log_semantics_error_prologue("14", exp1->lineno);
                fprintf(stdout, "No such field \"%s\".\n", field_name);
                goto handle_exp_failed;
            }
            exp->exp_type = field->type;
            return;
        }
    }
    
    

handle_exp_failed:
    exp->exp_type = type_new_invalid();
    return;
}

static void handle_node_postorder(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Exp) handle_exp(ast_node);
    if (ast_node->node_type == AST_NODE_CompSt) {
        --scope_depth;
        clean_deflist(ast_2nd_child(ast_node));
    }
    if (ast_node->node_type == AST_NODE_ExtDef) {
        ast_node_t *fundec = ast_2nd_child(ast_node);
        if (fundec->node_type == AST_NODE_FunDec) {
            ast_node_t *varlist = ast_3rd_child(fundec);
            if (varlist->node_type == AST_NODE_VarList) clean_varlist(varlist);
        }
    }
}

static type_t *current_return_type = NULL;
static void handle_node_for_return_checking(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_FunDec) {
        ast_node_t *id = ast_1st_child(ast_node);
        symbtable_entry_t *entry = symbtable_query_entry(id->attr.identifier_value);
        if (!entry) goto invalid_fundec;
        if (entry->symb_type != SYMB_FUNC) goto invalid_fundec;
        current_return_type = entry->type->return_type;
        return;

invalid_fundec:
        current_return_type = NULL;
        return;
    }
    if (ast_node->node_type == AST_NODE_Stmt) {
        ast_node_t *ret = ast_1st_child(ast_node);
        if (ret->node_type == AST_NODE_RETURN) {
            if (current_return_type == NULL) return;
            type_t *return_type = ast_2nd_child(ast_node)->exp_type;
            if (return_type->primitive == PRIM_INVALID) return;
            if (current_return_type->primitive == PRIM_INVALID) return;
            if (!type_check_equality(return_type, current_return_type)) {
                log_semantics_error_prologue("8", ret->lineno);
                fprintf(stdout, "Return type does not match with function declaration.\n");
            }
            return;
        }
    }
}

static void handle_node_for_condition_checking(ast_node_t *ast_node, int depth) {
    if (ast_node->node_type == AST_NODE_Stmt) {
        ast_node_t *tbd = ast_1st_child(ast_node);
        if (tbd->node_type == AST_NODE_IF || tbd->node_type == AST_NODE_WHILE) {
            ast_node_t *exp = ast_3rd_child(ast_node);
            if (exp->exp_type->primitive == PRIM_INVALID) return;
            if (!type_check_int(exp->exp_type)) {
                log_semantics_error_prologue("7", ast_node->lineno);
                fprintf(stdout, "Incompatible type in if/while.\n");
            }
        }
    }
}

void semantics_check() {
    {
        symbtable_add_entry("read", SYMB_FUNC, type_new_func(type_new_basic_int()), 0);
        type_t *temp = type_new_func(type_new_basic_int());
        type_add_func_arg(temp, type_new_basic_int());
        symbtable_add_entry("write", SYMB_FUNC, temp, 0);
    }
    /* definition checking and type checking */
    ast_walk(handle_node_preorder, handle_node_postorder);
    /* return type checking */
    ast_walk(handle_node_for_return_checking, ast_walk_action_nop);
    /* if and while condition checking */
    ast_walk(handle_node_for_condition_checking, ast_walk_action_nop);
}
