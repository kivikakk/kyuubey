#ifndef SFONT_H
#define SFONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

typedef struct {
    SDL_Texture *charset[256];
} sdlfont;

sdlfont *read_ega_sdlfont(const char *filename);
void render_sfont(sdlfont *sfont, unsigned short pair, int x, int y);
void free_sdlfont(sdlfont *sfont);

#endif

/* vim: set sw=4 et: */
