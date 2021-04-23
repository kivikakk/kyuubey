#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <SDL_render.h>

#include "sfont.h"

int renderer_init(void);
void renderer_quit(void);

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern sdlfont_t *sfont;

#endif

