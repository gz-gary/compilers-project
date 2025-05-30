#ifndef __DAG_H__
#define __DAG_H__

#include "IR.h"

typedef enum {
  DAG_VAR,    // 变量
  DAG_CONST,  // 常量
  DAG_BINARY, // 二元操作
} DAG_node_type;

typedef struct DAG_node_t {
  DAG_node_type type; // 节点类型
  union {
    IR_var var;    // 变量id
    int const_val; // 常量值
    struct {
      IR_OP_TYPE op;                // 操作符
      struct DAG_node_t *lhs, *rhs; // 左右子节点
    } binary;                       // 二元操作类型
  } data;

  // 在"IR.h"中已定义Vector
  Vec_IR_var defs; // 定义的变量列表
  bool visited;    // DAG->IR

  bool is_const_folded; // 是否常量折叠
  int folded_value;     // 折叠后的值
} DAG_node_t, *DAG_node_ptr;

DEF_MAP(IR_var, DAG_node_ptr) // 变量->DAG节点

// 表达式查重表，用于公共子表达式消除
typedef struct {
  IR_OP_TYPE op;
  DAG_node_t *lhs, *rhs; // 左右子节点
} DAG_expr_t;

extern int DAG_expr_cmp(const DAG_expr_t a, const DAG_expr_t b);
DEF_MAP_CMP(DAG_expr_t, DAG_node_ptr, DAG_expr_cmp)

void build_DAG_from_stmt(IR_stmt *stmt, Map_IR_var_DAG_node_ptr *var2node,
                         Map_DAG_expr_t_DAG_node_ptr *expr2node);
void generate_IR_from_DAG(IR_block *blk, Map_IR_var_DAG_node_ptr *var2node);

#endif // __DAG_H__