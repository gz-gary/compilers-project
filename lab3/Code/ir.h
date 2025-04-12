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
    IR_FUNDEC,
    IR_RETURN,
    IR_DEC,
    IR_LABEL,
    IR_GOTO,
    IR_RELOP_GOTO,
    IR_CALL,
    IR_ARG,
    IR_PARAM,
    IR_READ,
    IR_WRITE
};

struct ir_code_t {
    enum ir_code_type_t type;
    struct ir_code_t* prev;
    struct ir_code_t* next;
    union {
        struct {
            struct ir_variable_t* result;
            struct ir_variable_t* op1;
            struct ir_variable_t* op2;
            const char *op_name;
        };
        struct {
            const char *fun_name;
        };
        struct {
            struct ir_variable_t* ret_var;
        };
        struct {
            const char *dec_name;
            int dec_size;
        };
        struct {
            const char *label_name;
        };
        struct {
            struct ir_code_t *goto_dest;
        };
        struct {
            struct ir_variable_t* relop1;
            struct ir_variable_t* relop2;
            const char *relop_name;
            struct ir_code_t *relop_goto_dest;
        };
        struct {
            struct ir_variable_t* call_result;
            const char *call_name;
        };
        struct {
            struct ir_variable_t* arg_var;
        };
        struct {
            struct ir_variable_t* param_var;
        };
        struct {
            struct ir_variable_t* read_var;
        };
        struct {
            struct ir_variable_t* write_var;
        };
    };
};

struct ir_code_block_t {
    struct ir_code_t *first, *last;
};


struct ir_dec_t {
    const char *name;
    int size;
    struct ir_dec_t *next;
};

struct ir_variable_t* ir_get_id_variable(const char *id);
struct ir_variable_t* ir_get_ref_variable(struct ir_variable_t *var);
struct ir_variable_t* ir_get_int_variable(int i);
struct ir_variable_t* ir_get_float_variable(float f);
struct ir_variable_t* ir_new_temp_variable();
struct ir_code_block_t* ir_concat_code_block(struct ir_code_block_t *block1, struct ir_code_block_t *block2);
struct ir_code_block_t* ir_append_code(struct ir_code_block_t *block, struct ir_code_t* code);
void ir_remove_last_code(struct ir_code_block_t *block);
struct ir_code_block_t* ir_new_code_block();
struct ir_code_t* ir_new_code_op(
    struct ir_variable_t* result,
    struct ir_variable_t* op1,
    struct ir_variable_t* op2,
    const char *op_name
);
struct ir_code_t* ir_new_code_fundec(const char *fun_name);
struct ir_code_t* ir_new_code_return(struct ir_variable_t *ret_var);
struct ir_code_t* ir_new_code_dec(const char *dec_name, int dec_size);
struct ir_code_t* ir_new_code_label();
struct ir_code_t* ir_new_code_goto(struct ir_code_t *goto_dest);
struct ir_code_t* ir_new_code_relop_goto(
    struct ir_variable_t* relop1,
    struct ir_variable_t* relop2,
    const char *relop_name,
    struct ir_code_t *relop_goto_dest
);
struct ir_code_t* ir_new_code_call(struct ir_variable_t* call_result, const char *call_name);
struct ir_code_t* ir_new_code_arg(struct ir_variable_t* arg_var);
struct ir_code_t* ir_new_code_param(struct ir_variable_t* param_var);
struct ir_code_t* ir_new_code_read(struct ir_variable_t* read_var);
struct ir_code_t* ir_new_code_write(struct ir_variable_t* write_var);

void ir_dump(FILE* file, struct ir_code_block_t *block);

#endif