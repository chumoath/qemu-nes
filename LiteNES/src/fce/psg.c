#include "psg.h"
#include "hal.h"

static byte prev_write;
static byte prev_write2;
static int p = 10;
static int p2 = 10;

inline byte psg_io_read(word address)
{
    // Joystick 1
    if (address == 0x4016 || address == 0x4017) {
        if (p++ < 9) {
            return nes_key_state(p);
        }
    }

    // if (address == 0x4017) {
    //     if (p2++ < 9) {
    //         return nes_key_state2(p);
    //     }
    // }
    return 0;
}

inline void psg_io_write(word address, byte data)
{
    if (address == 0x4016 || address == 0x4017) {
        if ((data & 1) == 0 && prev_write == 1) {
            // strobe
            p = 0;
        }   
    }

    prev_write = data & 1;

    // if (address == 0x4017) {
    //     if ((data & 1) == 0 && prev_write2 == 1) {
    //         // strobe
    //         p2 = 0;
    //     } 
    //     prev_write2 = data & 1;
    // }
}
