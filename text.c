#include "text.h"
#include "renderer.h"

unsigned short screen[80 * 25];
int screen_cursor_x;
int screen_cursor_y;

static int cursor_on = 1;

void text_refresh(void) {
    SDL_RenderClear(renderer);

    for (int y = 0; y < 25; ++y) {
        for (int x = 0; x < 80; ++x) {
            render_sfont(sfont, screen[y * 80 + x], x, y);
        }
    }

    if (cursor_on) {
        text_draw_cursor(screen_cursor_x, screen_cursor_y);
    }

    SDL_RenderPresent(renderer);
}

void text_refresh_at(int x, int y) {
    render_sfont(sfont, screen[y * 80 + x], x, y);
}

void text_draw_cursor(int x, int y) {
    unsigned short pair = screen[y * 80 + x];
    int fg = cga_colors[(pair >> 8) & 0xf];
    SDL_SetRenderDrawColor(renderer, fg >> 16, (fg >> 8) & 0xff, fg & 0xff, SDL_ALPHA_OPAQUE);
    SDL_Rect rect = { x * FONT_WIDTH, y * FONT_HEIGHT + FONT_HEIGHT - 3, FONT_WIDTH, 2 };
    SDL_RenderFillRect(renderer, &rect);
}

void text_cursor_toggle(void) {
    cursor_on = !cursor_on;
}

/* vim: set sw=4 et: */
