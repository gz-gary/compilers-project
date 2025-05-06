#include "asm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    for (int i = 0; i < 32; ++i) regs[i].used = 0;
    reg_zero->used = 1;
    reg_at->used = 1;
    reg_sp->used = 1;
    reg_ra->used = 1;

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

static void asm_append_code(asm_code_t *code) {
    tail->next = code;
    tail = code;
}



static struct {
    const char *var; // variable
    int offset; // offset
    int size; // size
    asm_reg_t* reg;
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
static int asm_query_stack_size() {
    return var_len > 0 ? vars[var_len - 1].offset + vars[var_len - 1].size : 0;
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
