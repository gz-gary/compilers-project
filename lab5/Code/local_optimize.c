#include "local_optimize.h"
#include "DAG.h"
#include <stdio.h>

static IR_stmt *tmp[10050];
static int tmp_cnt = 0;

static void IR_block_clear_stmts(IR_block *blk) {
    while (blk->stmts.tail != NULL) {
        IR_stmt *stmt = blk->stmts.tail->val;

        List_IR_stmt_ptr_pop_back(&blk->stmts);
        // 或者是VCALL(blk->stmts, pop_back);
        RDELETE(IR_stmt, stmt);
    }
}

static void IR_block_local_optimize(IR_block *blk) {
    // 初始化DAG相关结构
    // 考虑要不要初始化一个vec存下来blk中所有stmt对应的DAG_node
    Map_IR_var_DAG_node_ptr var2node;
    Map_DAG_expr_t_DAG_node_ptr expr2node;
    Map_IR_var_DAG_node_ptr_init(&var2node);
    Map_DAG_expr_t_DAG_node_ptr_init(&expr2node);
    
    // 读取所有stmt到DAG中
    for_list(IR_stmt_ptr, i, blk->stmts) {
        IR_stmt *stmt = i->val;
        build_DAG_from_stmt(stmt, &var2node, &expr2node);
    }

    // 清除原有stmts，其实这里stmts可以删掉了，我再创建新的stmts就是？
    IR_block_clear_stmts(blk);

    // DAG->IR
    generate_IR_from_DAG(blk, &var2node);
}

void local_optimize() {
    for_vec(IR_function_ptr, i, ir_program_global->functions) {
        IR_function *func = *i;
        for_list(IR_block_ptr, i, func->blocks) {
            IR_block_local_optimize(i->val);
        }
    }
}

/*
试了一下local.ir（主代码块仅有一个block）
使用`for_list(IR_block_ptr, i, func->blocks)`遍历block发现有3个block
确定为entry->block->exit
同时在我测试代码中，我将block中的stmt全部pop_back到tmp中
并且重新拷贝到block中（但是忘记删除tmp）
最后发现main.c中IR_output(argc >= 3 ? argv[2] : NULL);输出了2遍block内容
可见我把tmp复制进exit block从而导致错误
*/
