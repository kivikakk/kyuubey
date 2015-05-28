#ifndef TEXT_H
#define TEXT_H

void text_refresh(void);
void text_refresh_at(int x, int y);
void text_draw_cursor(int x, int y);
void text_cursor_toggle(void);

extern unsigned short screen[80 * 25];
extern int screen_cursor_x;
extern int screen_cursor_y;

#endif

/* vim: set sw=4 et: */
