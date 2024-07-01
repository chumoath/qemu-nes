#include "qemu/osdep.h"
#include "qemu/datadir.h"
#include "hw/loader.h"
#include "elf.h"
#include "boot.h"
#include "qemu/error-report.h"

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

typedef struct {
    char signature[4];
    byte prg_block_count;
    byte chr_block_count;
    word rom_type;
    byte reserved[8];
} ines_header;

static ines_header fce_rom_header;
static char rom[1048576];

// FCE Lifecycle

static void romread(char *rom, void *buf, int size)
{
    static int off = 0;
    memcpy(buf, rom + off, size);
    off += size;
}

static fce_load_rom(char *rom)
{
    byte mmc_id;
    int prg_size;
    static byte buf[1048576];

    romread(rom, &fce_rom_header, sizeof(fce_rom_header));

    if (memcmp(fce_rom_header.signature, "NES\x1A", 4)) {
        return -1;
    }

    mmc_id = ((fce_rom_header.rom_type & 0xF0) >> 4);
    prg_size = fce_rom_header.prg_block_count * 0x4000;

    romread(rom, buf, prg_size);

    if (mmc_id == 0 || mmc_id == 3) {
        // if there is only one PRG block, we must repeat it twice
        if (fce_rom_header.prg_block_count == 1) {
            address_space_write(&address_space_memory, 0x8000, MEMTXATTRS_UNSPECIFIED, buf, 0x4000);
            address_space_write(&address_space_memory, 0xC000, MEMTXATTRS_UNSPECIFIED, buf, 0x4000);
        }
        else {
            address_space_write(&address_space_memory, 0x8000, MEMTXATTRS_UNSPECIFIED, buf, 0x8000);
        }
    }
    else {
        return -1;
    }

    // Copying CHR pages into MMC and PPU
    for (int i = 0; i < fce_rom_header.chr_block_count; i++) {
        romread(rom, buf, 0x2000);
        // mmc_id 不是 3，没有用到
        // mmc_append_chr_rom_page(buf);

        if (i == 0) {
            // ppu_copy(0x0000, buf, 0x2000);
        }
    }

    return 0;
}

bool nes_load_firmware(NESCPU *cpu, MachineState *ms, MemoryRegion *program_mr, const char *firmware)
{
    g_autofree char *filename = NULL;
    FILE *fp;
    int nread;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, firmware);
    if (filename == NULL) {
        error_report("Unable to find %s", firmware);
        return false;
    }

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        error_report("Open rom file failed.");
        return false;
    }

    nread = fread(rom, sizeof(rom), 1, fp);
    if (nread == 0 && ferror(fp)) {
        error_report("Read rom file failed.");
        return false;
    }

    if (fce_load_rom(rom) != 0)
    {
        error_report("Invalid or unsupported rom.");
        return false;
    }

    return true;
}
