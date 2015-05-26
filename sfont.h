#ifndef SFONT_H
#define SFONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

/* FONT_WIDTH is relatively more involved to change as `rawchar' is defined in
 * terms of `unsigned char'. */
#define FONT_WIDTH  8
#define FONT_HEIGHT 16

typedef struct {
    SDL_Texture *charset[256];
} sdlfont;

sdlfont *read_raw_sdlfont(const char *filename);
void render_sfont(sdlfont *sfont, unsigned short pair, int x, int y);
void free_sdlfont(sdlfont *sfont);

#endif

/* vim: set sw=4 et: */
