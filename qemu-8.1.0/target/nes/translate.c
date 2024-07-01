#include "qemu/osdep.h"
#include "qemu/qemu-print.h"
#include "tcg/tcg.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "tcg/tcg-op.h"
#include "exec/cpu_ldst.h"
#include "exec/helper-proto.h"
#include "exec/helper-gen.h"
#include "exec/log.h"
#include "exec/translator.h"
#include "qemu/datadir.h"
#include "hw/loader.h"

#define HELPER_H "helper.h"
#include "exec/helper-info.c.inc"
#undef  HELPER_H


#undef BREAKPOINT_ON_BREAK

static TCGv cpu_pc;
static TCGv cpu_sp;
static TCGv cpu_a;
static TCGv cpu_x;
static TCGv cpu_y;
static TCGv cpu_f_C;
static TCGv cpu_f_Z;
static TCGv cpu_f_D;
static TCGv cpu_f_B;
static TCGv cpu_f_I;
static TCGv cpu_f_U;
static TCGv cpu_f_N;
static TCGv cpu_f_V;

#define DISAS_EXIT   DISAS_TARGET_0  /* We want return to the cpu main loop.  */
#define DISAS_LOOKUP DISAS_TARGET_1  /* We have a variable condition exit.  */
#define DISAS_CHAIN  DISAS_TARGET_2  /* We have a single condition exit.  */

typedef struct DisasContext DisasContext;

/* This is the state at translation time. */
struct DisasContext {
    DisasContextBase base;
    CPUNESState *env;
    CPUState *cs;
    target_long pc;
};

void nes_cpu_tcg_init(void)
{
#define NES_REG_OFFS(x) offsetof(CPUNESState, x)
    cpu_pc = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(PC), "PC");
    cpu_sp = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(SP), "SP");
    cpu_a = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(A), "A");
    cpu_x = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(X), "X");
    cpu_y = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(Y), "Y");
    cpu_f_C = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_C), "f_C");
    cpu_f_Z = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_Z), "f_Z");
    cpu_f_D = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_D), "f_D");
    cpu_f_B = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_B), "f_B");
    cpu_f_I = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_I), "f_I");
    cpu_f_U = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_U), "f_U");
    cpu_f_N = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_N), "f_N");
    cpu_f_V = tcg_global_mem_new_i32(cpu_env, NES_REG_OFFS(f_V), "f_V");
#undef NES_REG_OFFS
}

static int sign_extend(int x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFFFFFF << bit_count);
    }
    return x;
}

/* decoder helper */
static uint32_t decode_load_bytes(DisasContext *ctx, uint32_t insn,
                           int i, int n)
{
    while (++i <= n) {
        uint8_t b = cpu_ldub_code(ctx->env, ctx->base.pc_next++);
        insn |= b << (32 - i * 8);
    }
    return insn;
}

#include "decode-insn.c.inc"

static void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
    const TranslationBlock *tb = ctx->base.tb;

    // pc_first ^ dest * 2；如果不 x2，永远不会 block chain
    if (translator_use_goto_tb(&ctx->base, dest)) {
        tcg_gen_goto_tb(n);
        tcg_gen_movi_i32(cpu_pc, dest);
        tcg_gen_exit_tb(tb, n);
    } else {
        tcg_gen_movi_i32(cpu_pc, dest);
        tcg_gen_lookup_and_goto_ptr();
    }
    ctx->base.is_jmp = DISAS_NORETURN;
}

static bool trans_A_ASL  (DisasContext *ctx, arg_A_ASL   *a)
{
    return true;
}

static bool trans_A_LSR  (DisasContext *ctx, arg_A_LSR   *a)
{
    return true;
}

static bool trans_A_ROL  (DisasContext *ctx, arg_A_ROL   *a)
{
    return true;
}

static bool trans_A_ROR  (DisasContext *ctx, arg_A_ROR   *a)
{
    return true;
}

