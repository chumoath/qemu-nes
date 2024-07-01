#include "qemu/osdep.h"
#include "cpu.h"

typedef struct DisasContext {
    disassemble_info *dis;
    uint32_t addr;
    uint32_t pc;
    uint8_t len;
    uint8_t bytes[8];
} DisasContext;


static uint32_t decode_load_bytes(DisasContext *ctx, uint32_t insn,
                                  int i, int n)
{
    uint32_t addr = ctx->addr;

    g_assert(ctx->len == i);
    g_assert(n <= ARRAY_SIZE(ctx->bytes));

    while (++i <= n) {
        ctx->dis->read_memory_func(addr++, &ctx->bytes[i - 1], 1, ctx->dis);
        insn |= ctx->bytes[i - 1] << (32 - i * 8);
    }
    ctx->addr = addr;
    ctx->len = n;

    return insn;
}

static bool decode_insn(DisasContext *ctx, uint16_t insn);
#include "decode-insn.c.inc"

#define output(mnemonic, format, ...) \
    (pctx->dis->fprintf_func(pctx->dis->stream, "%-9s " format, \
                              mnemonic, ##__VA_ARGS__))


#define INSN(opcode, format, ...)                                       \
static bool trans_##opcode(DisasContext *pctx, arg_##opcode * a)        \
{                                                                       \
    output(#opcode, format, ##__VA_ARGS__);                             \
    return true;                                                        \
}

static int sign_extend(int x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFFFFFF << bit_count);
    }
    return x;
}

int nes_print_insn(bfd_vma addr, disassemble_info *dis)
{
    DisasContext ctx;
    uint32_t insn;
    int i;

    ctx.dis = dis;
    ctx.pc = ctx.addr = addr;
    ctx.len = 0;

    insn = decode_load(&ctx);
    if (!decode(&ctx, insn)) {
        ctx.dis->fprintf_func(ctx.dis->stream, ".byte\t");
        for (i = 0; i < ctx.addr - addr; i++) {
            if (i > 0) {
                ctx.dis->fprintf_func(ctx.dis->stream, ",");
            }
            ctx.dis->fprintf_func(ctx.dis->stream, "0x%02x", insn >> 24);
            insn <<= 8;
        }
    }
    return ctx.addr - addr;
}


INSN(A_ASL  , "")
INSN(A_LSR  , "")
INSN(A_ROL  , "")
INSN(A_ROR  , "")
INSN(DEX    , "")
INSN(DEY    , "")
INSN(INX    , "")
INSN(INY    , "")
INSN(BRK    , "")
INSN(RTI    , "")
INSN(RTS    , "")
INSN(NOP    , "")
INSN(CLC    , "")
INSN(CLD    , "")
INSN(CLI    , "")
INSN(CLV    , "")
INSN(SEC    , "")
INSN(SED    , "")
INSN(SEI    , "")
INSN(PHA    , "")
INSN(PHP    , "")
INSN(PLA    , "")
INSN(PLP    , "")
INSN(TAX    , "")
INSN(TAY    , "")
INSN(TSX    , "")
INSN(TXA    , "")
INSN(TXS    , "")
INSN(TYA    , "")
INSN(IMM_ADC, "")
INSN(IMM_SBC, "")
INSN(IMM_AND, "")
INSN(IMM_EOR, "")
INSN(IMM_ORA, "")
INSN(IMM_CMP, "")
INSN(IMM_CPX, "")
INSN(IMM_LDA, "")
INSN(IMM_LDX, "")
INSN(IMM_LDY, "")
INSN(RE_BCC , "")
INSN(RE_BCS , "")
INSN(RE_BEQ , "")
INSN(RE_BMI , "")
INSN(RE_BNE , "")
INSN(RE_BPL , "")
INSN(RE_BVC , "")
INSN(RE_BVS , "")
INSN(AB_ADC , "")
INSN(ABX_ADC, "")
INSN(ABY_ADC, "")
INSN(AB_SBC , "")
INSN(ABX_SBC, "")
INSN(ABY_SBC, "")
INSN(AB_DEC , "")
INSN(ABX_DEC, "")
INSN(AB_INC , "")
INSN(ABX_INC, "")
INSN(AB_AND , "")
INSN(ABX_AND, "")
INSN(ABY_AND, "")
INSN(AB_ASL , "")
INSN(ABX_ASL, "")
INSN(AB_LSR , "")
INSN(ABX_LSR, "")
INSN(AB_ORA , "")
INSN(ABX_ORA, "")
INSN(ABY_ORA, "")
INSN(AB_EOR , "")
INSN(ABX_EOR, "")
INSN(ABY_EOR, "")
INSN(AB_ROR , "")
INSN(ABX_ROR, "")
INSN(AB_ROL , "")
INSN(ABX_ROL, "")
INSN(AB_JMP , "")
INSN(IN_JMP , "")
INSN(AB_JSR , "")
INSN(AB_LDY , "")
INSN(ABX_LDY, "")
INSN(AB_LDX , "")
INSN(ABY_LDX, "")
INSN(AB_STA , "")
INSN(ABX_STA, "")
INSN(ABY_STA, "")
INSN(AB_STX , "")
INSN(AB_STY , "")
INSN(AB_BIT , "")
INSN(AB_CPX , "")
INSN(AB_CPY , "")
INSN(AB_CMP , "")
INSN(ABX_CMP, "")
INSN(ABY_CMP, "")
INSN(AB_LDA , "")
INSN(ABX_LDA, "")
INSN(ABY_LDA, "")
INSN(ZP_ADC , "")
INSN(ZPX_ADC, "")
INSN(INX_ADC, "")
INSN(INY_ADC, "")
INSN(ZP_SBC , "")
INSN(ZPX_SBC, "")
INSN(INX_SBC, "")
INSN(INY_SBC, "")
INSN(ZP_DEC , "")
INSN(ZPX_DEC, "")
INSN(ZP_INC , "")
INSN(ZPX_INC, "")
INSN(ZP_AND , "")
INSN(ZPX_AND, "")
INSN(INX_AND, "")
INSN(INY_AND, "")
INSN(ZP_ASL , "")
INSN(ZPX_ASL, "")
INSN(ZP_LSR , "")
INSN(ZPX_LSR, "")
INSN(ZP_EOR , "")
INSN(ZPX_EOR, "")
INSN(INX_EOR, "")
INSN(INY_EOR, "")
INSN(ZP_ORA , "")
INSN(ZPX_ORA, "")
INSN(INX_ORA, "")
INSN(INY_ORA, "")
INSN(ZP_ROL , "")
INSN(ZPX_ROL, "")
INSN(ZP_ROR , "")
INSN(ZPX_ROR, "")
INSN(ZP_BIT , "")
INSN(ZP_CMP , "")
INSN(ZPX_CMP, "")
INSN(INX_CMP, "")
INSN(INY_CMP, "")
INSN(ZP_CPX , "")
INSN(ZP_CPY , "")
INSN(ZP_LDA , "")
INSN(ZPX_LDA, "")
INSN(INX_LDA, "")
INSN(INY_LDA, "")
INSN(ZP_LDX , "")
INSN(ZPY_LDX, "")
INSN(ZP_LDY , "")
INSN(ZPX_LDY, "")
INSN(ZP_STA , "")
INSN(ZPX_STA, "")
INSN(INX_STA, "")
INSN(INY_STA, "")
INSN(ZP_STX , "")
INSN(ZPY_STX, "")
INSN(ZP_STY , "")
INSN(ZPX_STY, "")