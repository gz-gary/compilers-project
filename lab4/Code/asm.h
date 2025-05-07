#ifndef ASM_H
#define ASM_H

#include "ir.h"

enum MIPS32_instr {
    MIPS32_dummy,
    MIPS32_tag,
    MIPS32_li,
    MIPS32_move,
    MIPS32_add,
    MIPS32_addi,
    MIPS32_sub,
    MIPS32_mul,
    MIPS32_div,
    MIPS32_mflo,
    MIPS32_lw,
    MIPS32_sw,
    MIPS32_j,
    MIPS32_jal,
    MIPS32_jr,
    MIPS32_beq,
    MIPS32_bne,
    MIPS32_bgt,
    MIPS32_blt,
    MIPS32_bge,
    MIPS32_ble,
};

struct asm_reg_t {
    const char *alias;
    int used_by;
    int available;
    int timestamp;
};
typedef struct asm_reg_t asm_reg_t;

struct asm_code_t {
    enum MIPS32_instr instr;
    union {
        struct { const char *tag_id; };
        struct { asm_reg_t *li_reg; int li_imm; };
        struct { asm_reg_t *move_reg_dest; asm_reg_t *move_reg_src; };
        struct { asm_reg_t *addi_reg_dest; asm_reg_t *addi_reg_src; int addi_imm; };
        struct { asm_reg_t *add_reg_dest; asm_reg_t *add_reg_src1; asm_reg_t *add_reg_src2; };
        struct { asm_reg_t *sub_reg_dest; asm_reg_t *sub_reg_src1; asm_reg_t *sub_reg_src2; };
        struct { asm_reg_t *mul_reg_dest; asm_reg_t *mul_reg_src1; asm_reg_t *mul_reg_src2; };
        struct { asm_reg_t *div_reg_src1; asm_reg_t *div_reg_src2; };
        struct { asm_reg_t *mflo_reg_dest; };
        struct { asm_reg_t *lw_reg_dest; int lw_imm_base; asm_reg_t *lw_reg_offset; };
        struct { asm_reg_t *sw_reg_dest; int sw_imm_base; asm_reg_t *sw_reg_offset; };
        struct { const char *j_id; };
        struct { const char *jal_id; };
        struct { asm_reg_t *jr_reg; };
        struct { asm_reg_t *beq_reg_src1; asm_reg_t *beq_reg_src2; const char *beq_id; };
        struct { asm_reg_t *bne_reg_src1; asm_reg_t *bne_reg_src2; const char *bne_id; };
        struct { asm_reg_t *bgt_reg_src1; asm_reg_t *bgt_reg_src2; const char *bgt_id; };
        struct { asm_reg_t *blt_reg_src1; asm_reg_t *blt_reg_src2; const char *blt_id; };
        struct { asm_reg_t *bge_reg_src1; asm_reg_t *bge_reg_src2; const char *bge_id; };
        struct { asm_reg_t *ble_reg_src1; asm_reg_t *ble_reg_src2; const char *ble_id; };
    };
    struct asm_code_t *next;
};
typedef struct asm_code_t asm_code_t;

void ir2asm(ir_code_block_t *block);
void asm_set_output_file(FILE *f);
void asm_dump();

#endif