static bool trans_DEX    (DisasContext *ctx, arg_DEX     *a)
{
    return true;
}

static bool trans_DEY    (DisasContext *ctx, arg_DEY     *a)
{
    return true;
}

static bool trans_INX    (DisasContext *ctx, arg_INX     *a)
{
    return true;
}

static bool trans_INY    (DisasContext *ctx, arg_INY     *a)
{
    return true;
}

static bool trans_BRK    (DisasContext *ctx, arg_BRK     *a)
{
    return true;
}

static bool trans_RTI    (DisasContext *ctx, arg_RTI     *a)
{
    return true;
}

static bool trans_RTS    (DisasContext *ctx, arg_RTS     *a)
{
    return true;
}

static bool trans_NOP    (DisasContext *ctx, arg_NOP     *a)
{
    return true;
}

static bool trans_CLC    (DisasContext *ctx, arg_CLC     *a)
{
    return true;
}

static bool trans_CLD    (DisasContext *ctx, arg_CLD     *a)
{
    return true;
}

static bool trans_CLI    (DisasContext *ctx, arg_CLI     *a)
{
    return true;
}

static bool trans_CLV    (DisasContext *ctx, arg_CLV     *a)
{
    return true;
}

static bool trans_SEC    (DisasContext *ctx, arg_SEC     *a)
{
    return true;
}

static bool trans_SED    (DisasContext *ctx, arg_SED     *a)
{
    return true;
}

static bool trans_SEI    (DisasContext *ctx, arg_SEI     *a)
{
    return true;
}

static bool trans_PHA    (DisasContext *ctx, arg_PHA     *a)
{
    return true;
}

static bool trans_PHP    (DisasContext *ctx, arg_PHP     *a)
{
    return true;
}

static bool trans_PLA    (DisasContext *ctx, arg_PLA     *a)
{
    return true;
}

static bool trans_PLP    (DisasContext *ctx, arg_PLP     *a)
{
    return true;
}

static bool trans_TAX    (DisasContext *ctx, arg_TAX     *a)
{
    return true;
}

static bool trans_TAY    (DisasContext *ctx, arg_TAY     *a)
{
    return true;
}

static bool trans_TSX    (DisasContext *ctx, arg_TSX     *a)
{
    return true;
}

static bool trans_TXA    (DisasContext *ctx, arg_TXA     *a)
{
    return true;
}

static bool trans_TXS    (DisasContext *ctx, arg_TXS     *a)
{
    return true;
}

static bool trans_TYA    (DisasContext *ctx, arg_TYA     *a)
{
    return true;
}

static bool trans_IMM_ADC(DisasContext *ctx, arg_IMM_ADC *a)
{
    return true;
}

static bool trans_IMM_SBC(DisasContext *ctx, arg_IMM_SBC *a)
{
    return true;
}

static bool trans_IMM_AND(DisasContext *ctx, arg_IMM_AND *a)
{
    return true;
}

static bool trans_IMM_EOR(DisasContext *ctx, arg_IMM_EOR *a)
{
    return true;
}

static bool trans_IMM_ORA(DisasContext *ctx, arg_IMM_ORA *a)
{
    return true;
}

static bool trans_IMM_CMP(DisasContext *ctx, arg_IMM_CMP *a)
{
    return true;
}

static bool trans_IMM_CPX(DisasContext *ctx, arg_IMM_CPX *a)
{
    return true;
}

static bool trans_IMM_LDA(DisasContext *ctx, arg_IMM_LDA *a)
{
    return true;
}

static bool trans_IMM_LDX(DisasContext *ctx, arg_IMM_LDX *a)
{
    return true;
}

static bool trans_IMM_LDY(DisasContext *ctx, arg_IMM_LDY *a)
{
    return true;
}

static bool trans_RE_BCC (DisasContext *ctx, arg_RE_BCC  *a)
{
    return true;
}

