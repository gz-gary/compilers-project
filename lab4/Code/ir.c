#include "ir.h"
#include <stdlib.h>
#include <string.h>

static int temp_counter = 0;
static int temp_counter2 = 0;

ir_variable_t *ir_get_id_variable(const char *id) {
    ir_variable_t *var = malloc(sizeof(ir_variable_t));
    var->name = id;
    return var;
}

struct ir_variable_t* ir_get_ref_variable(struct ir_variable_t *var) {
    ir_variable_t *ref = malloc(sizeof(ir_variable_t));
    int len = snprintf(NULL, 0, "&%s", var->name) + 1;
    char *name = calloc(len, sizeof(char));
    sprintf(name, "&%s", var->name);
    ref->name = name;
    return ref;
}

ir_variable_t *ir_get_int_variable(int i) {
    ir_variable_t *var = malloc(sizeof(ir_variable_t));
    int len = snprintf(NULL, 0, "#%d", i) + 1;
    char *name = calloc(len, sizeof(char));
    sprintf(name, "#%d", i);
    var->name = name;
    return var;
}

ir_variable_t *ir_get_float_variable(float f) {
    ir_variable_t *var = malloc(sizeof(ir_variable_t));
    int len = snprintf(NULL, 0, "#%.6f", f) + 1;
    char *name = calloc(len, sizeof(char));
    sprintf(name, "#%.6f", f);
    var->name = name;
    return var;
}

ir_variable_t *ir_new_temp_variable() {
    ir_variable_t *var = malloc(sizeof(ir_variable_t));
    int len = snprintf(NULL, 0, "t%d", temp_counter) + 1;
    char *name = calloc(len, sizeof(char));
    sprintf(name, "t%d", temp_counter);
    var->name = name;
    temp_counter++;
    return var;
}

ir_code_block_t *ir_concat_code_block(ir_code_block_t *block1, ir_code_block_t *block2) {
    if (block1->first == NULL) return block2;
    if (block2->first != NULL) {
        block2->first->prev = block1->last;
        block1->last->next = block2->first;
        block1->last = block2->last;
    }
    // free(block2);
    return block1;
}

ir_code_block_t *ir_append_code(ir_code_block_t *block, ir_code_t *code) {
    if (block->first == NULL) {
        block->first = block->last = code;
    } else {
        code->prev = block->last;
        block->last->next = code;
        block->last = code;
    }
    return block;
}

void ir_remove_last_code(ir_code_block_t *block) {
    ir_code_t *code = block->last;
    if (code) {
        if (code->prev) code->prev->next = NULL;
        block->last = code->prev;
        code->prev = NULL;
        // free(code);
    }
}

ir_code_block_t *ir_new_code_block() {
    ir_code_block_t *block = malloc(sizeof(ir_code_block_t));
    block->first = block->last = NULL;
    return block;
}

ir_code_t *ir_new_code_op(ir_variable_t *result, ir_variable_t *op1, ir_variable_t *op2, const char *op_name) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_OP;
    code->result = result;
    code->op1 = op1;
    code->op2 = op2;
    code->op_name = op_name;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_fundec(const char *fun_name) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_FUNDEC;
    code->fun_name = fun_name;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_return(ir_variable_t *ret_var) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_RETURN;
    code->ret_var = ret_var;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_dec(const char *dec_name, int dec_size) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_DEC;
    code->dec_name = dec_name;
    code->dec_size = dec_size;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_label() {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_LABEL;

    int len = snprintf(NULL, 0, "label%d", temp_counter2) + 1;
    char *name = calloc(len, sizeof(char));
    sprintf(name, "label%d", temp_counter2);
    code->label_name = name;
    temp_counter2++;

    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_goto(ir_code_t *goto_dest) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_GOTO;
    code->goto_dest = goto_dest;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_relop_goto(ir_variable_t *relop1, ir_variable_t *relop2, const char *relop_name, ir_code_t *relop_goto_dest) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_RELOP_GOTO;
    code->relop1 = relop1;
    code->relop2 = relop2;
    code->relop_name = relop_name;
    code->relop_goto_dest = relop_goto_dest;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_call(ir_variable_t *call_result, const char *call_name) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_CALL;
    code->call_result = call_result;
    code->call_name = call_name;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_arg(ir_variable_t *arg_var, int arg_ref) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_ARG;
    code->arg_var = arg_var;
    code->arg_ref = arg_ref;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_param(ir_variable_t *param_var) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_PARAM;
    code->param_var = param_var;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_read(ir_variable_t *read_var) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_READ;
    code->read_var = read_var;
    code->prev = code->next = NULL;
    return code;
}

ir_code_t *ir_new_code_write(ir_variable_t *write_var) {
    ir_code_t *code = malloc(sizeof(ir_code_t));
    code->type = IR_WRITE;
    code->write_var = write_var;
    code->prev = code->next = NULL;
    return code;
}

void ir_dump(FILE *file, ir_code_block_t *block) {
    ir_code_t *code = block->first;
    while (code != NULL) {
        switch (code->type) {
        case IR_OP:
            {
                if (!strcmp(code->op_name, "ASSIGN"))
                    fprintf(file, "%s := %s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "AND"))
                    fprintf(file, "%s := %s && %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "OR"))
                    fprintf(file, "%s := %s || %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "PLUS"))
                    fprintf(file, "%s := %s + %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "MINUS"))
                    fprintf(file, "%s := %s - %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "MULT"))
                    fprintf(file, "%s := %s * %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "DIV"))
                    fprintf(file, "%s := %s / %s\n", code->result->name, code->op1->name, code->op2->name);
                else if (!strcmp(code->op_name, "NEG"))
                    fprintf(file, "%s := #0 - %s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "POS"))
                    fprintf(file, "%s := #0 + %s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "NOT"))
                    fprintf(file, "%s := !%s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "DEREF_R"))
                    fprintf(file, "%s := *%s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "DEREF_L"))
                    fprintf(file, "*%s := %s\n", code->result->name, code->op1->name);
                else if (!strcmp(code->op_name, "DEREF_LR"))
                    fprintf(file, "*%s := *%s\n", code->result->name, code->op1->name);
            }
            break;
        case IR_FUNDEC:
            fprintf(file, "FUNCTION %s :\n", code->fun_name);
            break;
        case IR_RETURN:
            fprintf(file, "RETURN %s\n", code->ret_var->name);
            break;
        case IR_DEC:
            fprintf(file, "DEC %s %d\n", code->dec_name, code->dec_size);
            break;
        case IR_LABEL:
            fprintf(file, "LABEL %s :\n", code->label_name);
            break;
        case IR_GOTO:
            fprintf(file, "GOTO %s\n", code->goto_dest->label_name);
            break;
        case IR_RELOP_GOTO:
            fprintf(file, "IF %s %s %s GOTO %s\n", code->relop1->name, code->relop_name, code->relop2->name, code->relop_goto_dest->label_name);
            break;
        case IR_CALL:
            fprintf(file, "%s := CALL %s\n", code->call_result->name, code->call_name);
            break;
        case IR_ARG:
            fprintf(file, "ARG %s%s\n", code->arg_ref ? "&" : "", code->arg_var->name);
            break;
        case IR_PARAM:
            fprintf(file, "PARAM %s\n", code->param_var->name);
            break;
        case IR_READ:
            fprintf(file, "READ %s\n", code->read_var->name);
            break;
        case IR_WRITE:
            fprintf(file, "WRITE %s\n", code->write_var->name);
            break;
        default:
            break;
        }
        code = code->next;
    }
}
