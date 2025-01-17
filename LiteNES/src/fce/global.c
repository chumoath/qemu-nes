#include "common.h"
#include "cpu.h"
#include "cpu-internal.h"
#include "fce.h"
#include "mmc.h"
#include "ppu.h"
#include "ppu-internal.h"
#include "memory.h"
CPU_STATE cpu;
byte CPU_RAM[0x8000];
byte op_code;             // Current instruction code
int op_value, op_address; // Arguments for current instruction
int op_cycles;            // Additional instruction cycles used (e.g. when paging occurs)
unsigned long long cpu_cycles;  // Total CPU Cycles Since Power Up (wraps)
void (*cpu_op_address_mode[256])();       // Array of address modes
void (*cpu_op_handler[256])();            // Array of instruction function pointers
bool cpu_op_in_base_instruction_set[256]; // true if instruction is in base 6502 instruction set
char *cpu_op_name[256];                   // Instruction names
char *cpu_address_mode_name[256];         // Instruction address mode names
int cpu_op_cycles[256];                   // CPU cycles used by instructions


const byte cpu_zn_flag_table[256] =
{
  zero_flag,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
  negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,negative_flag,
};


const pal palette[64] = {
	{ 0x80, 0x80, 0x80 },
	{ 0x00, 0x00, 0xBB },
	{ 0x37, 0x00, 0xBF },
	{ 0x84, 0x00, 0xA6 },
	{ 0xBB, 0x00, 0x6A },
	{ 0xB7, 0x00, 0x1E },
	{ 0xB3, 0x00, 0x00 },
	{ 0x91, 0x26, 0x00 },
	{ 0x7B, 0x2B, 0x00 },
	{ 0x00, 0x3E, 0x00 },
	{ 0x00, 0x48, 0x0D },
	{ 0x00, 0x3C, 0x22 },
	{ 0x00, 0x2F, 0x66 },
	{ 0x00, 0x00, 0x00 },
	{ 0x05, 0x05, 0x05 },
	{ 0x05, 0x05, 0x05 },
	{ 0xC8, 0xC8, 0xC8 },
	{ 0x00, 0x59, 0xFF },
	{ 0x44, 0x3C, 0xFF },
	{ 0xB7, 0x33, 0xCC },
	{ 0xFF, 0x33, 0xAA },
	{ 0xFF, 0x37, 0x5E },
	{ 0xFF, 0x37, 0x1A },
	{ 0xD5, 0x4B, 0x00 },
	{ 0xC4, 0x62, 0x00 },
	{ 0x3C, 0x7B, 0x00 },
	{ 0x1E, 0x84, 0x15 },
	{ 0x00, 0x95, 0x66 },
	{ 0x00, 0x84, 0xC4 },
	{ 0x11, 0x11, 0x11 },
	{ 0x09, 0x09, 0x09 },
	{ 0x09, 0x09, 0x09 },
	{ 0xFF, 0xFF, 0xFF },
	{ 0x00, 0x95, 0xFF },
	{ 0x6F, 0x84, 0xFF },
	{ 0xD5, 0x6F, 0xFF },
	{ 0xFF, 0x77, 0xCC },
	{ 0xFF, 0x6F, 0x99 },
	{ 0xFF, 0x7B, 0x59 },
	{ 0xFF, 0x91, 0x5F },
	{ 0xFF, 0xA2, 0x33 },
	{ 0xA6, 0xBF, 0x00 },
	{ 0x51, 0xD9, 0x6A },
	{ 0x4D, 0xD5, 0xAE },
	{ 0x00, 0xD9, 0xFF },
	{ 0x66, 0x66, 0x66 },
	{ 0x0D, 0x0D, 0x0D },
	{ 0x0D, 0x0D, 0x0D },
	{ 0xFF, 0xFF, 0xFF },
	{ 0x84, 0xBF, 0xFF },
	{ 0xBB, 0xBB, 0xFF },
	{ 0xD0, 0xBB, 0xFF },
	{ 0xFF, 0xBF, 0xEA },
	{ 0xFF, 0xBF, 0xCC },
	{ 0xFF, 0xC4, 0xB7 },
	{ 0xFF, 0xCC, 0xAE },
	{ 0xFF, 0xD9, 0xA2 },
	{ 0xCC, 0xE1, 0x99 },
	{ 0xAE, 0xEE, 0xB7 },
	{ 0xAA, 0xF7, 0xEE },
	{ 0xB3, 0xEE, 0xFF },
	{ 0xDD, 0xDD, 0xDD },
	{ 0x11, 0x11, 0x11 },
	{ 0x11, 0x11, 0x11 }
};

byte mmc_id;


PPU_STATE ppu;
byte ppu_latch;
bool ppu_sprite_hit_occured = false;
const word ppu_base_nametable_addresses[4] = { 0x2000, 0x2400, 0x2800, 0x2C00 };
// For sprite-0-hit checks
byte ppu_screen_background[264][248];

// Precalculated tile high and low bytes addition for pattern tables
byte ppu_l_h_addition_table[256][256][8];
byte ppu_l_h_addition_flip_table[256][256][8];


byte PPU_SPRRAM[0x100];
byte PPU_RAM[0x4000];