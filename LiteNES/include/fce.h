#ifndef FCE_H
#define FCE_H

int fce_load_rom(char *rom);
void fce_init();
void fce_run();
void fce_update_screen();



// Palette

typedef struct __pal {
	int r;
	int g;
	int b;
} pal;

extern const pal palette[64];

#endif