static bool trans_RE_BCS (DisasContext *ctx, arg_RE_BCS  *a)
{
    return true;
}

static bool trans_RE_BEQ (DisasContext *ctx, arg_RE_BEQ  *a)
{
    return true;
}

static bool trans_RE_BMI (DisasContext *ctx, arg_RE_BMI  *a)
{
    return true;
}

static bool trans_RE_BNE (DisasContext *ctx, arg_RE_BNE  *a)
{
    return true;
}

static bool trans_RE_BPL (DisasContext *ctx, arg_RE_BPL  *a)
{
    return true;
}

static bool trans_RE_BVC (DisasContext *ctx, arg_RE_BVC  *a)
{
    return true;
}

static bool trans_RE_BVS (DisasContext *ctx, arg_RE_BVS  *a)
{
    return true;
}

static bool trans_AB_ADC (DisasContext *ctx, arg_AB_ADC  *a)
{
    return true;
}

static bool trans_ABX_ADC(DisasContext *ctx, arg_ABX_ADC *a)
{
    return true;
}

static bool trans_ABY_ADC(DisasContext *ctx, arg_ABY_ADC *a)
{
    return true;
}

static bool trans_AB_SBC (DisasContext *ctx, arg_AB_SBC  *a)
{
    return true;
}

static bool trans_ABX_SBC(DisasContext *ctx, arg_ABX_SBC *a)
{
    return true;
}

static bool trans_ABY_SBC(DisasContext *ctx, arg_ABY_SBC *a)
{
    return true;
}

static bool trans_AB_DEC (DisasContext *ctx, arg_AB_DEC  *a)
{
    return true;
}

static bool trans_ABX_DEC(DisasContext *ctx, arg_ABX_DEC *a)
{
    return true;
}

static bool trans_AB_INC (DisasContext *ctx, arg_AB_INC  *a)
{
    return true;
}

static bool trans_ABX_INC(DisasContext *ctx, arg_ABX_INC *a)
{
    return true;
}

static bool trans_AB_AND (DisasContext *ctx, arg_AB_AND  *a)
{
    return true;
}

static bool trans_ABX_AND(DisasContext *ctx, arg_ABX_AND *a)
{
    return true;
}

static bool trans_ABY_AND(DisasContext *ctx, arg_ABY_AND *a)
{
    return true;
}

static bool trans_AB_ASL (DisasContext *ctx, arg_AB_ASL  *a)
{
    return true;
}

static bool trans_ABX_ASL(DisasContext *ctx, arg_ABX_ASL *a)
{
    return true;
}

static bool trans_AB_LSR (DisasContext *ctx, arg_AB_LSR  *a)
{
    return true;
}

static bool trans_ABX_LSR(DisasContext *ctx, arg_ABX_LSR *a)
{
    return true;
}

static bool trans_AB_ORA (DisasContext *ctx, arg_AB_ORA  *a)
{
    return true;
}

static bool trans_ABX_ORA(DisasContext *ctx, arg_ABX_ORA *a)
{
    return true;
}

static bool trans_ABY_ORA(DisasContext *ctx, arg_ABY_ORA *a)
{
    return true;
}

static bool trans_AB_EOR (DisasContext *ctx, arg_AB_EOR  *a)
{
    return true;
}

static bool trans_ABX_EOR(DisasContext *ctx, arg_ABX_EOR *a)
{
    return true;
}

static bool trans_ABY_EOR(DisasContext *ctx, arg_ABY_EOR *a)
{
    return true;
}

static bool trans_AB_ROR (DisasContext *ctx, arg_AB_ROR  *a)
{
    return true;
}

static bool trans_ABX_ROR(DisasContext *ctx, arg_ABX_ROR *a)
{
    return true;
}

static bool trans_AB_ROL (DisasContext *ctx, arg_AB_ROL  *a)
{
    return true;
}

static bool trans_ABX_ROL(DisasContext *ctx, arg_ABX_ROL *a)
{
    return true;
}

