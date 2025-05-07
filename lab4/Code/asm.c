#include "asm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ir_parse_imm(const char *str) {
    int imm;
    sscanf(str + 1, "%d", &imm);
    return imm;
}

static FILE *output_file = NULL;

static asm_code_t head = { .instr = MIPS32_dummy, .next = NULL };
static asm_code_t *tail = &head;

static asm_reg_t regs[32];
static asm_reg_t *reg_zero = &regs[0];
static asm_reg_t *reg_at = &regs[1];
static asm_reg_t *reg_v0 = &regs[2];
static asm_reg_t *reg_v1 = &regs[3];
static asm_reg_t *reg_a0 = &regs[4];
static asm_reg_t *reg_a1 = &regs[5];
static asm_reg_t *reg_a2 = &regs[6];
static asm_reg_t *reg_a3 = &regs[7];
static asm_reg_t *reg_t0 = &regs[8];
static asm_reg_t *reg_t1 = &regs[9];
static asm_reg_t *reg_t2 = &regs[10];
static asm_reg_t *reg_t3 = &regs[11];
static asm_reg_t *reg_t4 = &regs[12];
static asm_reg_t *reg_t5 = &regs[13];
static asm_reg_t *reg_t6 = &regs[14];
static asm_reg_t *reg_t7 = &regs[15];
static asm_reg_t *reg_s0 = &regs[16];
static asm_reg_t *reg_s1 = &regs[17];
static asm_reg_t *reg_s2 = &regs[18];
static asm_reg_t *reg_s3 = &regs[19];
static asm_reg_t *reg_s4 = &regs[20];
static asm_reg_t *reg_s5 = &regs[21];
static asm_reg_t *reg_s6 = &regs[22];
static asm_reg_t *reg_s7 = &regs[23];
static asm_reg_t *reg_t8 = &regs[24];
static asm_reg_t *reg_t9 = &regs[25];
static asm_reg_t *reg_k0 = &regs[26];
static asm_reg_t *reg_k1 = &regs[27];
static asm_reg_t *reg_gp = &regs[28];
static asm_reg_t *reg_sp = &regs[29];
static asm_reg_t *reg_fp = &regs[30];
static asm_reg_t *reg_ra = &regs[31];
static asm_reg_t *asm_reg_init() {
    for (int i = 0; i < 32; ++i) {
        regs[i].available = 1;
        regs[i].used_by = -1;
    }
    asm_reg_t* regs_unavailable[] = {
        reg_v0, reg_v1,
        reg_zero, reg_at, reg_sp, reg_fp, reg_ra, reg_gp, reg_k0, reg_k1,
        reg_a0, reg_a1, reg_a2, reg_a3
    };
    for (int i = 0; i < sizeof(regs_unavailable) / sizeof(asm_reg_t*); ++i) regs_unavailable[i]->available = 0;

    reg_zero->alias = "zero";
    reg_at->alias = "at";
    reg_v0->alias = "v0";
    reg_v1->alias = "v1";
    reg_a0->alias = "a0";
    reg_a1->alias = "a1";
    reg_a2->alias = "a2";
    reg_a3->alias = "a3";
    reg_t0->alias = "t0";
    reg_t1->alias = "t1";
    reg_t2->alias = "t2";
    reg_t3->alias = "t3";
    reg_t4->alias = "t4";
    reg_t5->alias = "t5";
    reg_t6->alias = "t6";
    reg_t7->alias = "t7";
    reg_s0->alias = "s0";
    reg_s1->alias = "s1";
    reg_s2->alias = "s2";
    reg_s3->alias = "s3";
    reg_s4->alias = "s4";
    reg_s5->alias = "s5";
    reg_s6->alias = "s6";
    reg_s7->alias = "s7";
    reg_t8->alias = "t8";
    reg_t9->alias = "t9";
    reg_k0->alias = "k0";
    reg_k1->alias = "k1";
    reg_gp->alias = "gp";
    reg_sp->alias = "sp";
    reg_fp->alias = "fp";
    reg_ra->alias = "ra";
}
static asm_code_t *asm_new_code() { return malloc(sizeof(asm_code_t)); }
static asm_code_t *asm_new_code_tag(const char *tag_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_tag;
    code->tag_id = tag_id;
    return code;
}
static asm_code_t *asm_new_code_addi(asm_reg_t *addi_reg_dest, asm_reg_t *addi_reg_src, int addi_imm) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_addi;
    code->addi_reg_dest = addi_reg_dest;
    code->addi_reg_src = addi_reg_src;
    code->addi_imm = addi_imm;
    return code;
}
static asm_code_t *asm_new_code_add(asm_reg_t *add_reg_dest, asm_reg_t *add_reg_src1, asm_reg_t *add_reg_src2) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_add;
    code->add_reg_dest = add_reg_dest;
    code->add_reg_src1 = add_reg_src1;
    code->add_reg_src2 = add_reg_src2;
    return code;
}
static asm_code_t *asm_new_code_li(asm_reg_t *li_reg, int li_imm) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_li;
    code->li_reg = li_reg;
    code->li_imm = li_imm;
    return code;
}
static asm_code_t *asm_new_code_lw(asm_reg_t *lw_reg_dest, int lw_imm_base, asm_reg_t *lw_reg_offset) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_lw;
    code->lw_reg_dest = lw_reg_dest;
    code->lw_imm_base = lw_imm_base;
    code->lw_reg_offset = lw_reg_offset;
    return code;
}
static asm_code_t *asm_new_code_sw(asm_reg_t *sw_reg_dest, int sw_imm_base, asm_reg_t *sw_reg_offset) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_sw;
    code->sw_reg_dest = sw_reg_dest;
    code->sw_imm_base = sw_imm_base;
    code->sw_reg_offset = sw_reg_offset;
    return code;
}
static asm_code_t *asm_new_code_move(asm_reg_t *move_reg_dest, asm_reg_t *move_reg_src) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_move;
    code->move_reg_dest = move_reg_dest;
    code->move_reg_src = move_reg_src;
    return code;
}
static asm_code_t *asm_new_code_mul(asm_reg_t *mul_reg_dest, asm_reg_t *mul_reg_src1, asm_reg_t *mul_reg_src2) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_mul;
    code->mul_reg_dest = mul_reg_dest;
    code->mul_reg_src1 = mul_reg_src1;
    code->mul_reg_src2 = mul_reg_src2;
    return code;
}
static asm_code_t *asm_new_code_div(asm_reg_t *div_reg_src1, asm_reg_t *div_reg_src2) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_div;
    code->div_reg_src1 = div_reg_src1;
    code->div_reg_src2 = div_reg_src2;
    return code;
}
static asm_code_t *asm_new_code_mflo(asm_reg_t *mflo_reg_dest) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_mflo;
    code->mflo_reg_dest = mflo_reg_dest;
    return code;
}
static asm_code_t *asm_new_code_sub(asm_reg_t *sub_reg_dest, asm_reg_t *sub_reg_src1, asm_reg_t *sub_reg_src2) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_sub;
    code->sub_reg_dest = sub_reg_dest;
    code->sub_reg_src1 = sub_reg_src1;
    code->sub_reg_src2 = sub_reg_src2;
    return code;
}
static asm_code_t *asm_new_code_j(const char *j_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_j;
    code->j_id = j_id;
    return code;
}
static asm_code_t *asm_new_code_beq(asm_reg_t *beq_reg_src1, asm_reg_t *beq_reg_src2, const char *beq_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_beq;
    code->beq_id = beq_id;
    code->beq_reg_src1 = beq_reg_src1;
    code->beq_reg_src2 = beq_reg_src2;
    code->beq_id = beq_id;
    return code;
}
static asm_code_t *asm_new_code_bne(asm_reg_t *bne_reg_src1, asm_reg_t *bne_reg_src2, const char *bne_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_bne;
    code->bne_id = bne_id;
    code->bne_reg_src1 = bne_reg_src1;
    code->bne_reg_src2 = bne_reg_src2;
    code->bne_id = bne_id;
    return code;
}
static asm_code_t *asm_new_code_bgt(asm_reg_t *bgt_reg_src1, asm_reg_t *bgt_reg_src2, const char *bgt_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_bgt;
    code->bgt_id = bgt_id;
    code->bgt_reg_src1 = bgt_reg_src1;
    code->bgt_reg_src2 = bgt_reg_src2;
    code->bgt_id = bgt_id;
    return code;
}
static asm_code_t *asm_new_code_blt(asm_reg_t *blt_reg_src1, asm_reg_t *blt_reg_src2, const char *blt_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_blt;
    code->blt_id = blt_id;
    code->blt_reg_src1 = blt_reg_src1;
    code->blt_reg_src2 = blt_reg_src2;
    code->blt_id = blt_id;
    return code;
}
static asm_code_t *asm_new_code_bge(asm_reg_t *bge_reg_src1, asm_reg_t *bge_reg_src2, const char *bge_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_bge;
    code->bge_id = bge_id;
    code->bge_reg_src1 = bge_reg_src1;
    code->bge_reg_src2 = bge_reg_src2;
    code->bge_id = bge_id;
    return code;
}
static asm_code_t *asm_new_code_ble(asm_reg_t *ble_reg_src1, asm_reg_t *ble_reg_src2, const char *ble_id) {
    asm_code_t *code = asm_new_code();
    code->instr = MIPS32_ble;
    code->ble_id = ble_id;
    code->ble_reg_src1 = ble_reg_src1;
    code->ble_reg_src2 = ble_reg_src2;
    code->ble_id = ble_id;
    return code;
}

