#include "DAG.h"
#include <assert.h>

// TODO: 这里op_stmt->rs1是IR_val
static DAG_node_ptr get_var_node(IR_var var,
                                 Map_IR_var_DAG_node_ptr *var2node) {
  if (Map_IR_var_DAG_node_ptr_exist(var2node, var)) {
    return Map_IR_var_DAG_node_ptr_get(var2node, var);
  } else {
    // 先malloc分配内存，再设置其中的内容，参考IR.c中IR_function_init
    DAG_node_ptr node = (DAG_node_ptr)malloc(sizeof(DAG_node_t));
    node->type = DAG_VAR;
    node->data.var = var;
    Vec_IR_var_init(&node->defs);
    node->visited = false;
    node->is_const_folded = false;
    node->folded_value = 0; // 初始值为0，后续可能会被常量折叠更新
    Map_IR_var_DAG_node_ptr_insert(var2node, var, node);
    return node;
  }
}

static DAG_node_ptr get_const_node(int const_val) {
  // 常量节点不需要存储在Map中，因为常量值是唯一的
  DAG_node_ptr node = (DAG_node_ptr)malloc(sizeof(DAG_node_t));
  node->type = DAG_CONST;
  node->data.const_val = const_val;
  Vec_IR_var_init(&node->defs);
  node->visited = false;
  node->is_const_folded = true;   // 常量节点默认是常量折叠的
  node->folded_value = const_val; // 折叠后的值就是常量值本身
  return node;
}

static DAG_node_ptr get_val_node(IR_val val,
                                 Map_IR_var_DAG_node_ptr *var2node) {
  if (val.is_const) {
    return get_const_node(val.const_val);
  } else {
    return get_var_node(val.var, var2node);
  }
}

static DAG_node_ptr get_binary_node(IR_OP_TYPE op, DAG_node_ptr lhs,
                                    DAG_node_ptr rhs,
                                    Map_DAG_expr_t_DAG_node_ptr *expr2node) {
  DAG_expr_t expr = {op, lhs, rhs};
  if (Map_DAG_expr_t_DAG_node_ptr_exist(expr2node, expr)) {
    // 自动消除公共子表达式
    return Map_DAG_expr_t_DAG_node_ptr_get(expr2node, expr);
  } else {
    // 先malloc分配内存，再设置其中的内容，参考IR.c中IR_function_init
    DAG_node_ptr node = (DAG_node_ptr)malloc(sizeof(DAG_node_t));
    node->type = DAG_BINARY;
    node->data.binary.op = op;
    node->data.binary.lhs = lhs;
    node->data.binary.rhs = rhs;
    Vec_IR_var_init(&node->defs);
    node->visited = false;
    node->is_const_folded = false; // 初始状态不是常量折叠
    node->folded_value = 0;        // 初始值为0，后续可能会被常量折叠更新

    // 常量折叠
    // if (lhs->is_const_folded && rhs->is_const_folded) {
    //   node->is_const_folded = true;
    //   switch (op) {
    //   case IR_OP_ADD:
    //     node->folded_value = lhs->folded_value + rhs->folded_value;
    //     break;
    //   case IR_OP_SUB:
    //     node->folded_value = lhs->folded_value - rhs->folded_value;
    //     break;
    //   case IR_OP_MUL:
    //     node->folded_value = lhs->folded_value * rhs->folded_value;
    //     break;
    //   case IR_OP_DIV:
    //     assert(rhs->folded_value != 0); // 除数不能为0
    //     node->folded_value = lhs->folded_value / rhs->folded_value;
    //     break;
    //   default:
    //     assert(0); // 不支持的操作符
    //   }
    // }
    Map_DAG_expr_t_DAG_node_ptr_insert(expr2node, expr, node);
    return node;
  }
}