static bool trans_AB_JMP (DisasContext *ctx, arg_AB_JMP  *a)
{
    return true;
}

static bool trans_IN_JMP (DisasContext *ctx, arg_IN_JMP  *a)
{
    return true;
}

static bool trans_AB_JSR (DisasContext *ctx, arg_AB_JSR  *a)
{
    return true;
}

static bool trans_AB_LDY (DisasContext *ctx, arg_AB_LDY  *a)
{
    return true;
}

static bool trans_ABX_LDY(DisasContext *ctx, arg_ABX_LDY *a)
{
    return true;
}

static bool trans_AB_LDX (DisasContext *ctx, arg_AB_LDX  *a)
{
    return true;
}

static bool trans_ABY_LDX(DisasContext *ctx, arg_ABY_LDX *a)
{
    return true;
}

static bool trans_AB_STA (DisasContext *ctx, arg_AB_STA  *a)
{
    return true;
}

static bool trans_ABX_STA(DisasContext *ctx, arg_ABX_STA *a)
{
    return true;
}

static bool trans_ABY_STA(DisasContext *ctx, arg_ABY_STA *a)
{
    return true;
}

static bool trans_AB_STX (DisasContext *ctx, arg_AB_STX  *a)
{
    return true;
}

static bool trans_AB_STY (DisasContext *ctx, arg_AB_STY  *a)
{
    return true;
}

static bool trans_AB_BIT (DisasContext *ctx, arg_AB_BIT  *a)
{
    return true;
}

static bool trans_AB_CPX (DisasContext *ctx, arg_AB_CPX  *a)
{
    return true;
}

static bool trans_AB_CPY (DisasContext *ctx, arg_AB_CPY  *a)
{
    return true;
}

static bool trans_AB_CMP (DisasContext *ctx, arg_AB_CMP  *a)
{
    return true;
}

static bool trans_ABX_CMP(DisasContext *ctx, arg_ABX_CMP *a)
{
    return true;
}

static bool trans_ABY_CMP(DisasContext *ctx, arg_ABY_CMP *a)
{
    return true;
}

static bool trans_AB_LDA (DisasContext *ctx, arg_AB_LDA  *a)
{
    return true;
}

static bool trans_ABX_LDA(DisasContext *ctx, arg_ABX_LDA *a)
{
    return true;
}

static bool trans_ABY_LDA(DisasContext *ctx, arg_ABY_LDA *a)
{
    return true;
}

static bool trans_ZP_ADC (DisasContext *ctx, arg_ZP_ADC  *a)
{
    return true;
}

static bool trans_ZPX_ADC(DisasContext *ctx, arg_ZPX_ADC *a)
{
    return true;
}

static bool trans_INX_ADC(DisasContext *ctx, arg_INX_ADC *a)
{
    return true;
}

static bool trans_INY_ADC(DisasContext *ctx, arg_INY_ADC *a)
{
    return true;
}

static bool trans_ZP_SBC (DisasContext *ctx, arg_ZP_SBC  *a)
{
    return true;
}

static bool trans_ZPX_SBC(DisasContext *ctx, arg_ZPX_SBC *a)
{
    return true;
}

static bool trans_INX_SBC(DisasContext *ctx, arg_INX_SBC *a)
{
    return true;
}

static bool trans_INY_SBC(DisasContext *ctx, arg_INY_SBC *a)
{
    return true;
}

static bool trans_ZP_DEC (DisasContext *ctx, arg_ZP_DEC  *a)
{
    return true;
}

static bool trans_ZPX_DEC(DisasContext *ctx, arg_ZPX_DEC *a)
{
    return true;
}

static bool trans_ZP_INC (DisasContext *ctx, arg_ZP_INC  *a)
{
    return true;
}

static bool trans_ZPX_INC(DisasContext *ctx, arg_ZPX_INC *a)
{
    return true;
}

