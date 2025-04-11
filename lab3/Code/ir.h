#ifndef IR_H
#define IR_H

#include <stdio.h>

typedef struct ir_variable_t ir_variable_t;
typedef struct ir_code_t ir_code_t;
typedef struct ir_code_block_t ir_code_block_t;

struct ir_variable_t {
    const char *name;
};

enum ir_code_type_t {
    IR_OP,
};

struct ir_code_t {
    enum ir_code_type_t type;
    struct ir_code_t* next;
    union {
        struct {
            struct ir_variable_t* result;
            struct ir_variable_t* op1;
            struct ir_variable_t* op2;
            const char *op_name;
        };
    };
};

struct ir_code_block_t {
    struct ir_code_t *first, *last;
};

struct ir_variable_t* ir_get_id_variable(const char *id);
struct ir_variable_t* ir_get_int_variable(int i);
struct ir_variable_t* ir_get_float_variable(float f);
struct ir_variable_t* ir_new_temp_variable();
struct ir_code_block_t* ir_concat_code_block(struct ir_code_block_t *block1, struct ir_code_block_t *block2);
struct ir_code_block_t* ir_append_code(struct ir_code_block_t *block, struct ir_code_t* code);
struct ir_code_block_t* ir_new_code_block();
struct ir_code_t* ir_new_code_op(
    struct ir_variable_t* result,
    struct ir_variable_t* op1,
    struct ir_variable_t* op2,
    const char *op_name
);

void ir_dump(FILE* file, struct ir_code_block_t *block);

#endif