#include "renderer.h"
#include "qb.h"
#include "text.h"

static void loop(void);

int main(int argc, char **argv) {
    int r = renderer_init();
    if (r) {
        return r;
    }

    qb_init();

    loop();

    renderer_quit();

    return 0;
}

#define FPS 60  /* Technically: vsync rate */
#define TICK_LENGTH (1000 / FPS)
#define FLIP_CURSOR 16

void loop(void) {
    SDL_Event event;
    Uint32 last_tick = SDL_GetTicks();

    text_refresh();
    int until_flip = FLIP_CURSOR;

    while (1) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    if (event.key.state == SDL_PRESSED) {
                        if (event.key.keysym.sym == SDLK_DOWN && cursor_y < total_lines - 1) {
                            ++cursor_y;
                        } else if (event.key.keysym.sym == SDLK_UP && cursor_y > 0) {
                            --cursor_y;
                        } else if (event.key.keysym.sym == SDLK_LEFT && cursor_x > 0) {
                            --cursor_x;
                        } else if (event.key.keysym.sym == SDLK_RIGHT) {
                            ++cursor_x;
                        }
                        qb_render();
                        text_refresh();
                    }
                    break;
            }
        }

        Uint32 next_tick = last_tick + TICK_LENGTH;
        last_tick = SDL_GetTicks();

        if (last_tick < next_tick) {
            SDL_Delay(next_tick - last_tick);
            last_tick = SDL_GetTicks();
        }

        --until_flip;
        if (until_flip == 0) {
            until_flip = FLIP_CURSOR;
            text_cursor_toggle();
            text_refresh();
        }
    }
}

/* vim: set sw=4 et: */