static void asm_append_code(asm_code_t *code) {
    tail->next = code;
    tail = code;
}



static int timestamp = 0;
static struct {
    const char *var; // variable
    int offset; // offset
    int size; // size
    asm_reg_t *reg;
} vars[10000];
static int var_len;
static void asm_init_vars() {
    var_len = 0;
}
static void asm_add_var(const char *var, int size) {
    if (var[0] == '#' || var[0] == '&') return;
    for (int i = 0; i < var_len; ++i)
        if (!strcmp(vars[i].var, var))
            return;
    vars[var_len].var = var;
    vars[var_len].offset = var_len > 0 ? vars[var_len - 1].offset + vars[var_len - 1].size : 0;
    vars[var_len].size = size;
    vars[var_len].reg = NULL;
    ++var_len;
}
static int asm_query_var_offset(const char *var) {
    for (int i = 0; i < var_len; ++i)
        if (!strcmp(vars[i].var, var))
            return vars[i].offset;
}
static int asm_query_var_idx(const char *var) {
    for (int i = 0; i < var_len; ++i)
        if (!strcmp(vars[i].var, var))
            return i;
}
static int asm_query_stack_size() {
    return var_len > 0 ? vars[var_len - 1].offset + vars[var_len - 1].size : 0;
}
static void asm_free_reg(asm_reg_t *reg) {
    int var_idx = reg->used_by;
    if (var_idx >= 0) {
        asm_append_code(asm_new_code_sw(reg, vars[var_idx].offset, reg_sp));
        vars[var_idx].reg = NULL;
    }
    reg->used_by = -1;
}
static void asm_move_into_reg(asm_reg_t* reg) {
    asm_append_code(asm_new_code_lw(reg, vars[reg->used_by].offset, reg_sp));
}
static asm_reg_t* asm_alloc_reg() {
    int idx = -1;
    for (int i = 0; i < 32; ++i) if (regs[i].available && regs[i].used_by == -1) {
        idx = i; break;
    }
    if (idx != -1) return &regs[idx];
    for (int i = 0; i < 32; ++i) if (regs[i].available) {
        if (idx == -1) idx = i;
        else if (regs[i].timestamp < regs[idx].timestamp) idx = i;
    }
    asm_free_reg(&regs[idx]);
    return &regs[idx];
}
static asm_reg_t* asm_alloc_reg4var(int var_idx, int if_move) {
    if (vars[var_idx].reg) { vars[var_idx].reg->timestamp = timestamp++; return vars[var_idx].reg; }
    asm_reg_t* reg = asm_alloc_reg();
    reg->timestamp = timestamp++;
    reg->used_by = var_idx;
    vars[var_idx].reg = reg;
    if (if_move) asm_move_into_reg(reg);
    return reg;
}
static asm_reg_t* asm_alloc_reg4imm(int imm) {
    asm_reg_t* reg = asm_alloc_reg();
    reg->timestamp = timestamp++;
    reg->used_by = -2;
    asm_append_code(asm_new_code_li(reg, imm));
    return reg;
}
static asm_reg_t* asm_alloc_reg4ref(int offset) {
    asm_reg_t* reg = asm_alloc_reg();
    reg->timestamp = timestamp++;
    reg->used_by = -2;
    asm_append_code(asm_new_code_addi(reg, reg_sp, offset));
    return reg;
}



