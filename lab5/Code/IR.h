#ifndef __IR_H__
#define __IR_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
  IR_OP_ADD,
  IR_OP_SUB,
  IR_OP_MUL,
  IR_OP_DIV,
} IR_OP_TYPE;

typedef enum {
  IR_RELOP_EQ,
  IR_RELOP_NE,
  IR_RELOP_GT,
  IR_RELOP_GE,
  IR_RELOP_LT,
  IR_RELOP_LE
} IR_RELOP_TYPE;

typedef unsigned IR_var;   // 数字表示 v1, v2...
typedef unsigned IR_label; // 数字表示 L1, L2...
typedef unsigned IR_DEC_size_t;
enum {IR_VAR_NONE = 0};
enum {IR_LABEL_NONE = 0};
typedef struct IR_val {
    bool is_const;
    union {
        IR_var var;
        int const_val;
    };
} IR_val;

#endif // __IR_H__