void build_DAG_from_stmt(IR_stmt *stmt, Map_IR_var_DAG_node_ptr *var2node,
                         Map_DAG_expr_t_DAG_node_ptr *expr2node) {

  switch (stmt->stmt_type) {
  case IR_OP_STMT: {
    IR_op_stmt *op_stmt = (IR_op_stmt *)stmt;
    IR_var dst = op_stmt->rd;

    DAG_node_ptr lhs = get_val_node(op_stmt->rs1, var2node);
    DAG_node_ptr rhs = get_val_node(op_stmt->rs2, var2node);

    DAG_node_ptr expr_node = get_binary_node(op_stmt->op, lhs, rhs, expr2node);

    VCALL(expr_node->defs, push_back, dst); // 将dst添加到expr_node的defs中
    // 添加dst->expr_node的映射
    Map_IR_var_DAG_node_ptr_insert(var2node, dst, expr_node);
    break;
  }
  case IR_ASSIGN_STMT: {
    break;
  }
  case IR_LOAD_STMT: {
    break;
  }
  case IR_STORE_STMT: {
    break;
  }
  case IR_CALL_STMT: {
    break;
  }
  case IR_IF_STMT: {
    break;
  }
  case IR_GOTO_STMT: {
    break;
  }
  case IR_RETURN_STMT: {
    break;
  }
  case IR_READ_STMT: {
    break;
  }
  case IR_WRITE_STMT: {
    break;
  }
  default:
    printf("不支持的stmt类型: %d\n", stmt->stmt_type);
    assert(0); // 不支持的stmt类型
  }
}

static void block_append_IR_op_stmt(IR_block *blk, DAG_node_ptr node,
                                    IR_var rd) {
  // (IR_OP_TYPE op, IR_var rd, IR_val rs1, IR_val rs2)
  IR_OP_TYPE op = node->data.binary.op;
  IR_val rs1, rs2;
  if (node->data.binary.lhs->type == DAG_CONST) {
    rs1.is_const = true;
    rs1.const_val = node->data.binary.lhs->data.const_val;
  } else {
    rs1.is_const = false;
    rs1.var = node->data.binary.lhs->data.var;
  }
  if (node->data.binary.rhs->type == DAG_CONST) {
    rs2.is_const = true;
    rs2.const_val = node->data.binary.rhs->data.const_val;
  } else {
    rs2.is_const = false;
    rs2.var = node->data.binary.rhs->data.var;
  }

  IR_stmt *stmt = (IR_stmt *)NEW(IR_op_stmt, op, rd, rs1, rs2);
  List_IR_stmt_ptr_push_back(&blk->stmts, stmt);
}

void generate_IR_from_DAG(IR_block *blk, Map_IR_var_DAG_node_ptr *var2node) {
  // 遍历DAG节点，生成IR语句
  for_map(IR_var, DAG_node_ptr, i, *var2node) {
    DAG_node_ptr node = i->val;
    if (!node->visited) {
      node->visited = true;
    }
    // 想一下什么类型DAG_node需要生成IR语句
    switch (node->type) {
    case DAG_BINARY: {
      block_append_IR_op_stmt(blk, node, i->key);
    }
    default:
      break;
    }
  }
}

static int DAG_node_cmp(const DAG_node_t *a, const DAG_node_t *b) {
  if (a->type != b->type) {
    return a->type < b->type; // 比较节点类型
  }
  switch (a->type) {
  case DAG_VAR:
    return a->data.var == b->data.var ? -1
                                      : a->data.var < b->data.var; // 比较变量id
  case DAG_CONST:
    return a->data.const_val == b->data.const_val
               ? -1
               : a->data.const_val < b->data.const_val; // 比较常量值
  case DAG_BINARY:
    if (a->data.binary.op != b->data.binary.op) {
      return a->data.binary.op == b->data.binary.op
                 ? -1
                 : a->data.binary.op < b->data.binary.op; // 比较操作符
    }
    if (a->data.binary.lhs != b->data.binary.lhs) {
      return a->data.binary.lhs == b->data.binary.lhs
                 ? -1
                 : a->data.binary.lhs < b->data.binary.lhs; // 比较左子节点
    }
    return a->data.binary.rhs == b->data.binary.rhs
               ? -1
               : a->data.binary.rhs < b->data.binary.rhs; // 比较右子节点
  default:
    assert(0);
    return -1;
  }
}

int DAG_expr_cmp(const DAG_expr_t a, const DAG_expr_t b) {
  if (a.op != b.op) {
    return a.op < b.op; // 比较操作符
  }
  int lhs_cmp = DAG_node_cmp(a.lhs, b.lhs);
  if (lhs_cmp != -1) {
    return lhs_cmp; // 如果左子节点不同，返回比较结果
  }
  int rhs_cmp = DAG_node_cmp(a.rhs, b.rhs);
  return rhs_cmp; // 如果右子节点不同，返回比较结果
}