void ir2asm(ir_code_block_t *block) {
    asm_reg_init();

    ir_code_t *code = block->first;
    while (code != NULL) {
        switch (code->type) {
        case IR_FUNDEC:
            {
                asm_append_code(asm_new_code_tag(code->fun_name));

                asm_init_vars();
                ir_code_t *inside = code->next;
                while (inside != NULL) {
                    if (inside->type == IR_DEC) asm_add_var(inside->dec_name, inside->dec_size);
                    if (inside->type == IR_OP) {
                        if (inside->op1) asm_add_var(inside->op1->name, 4);
                        if (inside->op2) asm_add_var(inside->op2->name, 4);
                        if (inside->result) asm_add_var(inside->result->name, 4);
                    }
                    inside = inside->next;
                }
                int stack_size = asm_query_stack_size();

                asm_append_code(asm_new_code_addi(reg_sp, reg_sp, -stack_size));

                inside = code->next;
                while (inside != NULL) {
                    switch (inside->type) {
                    case IR_OP:
                        if (!strcmp(inside->op_name, "ASSIGN")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            if (inside->op1->name[0] == '#') { // constant assignment
                                asm_append_code(asm_new_code_li(vars[dest_idx].reg, ir_parse_imm(inside->op1->name)));
                            } else if (inside->op1->name[0] == '&') { // address assignment
                                int src_idx = asm_query_var_idx((inside->op1->name) + 1);
                                asm_append_code(asm_new_code_addi(vars[dest_idx].reg, reg_sp, vars[src_idx].offset));
                            } else { // var assignment
                                int src_idx = asm_query_var_idx(inside->op1->name);
                                asm_alloc_reg4var(src_idx, 1);
                                asm_append_code(asm_new_code_move(vars[dest_idx].reg, vars[src_idx].reg));
                            }
                            // asm_free_reg(vars[dest_idx].reg);
                        } else if (!strcmp(inside->op_name, "PLUS")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src1_reg, *src2_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src1_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                src1_reg = asm_alloc_reg4ref(vars[asm_query_var_idx((inside->op1->name) + 1)].offset);
                                break;
                            default:
                                src1_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            switch (inside->op2->name[0]) {
                            case '#':
                                asm_append_code(asm_new_code_addi(dest_reg, src1_reg, ir_parse_imm(inside->op2->name)));
                                break;
                            case '&':
                                break;
                            default:
                                src2_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op2->name), 1);
                                asm_append_code(asm_new_code_add(dest_reg, src1_reg, src2_reg));
                                break;
                            }
                            if (src1_reg->used_by == -2) asm_free_reg(src1_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "MINUS")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src1_reg, *src2_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src1_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                src1_reg = asm_alloc_reg4ref(vars[asm_query_var_idx((inside->op1->name) + 1)].offset);
                                break;
                            default:
                                src1_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            switch (inside->op2->name[0]) {
                            case '#':
                                asm_append_code(asm_new_code_addi(dest_reg, src1_reg, -ir_parse_imm(inside->op2->name)));
                                break;
                            case '&':
                                break;
                            default:
                                src2_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op2->name), 1);
                                asm_append_code(asm_new_code_sub(dest_reg, src1_reg, src2_reg));
                                break;
                            }
                            if (src1_reg->used_by == -2) asm_free_reg(src1_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "MULT")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src1_reg, *src2_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src1_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                break;
                            default:
                                src1_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            switch (inside->op2->name[0]) {
                            case '#':
                                src2_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op2->name));
                                break;
                            case '&':
                                break;
                            default:
                                src2_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op2->name), 1);
                                break;
                            }
                            asm_append_code(asm_new_code_mul(dest_reg, src1_reg, src2_reg));
                            if (src1_reg->used_by == -2) asm_free_reg(src1_reg);
                            if (src2_reg->used_by == -2) asm_free_reg(src2_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "DIV")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src1_reg, *src2_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src1_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                break;
                            default:
                                src1_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            switch (inside->op2->name[0]) {
                            case '#':
                                src2_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op2->name));
                                break;
                            case '&':
                                break;
                            default:
                                src2_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op2->name), 1);
                                break;
                            }
                            asm_append_code(asm_new_code_div(src1_reg, src2_reg));
                            asm_append_code(asm_new_code_mflo(dest_reg));
                            if (src1_reg->used_by == -2) asm_free_reg(src1_reg);
                            if (src2_reg->used_by == -2) asm_free_reg(src2_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "NEG")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                break;
                            default:
                                src_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            asm_append_code(asm_new_code_sub(dest_reg, reg_zero, src_reg));
                            if (src_reg->used_by == -2) asm_free_reg(src_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "POS")) {
                            int dest_idx = asm_query_var_idx(inside->result->name);
                            asm_alloc_reg4var(dest_idx, 0);
                            asm_reg_t *dest_reg = vars[dest_idx].reg;
                            asm_reg_t *src_reg;
                            switch (inside->op1->name[0]) {
                            case '#':
                                src_reg = asm_alloc_reg4imm(ir_parse_imm(inside->op1->name));
                                break;
                            case '&':
                                break;
                            default:
                                src_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                                break;
                            }
                            asm_append_code(asm_new_code_move(dest_reg, src_reg));
                            if (src_reg->used_by == -2) asm_free_reg(src_reg);
                            // asm_free_reg(dest_idx);
                        } else if (!strcmp(inside->op_name, "DEREF_R")) {
                            asm_reg_t *dest_reg = asm_alloc_reg4var(asm_query_var_idx(inside->result->name), 0);
                            asm_reg_t *src_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                            asm_append_code(asm_new_code_lw(dest_reg, 0, src_reg));
                        } else if (!strcmp(inside->op_name, "DEREF_L")) {
                            asm_reg_t *dest_reg = asm_alloc_reg4var(asm_query_var_idx(inside->result->name), 1);
                            asm_reg_t *src_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                            asm_append_code(asm_new_code_sw(src_reg, 0, dest_reg));
                        } else if (!strcmp(inside->op_name, "DEREF_LR")) {
                            asm_reg_t *dest_reg = asm_alloc_reg4var(asm_query_var_idx(inside->result->name), 1);
                            asm_reg_t *src_reg = asm_alloc_reg4var(asm_query_var_idx(inside->op1->name), 1);
                            asm_reg_t *middle_reg = asm_alloc_reg();
                            asm_append_code(asm_new_code_lw(middle_reg, 0, src_reg));
                            asm_append_code(asm_new_code_sw(middle_reg, 0, dest_reg));
                        }

                        break;
                    case IR_LABEL:
                        asm_append_code(asm_new_code_tag(inside->label_name));
                        break;
                    case IR_GOTO:
                        asm_append_code(asm_new_code_j(inside->goto_dest->label_name));
                        break;
                    case IR_RELOP_GOTO:
                        {
                            asm_reg_t *src1_reg;
                            asm_reg_t *src2_reg;
                            switch (inside->relop1->name[0]) {
                            case '#':
                                src1_reg = asm_alloc_reg4imm(ir_parse_imm(inside->relop1->name));
                                break;
                            case '&':
                                break;
                            default:
                                src1_reg = asm_alloc_reg4var(asm_query_var_idx(inside->relop1->name), 1);
                                break;
                            }
                            switch (inside->relop2->name[0]) {
                            case '#':
                                src2_reg = asm_alloc_reg4imm(ir_parse_imm(inside->relop2->name));
                                break;
                            case '&':
                                break;
                            default:
                                src2_reg = asm_alloc_reg4var(asm_query_var_idx(inside->relop2->name), 1);
                                break;
                            }
                            if (!strcmp(inside->relop_name, "==")) {
                                asm_append_code(asm_new_code_beq(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            } else if (!strcmp(inside->relop_name, "!=")) {
                                asm_append_code(asm_new_code_bne(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            } else if (!strcmp(inside->relop_name, ">")) {
                                asm_append_code(asm_new_code_bgt(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            } else if (!strcmp(inside->relop_name, "<")) {
                                asm_append_code(asm_new_code_blt(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            } else if (!strcmp(inside->relop_name, ">=")) {
                                asm_append_code(asm_new_code_bge(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            } else if (!strcmp(inside->relop_name, "<=")) {
                                asm_append_code(asm_new_code_ble(src1_reg, src2_reg, inside->relop_goto_dest->label_name));
                            }
                            if (src1_reg->used_by == -2) asm_free_reg(src1_reg);
                            if (src2_reg->used_by == -2) asm_free_reg(src2_reg);
                        }
                        break;
                    }
                    inside = inside->next;
                }
            }
            break;
        default:
            break;
        }
        code = code->next;
    }
}

void asm_set_output_file(FILE *f) {
    output_file = f;
    if (output_file == NULL) output_file = stdout;
}

void asm_dump() {
    asm_code_t *code = head.next;
    while (code != NULL) {
        switch (code->instr) {
            case MIPS32_dummy:
                break;
            case MIPS32_tag:
                fprintf(output_file, "%s:\n", code->tag_id);
                break;
            case MIPS32_li:
                fprintf(output_file, "li $%s, %d\n", code->li_reg->alias, code->li_imm);
                break;
            case MIPS32_move:
                fprintf(output_file, "move $%s, $%s\n", code->move_reg_dest->alias, code->move_reg_src->alias);
                break;
            case MIPS32_add:
                fprintf(output_file, "add $%s, $%s, $%s\n", code->add_reg_dest->alias, code->add_reg_src1->alias, code->add_reg_src2->alias);
                break;
            case MIPS32_addi:
                fprintf(output_file, "addi $%s, $%s, %d\n", code->addi_reg_dest->alias, code->addi_reg_src->alias, code->addi_imm);
                break;
            case MIPS32_sub:
                fprintf(output_file, "sub $%s, $%s, $%s\n", code->sub_reg_dest->alias, code->sub_reg_src1->alias, code->sub_reg_src2->alias);
                break;
            case MIPS32_mul:
                fprintf(output_file, "mul $%s, $%s, $%s\n", code->mul_reg_dest->alias, code->mul_reg_src1->alias, code->mul_reg_src2->alias);
                break;
            case MIPS32_div:
                fprintf(output_file, "div $%s, $%s\n", code->div_reg_src1->alias, code->div_reg_src2->alias);
                break;
            case MIPS32_mflo:
                fprintf(output_file, "mflo $%s\n", code->mflo_reg_dest->alias);
                break;
            case MIPS32_lw:
                fprintf(output_file, "lw $%s, %d($%s)\n", code->lw_reg_dest->alias, code->lw_imm_base, code->lw_reg_offset->alias);
                break;
            case MIPS32_sw:
                fprintf(output_file, "sw $%s, %d($%s)\n", code->sw_reg_dest->alias, code->sw_imm_base, code->sw_reg_offset->alias);
                break;
            case MIPS32_j:
                fprintf(output_file, "j %s\n", code->j_id);
                break;
            case MIPS32_jal:
                fprintf(output_file, "jal %s\n", code->jal_id);
                break;
            case MIPS32_jr:
                fprintf(output_file, "jr $%s\n", code->jr_reg->alias);
                break;
            case MIPS32_beq:
                fprintf(output_file, "beq $%s, $%s, %s\n", code->beq_reg_src1->alias, code->beq_reg_src2->alias, code->beq_id);
                break;
            case MIPS32_bne:
                fprintf(output_file, "bne $%s, $%s, %s\n", code->bne_reg_src1->alias, code->bne_reg_src2->alias, code->bne_id);
                break;
            case MIPS32_bgt:
                fprintf(output_file, "bgt $%s, $%s, %s\n", code->bgt_reg_src1->alias, code->bgt_reg_src2->alias, code->bgt_id);
                break;
            case MIPS32_blt:
                fprintf(output_file, "blt $%s, $%s, %s\n", code->blt_reg_src1->alias, code->blt_reg_src2->alias, code->blt_id);
                break;
            case MIPS32_bge:
                fprintf(output_file, "bge $%s, $%s, %s\n", code->bge_reg_src1->alias, code->bge_reg_src2->alias, code->bge_id);
                break;
            case MIPS32_ble:
                fprintf(output_file, "ble $%s, $%s, %s\n", code->ble_reg_src1->alias, code->ble_reg_src2->alias, code->ble_id);
                break;
        }
        code = code->next;
    }
}