static bool trans_ZP_AND (DisasContext *ctx, arg_ZP_AND  *a)
{
    return true;
}

static bool trans_ZPX_AND(DisasContext *ctx, arg_ZPX_AND *a)
{
    return true;
}

static bool trans_INX_AND(DisasContext *ctx, arg_INX_AND *a)
{
    return true;
}

static bool trans_INY_AND(DisasContext *ctx, arg_INY_AND *a)
{
    return true;
}

static bool trans_ZP_ASL (DisasContext *ctx, arg_ZP_ASL  *a)
{
    return true;
}

static bool trans_ZPX_ASL(DisasContext *ctx, arg_ZPX_ASL *a)
{
    return true;
}

static bool trans_ZP_LSR (DisasContext *ctx, arg_ZP_LSR  *a)
{
    return true;
}

static bool trans_ZPX_LSR(DisasContext *ctx, arg_ZPX_LSR *a)
{
    return true;
}

static bool trans_ZP_EOR (DisasContext *ctx, arg_ZP_EOR  *a)
{
    return true;
}

static bool trans_ZPX_EOR(DisasContext *ctx, arg_ZPX_EOR *a)
{
    return true;
}

static bool trans_INX_EOR(DisasContext *ctx, arg_INX_EOR *a)
{
    return true;
}

static bool trans_INY_EOR(DisasContext *ctx, arg_INY_EOR *a)
{
    return true;
}

static bool trans_ZP_ORA (DisasContext *ctx, arg_ZP_ORA  *a)
{
    return true;
}

static bool trans_ZPX_ORA(DisasContext *ctx, arg_ZPX_ORA *a)
{
    return true;
}

static bool trans_INX_ORA(DisasContext *ctx, arg_INX_ORA *a)
{
    return true;
}

static bool trans_INY_ORA(DisasContext *ctx, arg_INY_ORA *a)
{
    return true;
}

static bool trans_ZP_ROL (DisasContext *ctx, arg_ZP_ROL  *a)
{
    return true;
}

static bool trans_ZPX_ROL(DisasContext *ctx, arg_ZPX_ROL *a)
{
    return true;
}

static bool trans_ZP_ROR (DisasContext *ctx, arg_ZP_ROR  *a)
{
    return true;
}

static bool trans_ZPX_ROR(DisasContext *ctx, arg_ZPX_ROR *a)
{
    return true;
}

static bool trans_ZP_BIT (DisasContext *ctx, arg_ZP_BIT  *a)
{
    return true;
}

static bool trans_ZP_CMP (DisasContext *ctx, arg_ZP_CMP  *a)
{
    return true;
}

static bool trans_ZPX_CMP(DisasContext *ctx, arg_ZPX_CMP *a)
{
    return true;
}

static bool trans_INX_CMP(DisasContext *ctx, arg_INX_CMP *a)
{
    return true;
}

static bool trans_INY_CMP(DisasContext *ctx, arg_INY_CMP *a)
{
    return true;
}

static bool trans_ZP_CPX (DisasContext *ctx, arg_ZP_CPX  *a)
{
    return true;
}

static bool trans_ZP_CPY (DisasContext *ctx, arg_ZP_CPY  *a)
{
    return true;
}

static bool trans_ZP_LDA (DisasContext *ctx, arg_ZP_LDA  *a)
{
    return true;
}

static bool trans_ZPX_LDA(DisasContext *ctx, arg_ZPX_LDA *a)
{
    return true;
}

static bool trans_INX_LDA(DisasContext *ctx, arg_INX_LDA *a)
{
    return true;
}

static bool trans_INY_LDA(DisasContext *ctx, arg_INY_LDA *a)
{
    return true;
}

static bool trans_ZP_LDX (DisasContext *ctx, arg_ZP_LDX  *a)
{
    return true;
}

static bool trans_ZPY_LDX(DisasContext *ctx, arg_ZPY_LDX *a)
{
    return true;
}

