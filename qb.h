#ifndef QB_H
#define QB_H

#include <SDL2/sdl_keycode.h>

typedef struct doc_line {
    char *line;
    int allocated, stored;
    struct doc_line *next;
} doc_line_t;

extern doc_line_t *active_doc;
extern int cursor_x;
extern int cursor_y;
extern int total_lines;
extern int qb_running;

void qb_init(void);
void qb_keypress(SDL_Keycode sym);
void qb_render(void);

#endif

/* vim: set sw=4 et: */
