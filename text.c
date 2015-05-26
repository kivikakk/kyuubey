#include "text.h"

static unsigned short screen[80 * 25];

void text_init() {
    for (int i = 0; i < 80 * 25; ++i) {
        screen[i] = 0x0720;
    }
}


/* vim: set sw=4 et: */
