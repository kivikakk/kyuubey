#include "text.h"
#include "renderer.h"

static unsigned short screen[80 * 25];

void text_init(void) {
    for (int i = 0; i < 80 * 25; ++i) {
        screen[i] = 0x0700;
    }
    
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int i = y * 80 + x;
            screen[i] = (y * 16 + x) | ((y / 2) << (8 + 4)) | (x << 8);
        }
    }
}

void text_refresh(void) {
    SDL_RenderClear(renderer);

    for (int y = 0; y < 25; ++y) {
        for (int x = 0; x < 80; ++x) {
            render_sfont(sfont, screen[y * 80 + x], x, y);
        }
    }

    SDL_RenderPresent(renderer);
}


/* vim: set sw=4 et: */
