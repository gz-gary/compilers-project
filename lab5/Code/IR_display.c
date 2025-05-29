//
// Created by hby on 22-11-8.
//

#include "IR.h"
#include <stdio.h>
#include <assert.h>

//// =============================== IR print ===============================

void IR_block_print(IR_block *block, FILE *out) {
    if(block->label != IR_LABEL_NONE)
        fprintf(out, "LABEL L%u :\n", block->label);
    for_list(IR_stmt_ptr, i, block->stmts)
        VCALL(*i->val, print, out);
}

// #define DEBUG

#ifdef DEBUG
void IR_block_println(IR_block *block, FILE *out) {
    IR_block_print(block, out);
    fprintf(out, "-----------------\n");
}
#endif

void IR_function_print(IR_function *func, FILE *out) {
    assert(func != NULL);
    assert(func->func_name != NULL);
    fprintf(out, "FUNCTION %s :\n", func->func_name);
    for_vec(IR_var, var, func->params)
        fprintf(out, "PARAM v%u\n", *var);

#ifdef DEBUG
    fprintf(out, "Map-----------------\n");
#endif
    for_map(IR_var, IR_Dec, it, func->map_dec) {
        fprintf(out, "DEC v%u %u\n", it->key, it->val.dec_size);
        fprintf(out, "v%u := &v%u\n", it->val.dec_addr, it->key);
    }

#ifdef DEBUG
    fprintf(out, "BBs-----------------\n");
    for_list(IR_block_ptr, i, func->blocks)
        IR_block_println(i->val, out);
#else
    for_list(IR_block_ptr, i, func->blocks)
        IR_block_print(i->val, out);
#endif

    fprintf(out, "\n");
}

void IR_program_print(IR_program *program, FILE *out) {
    for_vec(IR_function *, func_ptr_ptr, program->functions) {
        IR_function *func = *func_ptr_ptr;
        IR_function_print(func, out);
    }
}
