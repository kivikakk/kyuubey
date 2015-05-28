#ifndef TEXT_H
#define TEXT_H

void text_init(void);
void text_refresh(void);
void text_refresh_at(int x, int y);
void text_draw_cursor(int x, int y);
void text_cursor_toggle(void);

#endif

/* vim: set sw=4 et: */