static bool trans_ZP_LDY (DisasContext *ctx, arg_ZP_LDY  *a)
{
    return true;
}

static bool trans_ZPX_LDY(DisasContext *ctx, arg_ZPX_LDY *a)
{
    return true;
}

static bool trans_ZP_STA (DisasContext *ctx, arg_ZP_STA  *a)
{
    return true;
}

static bool trans_ZPX_STA(DisasContext *ctx, arg_ZPX_STA *a)
{
    return true;
}

static bool trans_INX_STA(DisasContext *ctx, arg_INX_STA *a)
{
    return true;
}

static bool trans_INY_STA(DisasContext *ctx, arg_INY_STA *a)
{
    return true;
}

static bool trans_ZP_STX (DisasContext *ctx, arg_ZP_STX  *a)
{
    return true;
}

static bool trans_ZPY_STX(DisasContext *ctx, arg_ZPY_STX *a)
{
    return true;
}

static bool trans_ZP_STY (DisasContext *ctx, arg_ZP_STY  *a)
{
    return true;
}

static bool trans_ZPX_STY(DisasContext *ctx, arg_ZPX_STY *a)
{
    return true;
}


static void nes_tr_init_disas_context(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    CPUNESState *env = cs->env_ptr;

    ctx->cs = cs;
    ctx->env = env;
}

static void nes_tr_tb_start(DisasContextBase *db, CPUState *cs)
{
}

static void nes_tr_insn_start(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);

    tcg_gen_insn_start(ctx->pc);
}

static void nes_tr_translate_insn(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    uint32_t insn;

    ctx->pc = ctx->base.pc_next;
    insn = decode_load(ctx);

    if (!decode(ctx, insn)) {
        gen_helper_unsupported(cpu_env);
        ctx->base.is_jmp = DISAS_NORETURN;
    }

    if (ctx->base.is_jmp == DISAS_NEXT) {
        target_ulong page_first = ctx->base.pc_first & TARGET_PAGE_MASK;

        if ((ctx->base.pc_next - page_first) >= TARGET_PAGE_SIZE - 4) {
            ctx->base.is_jmp = DISAS_TOO_MANY;
        }
    }
}

static void nes_tr_tb_stop(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);

    switch (ctx->base.is_jmp) {
    case DISAS_NORETURN:
        break;
    case DISAS_CHAIN:
        gen_goto_tb(ctx, 1, ctx->pc);
        break;
    case DISAS_TOO_MANY:
        tcg_gen_movi_tl(cpu_pc, ctx->pc);
    case DISAS_LOOKUP:
        // 找不到会返回 tcg_code_gen_epilogue，所以不需要 exit
        // lookup_tb_ptr->cpu_get_tb_cpu_state->nes(cpu.c) R_PC
        tcg_gen_lookup_and_goto_ptr();
        break;
    case DISAS_EXIT:
        tcg_gen_exit_tb(NULL, 0);
        break;
    default:
        g_assert_not_reached();
    }
}

static void nes_tr_disas_log(const DisasContextBase *dcbase,
                             CPUState *cs, FILE *logfile)
{
    fprintf(logfile, "IN: %s\n", lookup_symbol(dcbase->pc_first));
    target_disas(logfile, cs, dcbase->pc_first, dcbase->tb->size);
}

static const TranslatorOps nes_tr_ops = {
    .init_disas_context = nes_tr_init_disas_context,
    .tb_start           = nes_tr_tb_start,
    .insn_start         = nes_tr_insn_start,
    .translate_insn     = nes_tr_translate_insn,
    .tb_stop            = nes_tr_tb_stop,
    .disas_log          = nes_tr_disas_log,
};

void gen_intermediate_code(CPUState *cs, TranslationBlock *tb, int *max_insns,
                           target_ulong pc, void *host_pc)
{
    DisasContext dc = { };
    translator_loop(cs, tb, max_insns, pc, host_pc, &nes_tr_ops, &dc.base);
}
