#ifndef QB_H
#define QB_H

#include <SDL2/sdl_keycode.h>

extern int qb_running;

void qb_init(void);
void qb_keypress(SDL_Keycode sym, Uint16 mod);
void qb_mouseclick(int button);

#endif

/* vim: set sw=4 et: */
