#ifndef QB_H
#define QB_H

#include <SDL_keycode.h>

extern int qb_running;

void qb_init(void);
void qb_keydown(SDL_Keycode sym, Uint16 mod);
void qb_keyup(SDL_Keycode sym);
void qb_keypress(SDL_Keycode sym, Uint16 mod);
void qb_mouseclick(int button);

#endif

/* vim: set sw=4 et: */
